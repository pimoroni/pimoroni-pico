# Add a SCD41 sensor breakout to your GFX Pack to make a handy CO2 detector!
# https://shop.pimoroni.com/products/scd41-co2-sensor-breakout
# Press A to reset the high/low values.

from gfx_pack import GfxPack, SWITCH_A
import breakout_scd41

gp = GfxPack()
display = gp.display

WIDTH, HEIGHT = display.get_bounds()
display.set_backlight(0)  # turn off the white component of the backlight

# the range of readings to map to colours (and scale our graph to)
# https://www.kane.co.uk/knowledge-centre/what-are-safe-levels-of-co-and-co2-in-rooms
MIN = 400
MAX = 2000

# pick what bits of the colour wheel to use (from 0-360°)
# https://www.cssscript.com/demo/hsv-hsl-color-wheel-picker-reinvented/
HUE_START = 100  # green
HUE_END = 0  # red

BRIGHTNESS = 0.5

# the area of the screen we want to draw our graph into
GRAPH_TOP = 24
GRAPH_BOTTOM = 54


# sets up a handy function we can call to clear the screen
def clear():
    display.set_pen(0)
    display.clear()
    display.set_pen(15)


# From CPython Lib/colorsys.py
def hsv_to_rgb(h, s, v):
    if s == 0.0:
        return v, v, v
    i = int(h * 6.0)
    f = (h * 6.0) - i
    p = v * (1.0 - s)
    q = v * (1.0 - s * f)
    t = v * (1.0 - s * (1.0 - f))
    i = i % 6
    if i == 0:
        return v, t, p
    if i == 1:
        return q, v, p
    if i == 2:
        return p, v, t
    if i == 3:
        return p, q, v
    if i == 4:
        return t, p, v
    if i == 5:
        return v, p, q


highest = 0.0
lowest = 4000.0
readings = []

# set up
breakout_scd41.init(gp.i2c)
breakout_scd41.start()

gp.set_backlight(0, 0, 0, 127)
display.set_font("bitmap8")
display.set_pen(15)
display.text("Waiting for sensor to be ready", 0, 0, WIDTH, 2)
display.update()

while True:
    if gp.switch_pressed(SWITCH_A):
        # reset recorded high / low values
        highest = 0.0
        lowest = 4000.0

    if breakout_scd41.ready():
        # read the sensor
        co2, temperature, humidity = breakout_scd41.measure()

        # update highest / lowest values
        if co2 < lowest:
            lowest = co2
        if co2 > highest:
            highest = co2

        # calculates a colour from the sensor reading
        hue = max(0, HUE_START + ((co2 - MIN) * (HUE_END - HUE_START) / (MAX - MIN)))

        # set the leds
        r, g, b = [int(255 * c) for c in hsv_to_rgb(hue / 360, 1.0, BRIGHTNESS)]
        gp.set_backlight(r, g, b, 0)

        # keep track of readings in a list (so we can draw the graph)
        readings.append(co2)
        # we only need to save a screen's worth of readings, so delete the oldest
        if len(readings) > WIDTH:
            readings.pop(0)

        # draw the graph
        clear()
        for r in range(len(readings)):
            # this line scales the y axis of the graph into the available space
            y = round(GRAPH_BOTTOM + ((readings[r] - MIN) * (GRAPH_TOP - GRAPH_BOTTOM) / (MAX - MIN)))
            display.pixel(r, y)

        # draw the text
        display.text("CO2", 0, 0, scale=2)
        display.text(f"Temp {temperature:.0f}°c", 0, 16, scale=1)
        display.text(f"Low {lowest:.0f}ppm", 0, HEIGHT - 8, scale=1)
        # measure the rest of the text before drawing so that we can right align it
        text_width = display.measure_text(f"{co2:.0f}ppm", scale=2)
        display.text(f"{co2:.0f}ppm", WIDTH - text_width, 0, scale=2)
        text_width = display.measure_text(f"Humidity {humidity:.0f}%", scale=1)
        display.text(f"Humidity {humidity:.0f}%", WIDTH - text_width, 16, scale=1)
        text_width = display.measure_text(f"High {highest:.0f}ppm", scale=1)
        display.text(f"High {highest:.0f}ppm", WIDTH - text_width, HEIGHT - 8, scale=1)

        display.update()
