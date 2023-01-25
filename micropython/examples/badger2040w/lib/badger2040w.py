import machine
import micropython
from picographics import PicoGraphics, DISPLAY_INKY_PACK
import network
from network_manager import NetworkManager
import WIFI_CONFIG
import uasyncio
import time
import gc
import wakeup


BUTTON_DOWN = 11
BUTTON_A = 12
BUTTON_B = 13
BUTTON_C = 14
BUTTON_UP = 15
BUTTON_USER = None  # User button not available on W

BUTTON_MASK = 0b11111 << 11

SYSTEM_VERY_SLOW = 0
SYSTEM_SLOW = 1
SYSTEM_NORMAL = 2
SYSTEM_FAST = 3
SYSTEM_TURBO = 4

UPDATE_NORMAL = 0
UPDATE_MEDIUM = 1
UPDATE_FAST = 2
UPDATE_TURBO = 3

LED = 22
ENABLE_3V3 = 10
BUSY = 26

WIDTH = 296
HEIGHT = 128

SYSTEM_FREQS = [
    4000000,
    12000000,
    48000000,
    133000000,
    250000000
]

WAKEUP_GPIO_STATE = wakeup.get_gpio_state()

BUTTONS = {
    BUTTON_DOWN: machine.Pin(BUTTON_DOWN, machine.Pin.IN, machine.Pin.PULL_DOWN),
    BUTTON_A: machine.Pin(BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN),
    BUTTON_B: machine.Pin(BUTTON_B, machine.Pin.IN, machine.Pin.PULL_DOWN),
    BUTTON_C: machine.Pin(BUTTON_C, machine.Pin.IN, machine.Pin.PULL_DOWN),
    BUTTON_UP: machine.Pin(BUTTON_UP, machine.Pin.IN, machine.Pin.PULL_DOWN),
}


def woken_by_button():
    return WAKEUP_GPIO_STATE & BUTTON_MASK > 0


def pressed_to_wake(button):
    return WAKEUP_GPIO_STATE & (1 << button) > 0


def system_speed(speed):
    try:
        machine.freq(SYSTEM_FREQS[speed])
    except IndexError:
        pass


class Badger2040W():
    def __init__(self):
        self.display = PicoGraphics(DISPLAY_INKY_PACK)
        self._led = machine.PWM(machine.Pin(LED))
        self._led.freq(1000)
        self._led.duty_u16(0)
        self._update_speed = 0

    def __getattr__(self, item):
        # Glue to redirect calls to PicoGraphics
        return getattr(self.display, item)

    def update(self):
        t_start = time.ticks_ms()
        self.display.update()
        t_elapsed = time.ticks_ms() - t_start

        delay_ms = [4700, 2600, 900, 250][self._update_speed]

        if t_elapsed < delay_ms:
            time.sleep((delay_ms - t_elapsed) / 1000)

    def set_update_speed(self, speed):
        self.display.set_update_speed(speed)
        self._update_speed = speed

    def led(self, brightness):
        brightness = max(0, min(255, brightness))
        self._led.duty_u16(int(brightness * 256))

    def invert(self, invert):
        raise RuntimeError("Display invert not supported in PicoGraphics.")

    def thickness(self, thickness):
        raise RuntimeError("Thickness not supported in PicoGraphics.")

    def halt(self):
        enable = machine.Pin(ENABLE_3V3, machine.Pin.OUT)
        enable.off()
        while not self.pressed_any():
            pass

    def pressed(self, button):
        return BUTTONS[button].value() == 1

    def pressed_any(self):
        for button in BUTTONS.values():
            if button.value():
                return True
        return False

    @micropython.native
    def icon(self, data, index, data_w, icon_size, x, y):
        s_x = (index * icon_size) % data_w
        s_y = int((index * icon_size) / data_w)

        for o_y in range(icon_size):
            for o_x in range(icon_size):
                o = ((o_y + s_y) * data_w) + (o_x + s_x)
                bm = 0b10000000 >> (o & 0b111)
                if data[o >> 3] & bm:
                    self.display.pixel(x + o_x, y + o_y)

    def image(self, data, w, h, x, y):
        for oy in range(h):
            row = data[oy]
            for ox in range(w):
                if row & 0b1 == 0:
                    self.display.pixel(x + ox, y + oy)
                row >>= 1

    def status_handler(self, mode, status, ip):
        print(mode, status, ip)
        self.display.set_pen(15)
        self.display.clear()
        self.display.set_pen(0)
        if status:
            self.display.text("Connected!", 10, 10, 300, 0.5)
            self.display.text(ip, 10, 30, 300, 0.5)
        else:
            self.display.text("Connecting...", 10, 10, 300, 0.5)
        self.display.update()

    def isconnected(self):
        return network.WLAN(network.STA_IF).isconnected()

    def ip_address(self):
        return network.WLAN(network.STA_IF).ifconfig()[0]

    def connect(self):
        if WIFI_CONFIG.COUNTRY == "":
            raise RuntimeError("You must populate WIFI_CONFIG for networking.")
        self.display.set_update_speed(2)
        network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=self.status_handler)
        uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
        gc.collect()
