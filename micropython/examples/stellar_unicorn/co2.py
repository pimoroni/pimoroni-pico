# Add a SCD41 sensor breakout to your Stellar Unicorn to make a handy CO2 detector!
# https://shop.pimoroni.com/products/scd41-co2-sensor-breakout
# Press A for CO2, B for temperature and C for humidity
# This example uses a custom tiny font - find 3x5.bitmapfont in /common/fonts

from stellar import StellarUnicorn
from picographics import PicoGraphics, DISPLAY_STELLAR_UNICORN as DISPLAY
import breakout_scd41
from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS

su = StellarUnicorn()
graphics = PicoGraphics(DISPLAY)
i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)

# the range of readings to map to colours
# https://www.kane.co.uk/knowledge-centre/what-are-safe-levels-of-co-and-co2-in-rooms
MIN = 400
MAX = 2000

# pick what bits of the colour wheel to use (from 0-360°)
# https://www.cssscript.com/demo/hsv-hsl-color-wheel-picker-reinvented/
HUE_START = 100  # green
HUE_END = 0  # red

# some pen colours to use
BLACK = graphics.create_pen(0, 0, 0)
WHITE = graphics.create_pen(255, 255, 255)

# some other variables to keep track of stuff
mode = "co2"
flashy_light = False


# sets up a handy function we can call to clear the screen
def clear():
    graphics.set_pen(BLACK)
    graphics.clear()


# set up
breakout_scd41.init(i2c)
breakout_scd41.start()

graphics.set_font(open("3x5.bitmapfont", "rb").read())
graphics.set_pen(WHITE)
graphics.text("WAIT", 0, 0, scale=1)
su.update(graphics)

while True:

    if su.is_pressed(StellarUnicorn.SWITCH_A):
        mode = "co2"
    if su.is_pressed(StellarUnicorn.SWITCH_B):
        mode = "temp"
    if su.is_pressed(StellarUnicorn.SWITCH_C):
        mode = "humidity"

    if breakout_scd41.ready():
        clear()

        # read the sensor
        co2, temperature, humidity = breakout_scd41.measure()

        # calculate a colour from the co2 reading
        hue = max(0, HUE_START + ((co2 - MIN) * (HUE_END - HUE_START) / (MAX - MIN)))

        # draw the border and background
        graphics.set_pen(graphics.create_pen_hsv((hue / 360), 1.0, 0.8))
        graphics.clear()
        graphics.set_pen(graphics.create_pen_hsv((hue / 360), 1.0, 0.2))
        graphics.rectangle(1, 1, 14, 14)

        graphics.set_pen(WHITE)
        if mode == "co2":
            # draw the co2 level
            co2_string = str(co2)
            if co2 < 1000:
                graphics.text(f"{co2:.0f}", 2, 2, scale=1, fixed_width=True)
            else:
                graphics.text(f"{co2_string[0]}", 1, 2, scale=1, fixed_width=True)
                graphics.text(f"{co2_string[1]}K", 7, 2, scale=1, fixed_width=True)
                graphics.pixel(5, 6)

            graphics.text("PPM", 3, 9, scale=1)

        if mode == "temp":
            # draw the temperature
            graphics.text("T:", 2, 2, scale=1)
            graphics.text(f"{temperature:.0f}°", 2, 8, scale=1, fixed_width=True)

        if mode == "humidity":
            # draw the temperature
            graphics.text("H:", 2, 2, scale=1)
            graphics.text(f"{humidity:.0f}%", 2, 8, scale=1, fixed_width=True)

        # flash the top right pixel to show the sensor's been read
        if flashy_light is False:
            graphics.pixel(15, 0)
            flashy_light = True
        else:
            flashy_light = False

        su.update(graphics)
