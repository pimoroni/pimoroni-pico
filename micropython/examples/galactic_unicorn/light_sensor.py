import time
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN as DISPLAY

"""
Light sensoring feature for the galactic unicorn
Uses the onboard light sensor to detect the light in the environment
The brightness level is displayed as percentage.
Brightness of the display is auto adjusted to the brightness level of the environment
Press A to turn auto brightness on
Press B to turn auto brightness off
"""
# set up unicorn and drawing variables
gu = GalacticUnicorn()
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
    current_lsv = gu.light()
    current_brightness_val = map_range(current_lsv)

    # uses the previous value to smooth out display changes reducing flickering
    brightness_diff = current_brightness_val - prev_brightness_val
    brightness_val = prev_brightness_val + (brightness_diff * TRANSITION_RATE)
    if brightness_val > 1:
        brightness_val = 1
    elif brightness_val < 0.1:
        brightness_val = 0.1

    return brightness_val


# draws percentage icon
def draw_percentage(x, y):
    graphics.rectangle(x + 1, y + 1, 2, 2)
    graphics.line(x + 1, y + 5, x + 6, y)
    graphics.rectangle(x + 4, y + 4, 2, 2)


# sets up a handy function we can call to clear the screen
def clear():
    graphics.set_pen(BLACK)
    graphics.clear()


mode = "auto"  # set auto brightness on
last = time.ticks_ms()
brightness_value = MIN_RANGE  # set the initial brightness level to the specified minimum
while True:
    current = time.ticks_ms()

    # set the display brightness
    brightness_value = calculate_brightness(brightness_value)
    gu.set_brightness(brightness_value)

    bp = (brightness_value / MAX_RANGE) * 100  # gets brightness value in percentage relative to the MAX_LS_VALUE set

    # deactivate auto brightness by pressing A
    if gu.is_pressed(GalacticUnicorn.SWITCH_A):
        print("Auto brightness off")
        mode = "off"

    # reactivate auto brightness by pressing A
    if gu.is_pressed(GalacticUnicorn.SWITCH_B):
        print("Auto brightness on")
        mode = "auto"

    # set brightness to default value if off
    if mode == "off":
        gu.set_brightness(0.5)

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

        # measure the rest of the text before drawing to right align it
        text_width = graphics.measure_text(f"{bp:.0f}  ", scale=1)
        graphics.text(f"{bp:.0f}", WIDTH - text_width, 1, scale=1)
        draw_percentage((WIDTH - 8), 1)

        # draw a bar for the background
        graphics.set_pen(GREY)
        graphics.rectangle(0, 9, WIDTH, 10)

        # draw a bar for the current brightness percentage
        graphics.set_pen(CURRENT_COLOUR)
        graphics.rectangle(0, 9, int((bp / 100) * WIDTH), 10)
        last = current

    # time to update the display
    gu.update(graphics)
