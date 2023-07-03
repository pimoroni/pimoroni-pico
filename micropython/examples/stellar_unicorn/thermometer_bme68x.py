import time
from stellar import StellarUnicorn
from picographics import PicoGraphics, DISPLAY_STELLAR_UNICORN
from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS
from breakout_bme68x import BreakoutBME68X

"""
Reads the temperature from a BME680 or BME688
... and displays an appropriately coloured pulsing blob.

Buttons:
A - Show temperature
B - Show humidity
C - Show pressure
"""

# The range of readings that we want to map to colours
MIN = 10
MAX = 30

# pick what bits of the colour wheel to use (from 0-360°)
# https://www.cssscript.com/demo/hsv-hsl-color-wheel-picker-reinvented/
HUE_START = 230  # blue
HUE_END = 359  # red

# rainbow party mode
rainbow_orb = False

# set up the Unicron
su = StellarUnicorn()
graphics = PicoGraphics(DISPLAY_STELLAR_UNICORN)

# set up the sensor
i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)
bme = BreakoutBME68X(i2c)

# set up constants and variables for drawing
WIDTH, HEIGHT = graphics.get_bounds()

BLACK = graphics.create_pen(0, 0, 0)
WHITE = graphics.create_pen(255, 255, 255)

forward = True
orb_brightness = 0.5
hue = 0.0
mode = "temperature"

graphics.set_font("bitmap8")

while True:

    if su.is_pressed(StellarUnicorn.SWITCH_A):
        mode = "temperature"
        print(f"mode = {mode}")

    elif su.is_pressed(StellarUnicorn.SWITCH_B):
        mode = "humidity"
        print(f"mode = {mode}")

    elif su.is_pressed(StellarUnicorn.SWITCH_C):
        mode = "pressure"
        print(f"mode = {mode}")

    # read the onboard sensor
    # the following two lines do some maths to convert the number from the temp sensor into celsius
    temperature, pressure, humidity, gas, status, _, _ = bme.read()

    print(f"""
    Temperature: {temperature:.2f} °C
    Humidity: {humidity:.2f} %
    Pressure: {pressure/100:.2f} hPa
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

    # select a pen colour for the text
    # try BLACK for a funky negative space effect
    graphics.set_pen(WHITE)

    if mode == "temperature":
        graphics.text(f"{temperature:.0f}°", 2, 5, scale=1)
        # or uncomment these lines if you'd prefer it in Freedom Units
        # fahrenheit = (temperature * 9 / 5) + 32
        # graphics.text(f"{fahrenheit:.0f}°", 2, 5, scale=1)

    if mode == "humidity":
        graphics.text(f"{humidity:.0f}%", 1, 5, scale=1)

    if mode == "pressure":
        if pressure / 100 < 1000:
            graphics.text(f"{pressure / 100:.0f} hPa", 1, 0, WIDTH, scale=1)
        else:
            pressure_string = str(pressure / 100)
            graphics.text(f"{pressure_string[0]}.{pressure_string[1]}k hPa", 0, 0, WIDTH, scale=1)

    # time to update the display
    su.update(graphics)
    time.sleep(0.1)
