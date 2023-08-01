import time
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY

"""
Auto brightness feature for the Cosmic unicorn
Uses the onboard light sensor to detect the light
The brightness percentage is displayed with brightness auto adjusted
"""
# set up unicorn and drawing variables
cu = CosmicUnicorn()
graphics = PicoGraphics(DISPLAY)

WIDTH, HEIGHT = graphics.get_bounds()
BLACK = graphics.create_pen(0, 0, 0)
WHITE = graphics.create_pen(255, 255, 255)
GREY = graphics.create_pen(20, 20, 20)
HUE_START = 0
HUE_END = 100
graphics.set_font("bitmap8")

# Text display sleep time in ms
TEXT_SLEEP = 500


# the onboard light sensor has a wide range from 0 t0 4095
# It is therefore needed to set a lower max and a higher minimum
MIN_LS_VALUE = 10
MAX_LS_VALUE = 295  # 4095 to use the full range
MIN_RANGE = 0.1
MAX_RANGE = 1


# perform linear interpolation to map a range of values to discrete
def map_range(
    x,
    min_input=MIN_LS_VALUE,
    max_input=MAX_LS_VALUE,
    min_output=MIN_RANGE,
    max_output=MAX_RANGE,
):
    return (x - min_input) * (max_output - min_output) / (
        max_input - min_input
    ) + min_output


# gets the light sensor value from onboard sensor and interpolates it
# clamps the brightness values
def calculate_brightness(current_lsv):
    brightness_val = map_range(current_lsv)
    if brightness_val > 1:
        brightness_val = 1
    elif brightness_val < 0.1:
        brightness_val = 0.1

    return brightness_val


# sets up a handy function we can call to clear the screen
def clear():
    graphics.set_pen(BLACK)
    graphics.clear()


def draw_percentage(x, y):
    graphics.rectangle(x + 1, y + 1, 2, 2)
    graphics.line(x, y + 6, x + 6, y)
    graphics.rectangle(x + 4, y + 4, 2, 2)


# draws a sun icon
def draw_sun(x, y, r):
    circle_x = x + 3 + r
    circle_y = y + 3 + r
    graphics.circle(circle_x, circle_y, r)
    graphics.line(circle_x, y, circle_x, y + 2)
    graphics.line(x, circle_y, x + 2, circle_y)
    graphics.line(circle_x, (y + 5 + 2 * r), circle_x, (y + 5 + 2 * r) + 2)
    graphics.line((x + 5 + 2 * r), circle_y, (x + 5 + 2 * r) + 2, circle_y)
    graphics.line(
        circle_x + 1 + r, circle_y - 1 - r, circle_x + 3 + r, circle_y - 3 - r
    )
    graphics.line(
        circle_x + 1 + r, circle_y + 1 + r, circle_x + 3 + r, circle_y + 3 + r
    )
    graphics.line(
        circle_x - 1 - r, circle_y - 1 - r, circle_x - 3 - r, circle_y - 3 - r
    )
    graphics.line(
        circle_x - 1 - r, circle_y + 1 + r, circle_x - 3 - r, circle_y + 3 + r
    )


mode = "auto"
last = time.ticks_ms()
while True:
    current = time.ticks_ms()

    # get light sensor value from the sensor
    ls_value = cu.light()
    brightness_value = calculate_brightness(ls_value)
    cu.set_brightness(brightness_value)
    # calculate brightness percentage
    bp = (brightness_value / MAX_RANGE) * 100

    # deactivate auto brightness by pressing A
    if cu.is_pressed(CosmicUnicorn.SWITCH_A):
        print("Auto brightness off")
        mode = "off"

    # reactivate auto brightness by pressing A
    if cu.is_pressed(CosmicUnicorn.SWITCH_B):
        print("Auto brightness on")
        mode = "auto"

    # set brightness to default value if off
    if mode == "off":
        cu.set_brightness(0.5)

    # set text update rate after a certain time to reduce flickering
    if current - last >= TEXT_SLEEP:
        clear()

        # calculate colour from the brightness value
        hue = max(0, HUE_START + ((bp - 0) * (HUE_END - HUE_START) / (100 - 0)))

        # create pens with this colour (and with the high / low colours)
        CURRENT_COLOUR = graphics.create_pen_hsv(hue / 360, 1.0, 0.8)
        HIGH_COLOUR = graphics.create_pen_hsv(HUE_END / 360, 1.0, 0.8)
        LOW_COLOUR = graphics.create_pen_hsv(HUE_START / 360, 1.0, 0.8)

        # draw the text
        graphics.set_pen(CURRENT_COLOUR)
        graphics.text("BRT ", 0, 1, scale=1)
        draw_percentage(15, 1)
        graphics.text(f"{bp:.0f}", 9, 24, scale=1)
        draw_percentage(20, 24)

        # draw sun icon
        draw_sun(0, 12, 2)

        # draw a bar for the background
        bar_width = WIDTH - 12
        graphics.set_pen(GREY)
        graphics.rectangle(13, 12, bar_width, 10)

        # draw a bar for the current brightness percentage
        graphics.set_pen(CURRENT_COLOUR)
        graphics.rectangle(13, 12, int((bp / 100) * bar_width), 10)

        last = current

    # time to update the display
    cu.update(graphics)
