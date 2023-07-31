import time
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY

"""
Light sensoring feature for the cosmic unicorn
Uses the onboard light sensor to detect the light in the environment
The brightness level is displayed as percentage.
Brightness of the display is auto adjusted to the brightness level of the environment
Press A to turn auto brightness on
Press B to turn auto brightness off
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

# Rate of display change i.e the lower the value the slower the transition
TRANSITION_RATE = 1.0 / 72.0


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
# clamps the brightness value it outside the ranges specified
def calculate_brightness(prev_brightness_val):
    current_lsv = cu.light()
    current_brightness_val = map_range(current_lsv)

    # uses the previous value to smooth out display changes reducing flickering
    brightness_diff = current_brightness_val - prev_brightness_val
    brightness_val = prev_brightness_val + (brightness_diff * TRANSITION_RATE)
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
    graphics.line(x + 1, y + 5, x + 6, y)
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

brightness_value = MIN_RANGE  # set the initial brightness level to the minimum
while True:
    current = time.ticks_ms()

    # set the display brightness
    brightness_value = calculate_brightness(brightness_value)
    cu.set_brightness(brightness_value)

    bp = (brightness_value / MAX_RANGE) * 100  # gets brightness value in percentage relative to the MAX_LS_VALUE set

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
        graphics.text("BRT: ", 0, 1, scale=1)
        graphics.text(f"{bp:.0f}", 7, 23, scale=1)
        draw_percentage((WIDTH - 10), 23)

        # draw sun icon
        draw_sun(0, 10, 2)

        # draw a bar for the background
        bar_width = WIDTH - 12
        graphics.set_pen(GREY)
        graphics.rectangle(13, 10, bar_width, 11)

        # draw a bar for the current brightness percentage
        graphics.set_pen(CURRENT_COLOUR)
        graphics.rectangle(13, 10, int((bp / 100) * bar_width), 11)

        last = current

    # time to update the display
    cu.update(graphics)
