from pimoroni import RGBLED, Button
from picographics import PicoGraphics, DISPLAY_GFX_PACK
from pimoroni_i2c import PimoroniI2C

# Index Constants
SWITCH_A = 0
SWITCH_B = 1
SWITCH_C = 2
SWITCH_D = 3
SWITCH_E = 4


class GfxPack:
    I2C_SDA_PIN = 4
    I2C_SCL_PIN = 5
    SWITCH_PINS = (12, 13, 14, 15, 22)
    LED_R_PIN = 6
    LED_G_PIN = 7
    LED_B_PIN = 8

    # Count Constants
    NUM_SWITCHES = 5

    def __init__(self):
        self.display = PicoGraphics(display=DISPLAY_GFX_PACK)

        # Set up the user switches
        self.__switches = []
        for i in range(self.NUM_SWITCHES):
            self.__switches.append(Button(self.SWITCH_PINS[i]))

        self.__rgb = RGBLED(GfxPack.LED_R_PIN, GfxPack.LED_G_PIN, GfxPack.LED_B_PIN, invert=False)

        # Set up the i2c for Qw/st and Breakout Garden
        self.i2c = PimoroniI2C(self.I2C_SDA_PIN, self.I2C_SCL_PIN, 100000)

    def switch_pressed(self, switch):
        if switch < 0 or switch >= self.NUM_SWITCHES:
            raise ValueError("switch out of range. Expected SWITCH_A (0), SWITCH_B (1),  SWITCH_C (2),  SWITCH_D (3), or  SWITCH_E (4)")
        return self.__switches[switch].is_pressed

    def set_backlight(self, r, g, b, w=None):
        self.__rgb.set_rgb(r, g, b)
        if w is not None:
            self.display.set_backlight(w / 255)
