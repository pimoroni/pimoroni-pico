from pimoroni import RGBLED, Button
from picographics import PicoGraphics, DISPLAY_INTERSTATE75_32X32, DISPLAY_INTERSTATE75_64X32, DISPLAY_INTERSTATE75_96X32, DISPLAY_INTERSTATE75_128X32, DISPLAY_INTERSTATE75_64X64, DISPLAY_INTERSTATE75_128X64, DISPLAY_INTERSTATE75_192X64, DISPLAY_INTERSTATE75_256X64
from pimoroni_i2c import PimoroniI2C
import hub75

# Index Constants
SWITCH_A = 0
SWITCH_B = 1


class Interstate75:
    I2C_SDA_PIN = 20
    I2C_SCL_PIN = 21
    SWITCH_PINS = (14, 15)
    LED_R_PIN = 16
    LED_G_PIN = 17
    LED_B_PIN = 18

    # Display Types
    DISPLAY_INTERSTATE75_32X32 = DISPLAY_INTERSTATE75_32X32
    DISPLAY_INTERSTATE75_64X32 = DISPLAY_INTERSTATE75_64X32
    DISPLAY_INTERSTATE75_96X32 = DISPLAY_INTERSTATE75_96X32
    DISPLAY_INTERSTATE75_128X32 = DISPLAY_INTERSTATE75_128X32
    DISPLAY_INTERSTATE75_64X64 = DISPLAY_INTERSTATE75_64X64
    DISPLAY_INTERSTATE75_128X64 = DISPLAY_INTERSTATE75_128X64
    DISPLAY_INTERSTATE75_192X64 = DISPLAY_INTERSTATE75_192X64
    DISPLAY_INTERSTATE75_256X64 = DISPLAY_INTERSTATE75_256X64

    PANEL_GENERIC = hub75.PANEL_GENERIC
    PANEL_FM6126A = hub75.PANEL_FM6126A

    # Count Constants
    NUM_SWITCHES = 2

    def __init__(self, display, panel_type=hub75.PANEL_GENERIC, stb_invert=False):
        self.display = PicoGraphics(display=display)
        self.width, self.height = self.display.get_bounds()
        self.hub75 = hub75.Hub75(self.width, self.height, panel_type=panel_type, stb_invert=stb_invert)
        self.hub75.start()

        # Set up the user switches
        self.__switches = []
        for i in range(self.NUM_SWITCHES):
            self.__switches.append(Button(self.SWITCH_PINS[i]))

        self.__rgb = RGBLED(Interstate75.LED_R_PIN, Interstate75.LED_G_PIN, Interstate75.LED_B_PIN, invert=True)

        # Set up the i2c for Qw/st and Breakout Garden
        self.i2c = PimoroniI2C(self.I2C_SDA_PIN, self.I2C_SCL_PIN, 100000)

    def update(self, buffer=None):
        if buffer is None:
            buffer = self.display
        self.hub75.update(buffer)

    def switch_pressed(self, switch):
        if switch < 0 or switch >= self.NUM_SWITCHES:
            raise ValueError("switch out of range. Expected SWITCH_A (0), SWITCH_B (1)")
        return self.__switches[switch].is_pressed

    def set_led(self, r, g, b):
        self.__rgb.set_rgb(r, g, b)
