import time
from machine import Pin, PWM, ADC


BREAKOUT_GARDEN_I2C_PINS = {"sda": 4, "scl": 5}
PICO_EXPLORER_I2C_PINS = {"sda": 20, "scl": 21}
HEADER_I2C_PINS = {"sda": 20, "scl": 21}
PICOVISION_I2C_PINS = {"sda": 6, "scl": 7}

# Motor and encoder directions
NORMAL_DIR = 0x00
REVERSED_DIR = 0x01

BREAKOUT_GARDEN_SPI_SLOT_FRONT = 0
BREAKOUT_GARDEN_SPI_SLOT_BACK = 1
PICO_EXPLORER_SPI_ONBOARD = 2


class Analog:
    def __init__(self, pin, amplifier_gain=1, resistor=0, offset=0):
        self.gain = amplifier_gain
        self.resistor = resistor
        self.offset = offset
        self.pin = ADC(pin)

    def read_voltage(self):
        return max((((self.pin.read_u16() * 3.3) / 65535) + self.offset) / self.gain, 0.0)

    def read_current(self):
        if self.resistor > 0:
            return self.read_voltage() / self.resistor
        else:
            return self.read_voltage()


class AnalogMux:
    def __init__(self, addr0, addr1=None, addr2=None, en=None, muxed_pin=None):
        self.addr0_pin = Pin(addr0, Pin.OUT)
        self.addr1_pin = Pin(addr1, Pin.OUT) if addr1 is not None else None
        self.addr2_pin = Pin(addr2, Pin.OUT) if addr2 is not None else None
        self.en_pin = Pin(en, Pin.OUT) if en is not None else None
        self.max_address = 0b001
        if addr1 is not None:
            self.max_address = 0b011
            if addr2 is not None:
                self.max_address = 0b111
        self.pulls = [None] * (self.max_address + 1)
        self.muxed_pin = muxed_pin

    def select(self, address):
        if address < 0:
            raise ValueError("address is less than zero")
        elif address > self.max_address:
            raise ValueError("address is greater than number of available addresses")
        else:
            if self.muxed_pin and self.pulls[address] is None:
                self.muxed_pin.init(Pin.IN, None)

            self.addr0_pin.value(address & 0b001)

            if self.addr1_pin is not None:
                self.addr1_pin.value(address & 0b010)

            if self.addr2_pin is not None:
                self.addr2_pin.value(address & 0b100)

            if self.en_pin is not None:
                self.en_pin.value(1)

            if self.muxed_pin and self.pulls[address] is not None:
                self.muxed_pin.init(Pin.IN, self.pulls[address])

    def disable(self):
        if self.en_pin is not None:
            self.en_pin.value(0)
        else:
            raise RuntimeError("there is no enable pin assigned to this mux")

    def configure_pull(self, address, pull=None):
        if address < 0:
            raise ValueError("address is less than zero")
        elif address > self.max_address:
            raise ValueError("address is greater than number of available addresses")
        else:
            self.pulls[address] = pull

    def read(self):
        if self.muxed_pin is not None:
            return self.muxed_pin.value()
        else:
            raise RuntimeError("there is no muxed pin assigned to this mux")


class Button:
    def __init__(self, button, invert=True, repeat_time=200, hold_time=1000):
        self.invert = invert
        self.repeat_time = repeat_time
        self.hold_time = hold_time
        self.pin = Pin(button, pull=Pin.PULL_UP if invert else Pin.PULL_DOWN)
        self.last_state = False
        self.pressed = False
        self.pressed_time = 0

    def read(self):
        current_time = time.ticks_ms()
        state = self.raw()
        changed = state != self.last_state
        self.last_state = state

        if changed:
            if state:
                self.pressed_time = current_time
                self.pressed = True
                self.last_time = current_time
                return True
            else:
                self.pressed_time = 0
                self.pressed = False
                self.last_time = 0

        if self.repeat_time == 0:
            return False

        if self.pressed:
            repeat_rate = self.repeat_time
            if self.hold_time > 0 and current_time - self.pressed_time > self.hold_time:
                repeat_rate /= 3
            if current_time - self.last_time > repeat_rate:
                self.last_time = current_time
                return True

        return False

    def raw(self):
        if self.invert:
            return not self.pin.value()
        else:
            return self.pin.value()

    @property
    def is_pressed(self):
        return self.raw()


class RGBLED:
    def __init__(self, r, g, b, invert=True):
        self.invert = invert
        self.led_r = PWM(Pin(r))
        self.led_r.freq(1000)
        self.led_g = PWM(Pin(g))
        self.led_g.freq(1000)
        self.led_b = PWM(Pin(b))
        self.led_b.freq(1000)

    def set_rgb(self, r, g, b):
        if self.invert:
            r = 255 - r
            g = 255 - g
            b = 255 - b
        self.led_r.duty_u16(int((r * 65535) / 255))
        self.led_g.duty_u16(int((g * 65535) / 255))
        self.led_b.duty_u16(int((b * 65535) / 255))


# A simple class for handling Proportional, Integral & Derivative (PID) control calculations
class PID:
    def __init__(self, kp, ki, kd, sample_rate):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.setpoint = 0
        self._error_sum = 0
        self._last_value = 0
        self._sample_rate = sample_rate

    def calculate(self, value, value_change=None):
        error = self.setpoint - value
        self._error_sum += error * self._sample_rate
        if value_change is None:
            rate_error = (value - self._last_value) / self._sample_rate
        else:
            rate_error = value_change
        self._last_value = value

        return (error * self.kp) + (self._error_sum * self.ki) - (rate_error * self.kd)


class Buzzer:
    def __init__(self, pin):
        self.pwm = PWM(Pin(pin))

    def set_tone(self, freq, duty=0.5):
        if freq < 50.0:  # uh... https://github.com/micropython/micropython/blob/af64c2ddbd758ab6bac0fcca94c66d89046663be/ports/rp2/machine_pwm.c#L105-L119
            self.pwm.duty_u16(0)
            return False

        self.pwm.freq(freq)
        self.pwm.duty_u16(int(65535 * duty))
        return True


class ShiftRegister:
    def __init__(self, clk, lat, dat):
        self.clk = Pin(clk, Pin.OUT)
        self.lat = Pin(lat, Pin.OUT)
        self.dat = Pin(dat, Pin.IN)

    def __iter__(self):
        self.lat.off()
        self.lat.on()
        for _ in range(8):
            yield self.dat.value()
            self.clk.on()
            self.clk.off()

    def __getitem__(self, k):
        return list(self)[k]

    def read(self):
        out = 0
        for bit in self:
            out <<= 1
            out += bit
        return out

    def is_set(self, mask):
        return self.read() & mask == mask


# A basic wrapper for PWM with regular on/off and toggle functions from Pin
# Intended to be used for driving LEDs with brightness control & compatibility with Pin
class PWMLED:
    def __init__(self, pin, invert=False):
        self._invert = invert
        self._led = PWM(Pin(pin, Pin.OUT))
        self._led.freq(1000)
        self._brightness = 0
        self.brightness(0)

    def brightness(self, brightness):
        brightness = min(1.0, max(0.0, brightness))
        self._brightness = brightness
        if self._invert:
            brightness = 1.0 - brightness
        self._led.duty_u16(int(65535 * brightness))

    def on(self):
        self.brightness(1)

    def off(self):
        self.brightness(0)

    def toggle(self):
        self.brightness(1 - self._brightness)
