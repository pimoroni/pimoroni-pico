import time
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY

'''
Display scrolling wisdom, quotes or greetz.

You can adjust the brightness with LUX + and -.
'''

# constants for controlling scrolling text
PADDING = 5
MESSAGE_COLOUR = (255, 255, 255)
OUTLINE_COLOUR = (0, 0, 0)
BACKGROUND_COLOUR = (10, 0, 96)
MESSAGE = "\"Space is big. Really big. You just won't believe how vastly hugely mind-bogglingly big it is. I mean, you may think it's a long way down the road to the chemist, but that's just peanuts to space.\" - Douglas Adams"
HOLD_TIME = 2.0
STEP_TIME = 0.075

# create cosmic object and graphics surface for drawing
cu = CosmicUnicorn()
graphics = PicoGraphics(DISPLAY)

width = CosmicUnicorn.WIDTH
height = CosmicUnicorn.HEIGHT


# function for drawing outlined text
def outline_text(text, x, y):
    graphics.set_pen(graphics.create_pen(int(OUTLINE_COLOUR[0]), int(OUTLINE_COLOUR[1]), int(OUTLINE_COLOUR[2])))
    graphics.text(text, x - 1, y - 1, -1, 1)
    graphics.text(text, x, y - 1, -1, 1)
    graphics.text(text, x + 1, y - 1, -1, 1)
    graphics.text(text, x - 1, y, -1, 1)
    graphics.text(text, x + 1, y, -1, 1)
    graphics.text(text, x - 1, y + 1, -1, 1)
    graphics.text(text, x, y + 1, -1, 1)
    graphics.text(text, x + 1, y + 1, -1, 1)

    graphics.set_pen(graphics.create_pen(int(MESSAGE_COLOUR[0]), int(MESSAGE_COLOUR[1]), int(MESSAGE_COLOUR[2])))
    graphics.text(text, x, y, -1, 1)


cu.set_brightness(0.5)

# state constants
STATE_PRE_SCROLL = 0
STATE_SCROLLING = 1
STATE_POST_SCROLL = 2

shift = 0
state = STATE_PRE_SCROLL

# set the font
graphics.set_font("bitmap8")

# calculate the message width so scrolling can happen
msg_width = graphics.measure_text(MESSAGE, 1)

last_time = time.ticks_ms()

while True:
    time_ms = time.ticks_ms()

    if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_UP):
        cu.adjust_brightness(+0.01)

    if cu.is_pressed(CosmicUnicorn.SWITCH_BRIGHTNESS_DOWN):
        cu.adjust_brightness(-0.01)

    if state == STATE_PRE_SCROLL and time_ms - last_time > HOLD_TIME * 1000:
        if msg_width + PADDING * 2 >= width:
            state = STATE_SCROLLING
        last_time = time_ms

    if state == STATE_SCROLLING and time_ms - last_time > STEP_TIME * 1000:
        shift += 1
        if shift >= (msg_width + PADDING * 2) - width - 1:
            state = STATE_POST_SCROLL
        last_time = time_ms

    if state == STATE_POST_SCROLL and time_ms - last_time > HOLD_TIME * 1000:
        state = STATE_PRE_SCROLL
        shift = 0
        last_time = time_ms

    graphics.set_pen(graphics.create_pen(int(BACKGROUND_COLOUR[0]), int(BACKGROUND_COLOUR[1]), int(BACKGROUND_COLOUR[2])))
    graphics.clear()

    outline_text(MESSAGE, x=PADDING - shift, y=2)

    # update the display
    cu.update(graphics)

    # pause for a moment (important or the USB serial device will fail)
    time.sleep(0.001)
