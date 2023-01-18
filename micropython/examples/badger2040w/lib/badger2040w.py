import machine
import micropython
from picographics import PicoGraphics, DISPLAY_INKY_PACK
from network_manager import NetworkManager
import WIFI_CONFIG
import uasyncio
import gc
import wakeup


BUTTON_UP = 15
BUTTON_DOWN = 11
BUTTON_A = 12
BUTTON_B = 13
BUTTON_C = 14
BUTTON_USER = 23

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
    11: machine.Pin(11),
    12: machine.Pin(12),
    13: machine.Pin(13),
    14: machine.Pin(14),
    15: machine.Pin(15),
    23: machine.Pin(23)
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

    def __getattr__(self, item):
        # Glue to redirect calls to PicoGraphics
        return getattr(self.display, item)

    def led(self, brightness):
        pass

    def invert(self, invert):
        pass

    def thickness(self, thickness):
        print("Thickness!")

    def halt(self):
        pass

    def pressed(self, button):
        return BUTTONS[button].value() == 1

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

    def connect(self):
        self.display.set_update_speed(2)
        network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=self.status_handler)
        uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
        gc.collect()
