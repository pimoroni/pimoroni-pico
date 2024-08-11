# Add a SCD41 sensor breakout to your Galactic Unicorn to make a handy CO2 detector!
# https://shop.pimoroni.com/products/scd41-co2-sensor-breakout
# Press A to reset the high/low values.

from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN as DISPLAY
import breakout_scd41
from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS

gu = GalacticUnicorn()
graphics = PicoGraphics(DISPLAY)
i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)

WIDTH, HEIGHT = graphics.get_bounds()

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
GREY = graphics.create_pen(20, 20, 20)


# sets up a handy function we can call to clear the screen
def clear():
    graphics.set_pen(BLACK)
    graphics.clear()


# some variables to keep track of high / low readings
highest = 0.0
lowest = 4000.0

# set up
breakout_scd41.init(i2c)
breakout_scd41.start()

graphics.set_font("bitmap8")
graphics.set_pen(WHITE)
graphics.text("Waiting!", 0, 0, scale=1)
gu.update(graphics)

while True:
    if gu.is_pressed(GalacticUnicorn.SWITCH_A):
        # reset recorded high / low values
        highest = 0.0
        lowest = 4000.0

    if breakout_scd41.ready():
        clear()
        # read the sensor
        co2, temperature, humidity = breakout_scd41.measure()

        # update highest / lowest values
        if co2 < lowest:
            lowest = co2
        if co2 > highest:
            highest = co2

        # calculate a colour from the co2 reading
        hue = max(0, HUE_START + ((co2 - MIN) * (HUE_END - HUE_START) / (MAX - MIN)))

        # create pens with this colour (and with the high / low colours)
        CURRENT_COLOUR = graphics.create_pen_hsv(hue / 360, 1.0, 0.8)
        HIGH_COLOUR = graphics.create_pen_hsv(HUE_END / 360, 1.0, 0.8)
        LOW_COLOUR = graphics.create_pen_hsv(HUE_START / 360, 1.0, 0.8)

        # draw the text
        graphics.set_pen(CURRENT_COLOUR)
        graphics.text("CO2", 0, 0, scale=1)
        # measure the rest of the text before drawing so that we can right align it
        text_width = graphics.measure_text(f"{co2:.0f}ppm", scale=1)
        graphics.text(f"{co2:.0f}ppm", WIDTH - text_width, 0, scale=1)

        # draw a bar for the background
        graphics.set_pen(GREY)
        graphics.rectangle(0, 9, WIDTH, 10)

        # draw a bar for the current co2 level
        graphics.set_pen(CURRENT_COLOUR)
        graphics.rectangle(0, 9, int(co2 / WIDTH), 10)

        # draw a bar for the highest co2 level seen
        graphics.set_pen(HIGH_COLOUR)
        graphics.pixel(int(highest / WIDTH), 9)
        graphics.pixel(int(highest / WIDTH), 10)

        # draw a bar for the lowest co2 level seen
        graphics.set_pen(LOW_COLOUR)
        graphics.pixel(int(lowest / WIDTH), 9)
        graphics.pixel(int(lowest / WIDTH), 10)

        gu.update(graphics)
