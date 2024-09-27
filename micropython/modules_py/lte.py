from machine import UART, Pin
from network import PPP
from micropython import const
import time


DEFAULT_PIN_RST = 35
DEFAULT_PIN_NETLIGHT = 34
DEFAULT_PIN_RX = 33
DEFAULT_PIN_TX = 32
DEFAULT_UART_ID = 0

DEFAULT_UART_TIMEOUT = const(1)
DEFAULT_UART_TIMEOUT_CHAR = const(1)
DEFAULT_UART_RXBUF = const(1024)
DEFAULT_UART_STARTUP_BAUD = const(115200)
DEFAULT_UART_BAUD = const(460800)


class CellularError(Exception):
  def __init__(self, message=None):
    self.message = "CellularError: " + message


class LTE():
    def __init__(self, apn, uart=None, reset_pin=None, netlight_pin=None, netlight_led=None, skip_reset=False):
        self._apn = apn
        self._reset = reset_pin or Pin(DEFAULT_PIN_RST, Pin.OUT)
        self._uart = uart or UART(
            DEFAULT_UART_ID,
            tx=Pin(DEFAULT_PIN_TX, Pin.OUT),
            rx=Pin(DEFAULT_PIN_RX, Pin.OUT))
        
        # Set PPP timeouts and rxbuf
        self._uart.init(
            timeout=DEFAULT_UART_TIMEOUT,
            timeout_char=DEFAULT_UART_TIMEOUT_CHAR,
            rxbuf=DEFAULT_UART_RXBUF)
        
        if not skip_reset:
            self._reset.value(0)
            time.sleep(1.0)
            self._reset.value(1)

        if netlight_led:
            self._led = netlight_led
            self._netlight = netlight_pin or Pin(DEFAULT_PIN_NETLIGHT, Pin.IN)
            self._netlight.irq(self._netlight_irq)
        
    def _netlight_irq(self, pin):
        self._led.value(pin.value())

    def ipconfig(self, *args, **kwargs):
        if len(args):
            return self._ppp.ipconfig(*args)
        else:
            return self._ppp.ipconfig(**kwargs)

    def iccid(self):
        try:
            return self._send_at_command("AT+CICCID", 1)
        except CellularError:
           return None

    def status(self):
        lte_status = self._send_at_command("AT+CEREG?", 1)
        gsm_status = self._send_at_command("AT+CGREG?", 1)
        return lte_status, gsm_status
    
    def signal_quality(self):
        try:
            response = self._send_at_command("AT+CSQ", 1)
            quality = int(response.split(":")[1].split(",")[0])
            db = -113 + (2 * quality) # conversion as per AT command set datasheet
            return db
        except CellularError:
            pass
        return None
    
    def stop_ppp(self):
        self._ppp.disconnect()
        self._send_at_command(f"AT+IPR={DEFAULT_UART_STARTUP_BAUD}")
        self._flush_uart()

    def start_ppp(self, baudrate=DEFAULT_UART_BAUD, connect=True):
        self._wait_ready(poll_time=1.0, timeout=30)

        # Switch to a faster baudrate
        self._send_at_command(f"AT+IPR={baudrate}")
        self._flush_uart()
        self._uart.init(
            baudrate=baudrate,
            timeout=DEFAULT_UART_TIMEOUT,
            timeout_char=DEFAULT_UART_TIMEOUT_CHAR,
            rxbuf=DEFAULT_UART_RXBUF)
        self._wait_ready(poll_time=1.0)

        # Connect!
        if connect:
            self.connect()

        # This will just always time out!?
        # try:
        #    self._send_at_command("ATD*99#", timeout=300)
        # except CellularError as e:
        #    print(e)

        # Force PPP to use modem's default settings...
        #time.sleep(2.0)
        self._flush_uart()
        self._uart.write("ATD*99#\r")
        self._uart.flush()
        #time.sleep(2.0)

        self._ppp = PPP(self._uart)
        self._ppp.connect()
        while self._ppp.status() != 4:
            time.sleep(1.0)

        return self._ppp.ifconfig()

    def connect(self, timeout=60):
        print("  - setting up cellular uart")
        # connect to and flush the uart
        # consume any unsolicited messages first, we don't need those  
        self._flush_uart()

        print("  - waiting for cellular module to be ready")

        # wait for the cellular module to respond to AT commands
        self._wait_ready()   

        self._send_at_command("ATE0") # disable local echo  
        self._send_at_command(f"AT+CGDCONT=1,\"IP\",\"{self._apn}\"") # set apn and activate pdp context  

        # wait for roaming lte connection to be established
        giveup = time.time() + timeout
        status = None
        while status != "+CEREG: 0,5" and status != "+CEREG: 0,1":
            status = self._send_at_command("AT+CEREG?", 1)
            time.sleep(0.25)
            if time.time() > giveup:
                raise CellularError("timed out getting network registration")    

        # disable server and client certification validation
        self._send_at_command("AT+CSSLCFG=\"authmode\",0,0") 
        self._send_at_command("AT+CSSLCFG=\"enableSNI\",0,1")

        print(f"  - SIM ICCID is {self.iccid()}")

    def _wait_ready(self, poll_time=0.25, timeout=10):
        giveup = time.time() + timeout
        while time.time() <= giveup:
            try:
                self._send_at_command("AT")
                return # if __send_at_command doesn't throw an exception then we're good!
            except CellularError as e:
                print(e)
                time.sleep(poll_time)

        raise CellularError("timed out waiting for AT response")  

    def _flush_uart(self):
        self._uart.flush()
        time.sleep(0.25)
        while self._uart.any():
            self._uart.read(self._uart.any())
            time.sleep(0.25)

    def _send_at_command(self, command, result_lines=0, timeout=5.0):
        # consume any unsolicited messages first, we don't need those    
        self._flush_uart()

        self._uart.write(command + "\r")
        #print(f"  - tx: {command}")
        self._uart.flush()
        status, data = self._read_result(result_lines, timeout=timeout)

        print("  -", command, status, data)

        if status == "TIMEOUT":
            #print.error("  !", command, status, data)
            raise CellularError(f"cellular module timed out for command {command}")

        if status not in ["OK", "DOWNLOAD"]:
            #print("  !", command, status, data)
            raise CellularError(f"non 'OK' or 'DOWNLOAD' result for command {command}")

        if result_lines == 1:
            return data[0]
        if result_lines > 1:
            return data  
        return None

    def _read_result(self, result_lines, timeout=1.0):
        status = None
        result = []
        start = time.ticks_ms()
        timeout *= 1000
        while len(result) < result_lines or status is None:
            if (time.ticks_ms() - start) > timeout:
                return "TIMEOUT", []

            line = self._uart.readline()

            if line:
                line = line.strip()
                if line in [b"OK", b"ERROR", b"DOWNLOAD"]:
                    status = line.decode("ascii")
                elif line != b"":
                    result.append(str(line, "ascii"))
                start = time.ticks_ms()

        return status, result

