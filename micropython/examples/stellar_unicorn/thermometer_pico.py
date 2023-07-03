from machine import ADC
import time
from stellar import StellarUnicorn
from picographics import PicoGraphics, DISPLAY_STELLAR_UNICORN

"""
Reads the internal temperature sensor on the Pico W...
... and displays an appropriately coloured pulsing blob.
"""

# The range of readings that we want to map to colours
MIN = 15
MAX = 35

# pick what bits of the colour wheel to use (from 0-360°)
# https://www.cssscript.com/demo/hsv-hsl-color-wheel-picker-reinvented/
HUE_START = 230  # blue
HUE_END = 359  # red

# rainbow party mode
rainbow_orb = False

# set up the Unicron
su = StellarUnicorn()
graphics = PicoGraphics(DISPLAY_STELLAR_UNICORN)

# set up the ADC
sensor_temp = ADC(ADC.CORE_TEMP)
conversion_factor = 3.3 / 65535  # used for calculating a temperature from the raw sensor reading

# set up constants and variables for drawing
WIDTH, HEIGHT = graphics.get_bounds()

BLACK = graphics.create_pen(0, 0, 0)
WHITE = graphics.create_pen(255, 255, 255)

forward = True
orb_brightness = 0.5
hue = 0.0

graphics.set_font("bitmap8")

while True:

    # read the onboard sensor
    # the following two lines do some maths to convert the number from the temp sensor into celsius
    reading = sensor_temp.read_u16() * conversion_factor
    temperature = 27 - (reading - 0.706) / 0.001721

    print(f"""
    Temperature: {temperature:.2f} °C
    """)

    # fills the screen with black
    graphics.set_pen(BLACK)
    graphics.clear()

    # draw a weird orb:
    # three overlapping circles with varying saturations
    if rainbow_orb is True:
        graphics.set_pen(graphics.create_pen_hsv((hue / 360), 0.5, orb_brightness))
        graphics.circle(8, 8, 7)
        graphics.set_pen(graphics.create_pen_hsv((hue / 360), 0.7, orb_brightness))
        graphics.circle(7, 7, 7)
        graphics.set_pen(graphics.create_pen_hsv((hue / 360), 1.0, orb_brightness))
        graphics.circle(7, 7, 5)
        hue += 0.01 * 360
    else:
        # calculate a colour from the temperature
        hue = max(0, HUE_START + ((temperature - MIN) * (HUE_END - HUE_START) / (MAX - MIN)))
        graphics.set_pen(graphics.create_pen_hsv((hue / 360), 0.6, orb_brightness))
        graphics.circle(8, 8, 7)
        graphics.set_pen(graphics.create_pen_hsv((hue / 360), 0.8, orb_brightness))
        graphics.circle(7, 7, 7)
        graphics.set_pen(graphics.create_pen_hsv((hue / 360), 1.0, orb_brightness))
        graphics.circle(7, 7, 5)

    # pulse the orb!
    if forward is True:
        orb_brightness += 0.01
    if orb_brightness >= 0.7:
        orb_brightness = 0.7
        forward = False

    if forward is False:
        orb_brightness -= 0.01
    if orb_brightness <= 0.3:
        orb_brightness = 0.3
        forward = True

    # draw the temperature
    # try BLACK for a funky negative space effect
    graphics.set_pen(WHITE)
    graphics.text(f"{temperature:.0f}°", 2, 5, scale=1)

    # or uncomment these lines if you'd prefer it in Freedom Units
    # graphics.set_pen(WHITE)
    # fahrenheit = (temperature * 9 / 5) + 32
    # graphics.text(f"{fahrenheit:.0f}°", 2, 5, scale=1)

    # time to update the display
    su.update(graphics)
    time.sleep(0.1)
