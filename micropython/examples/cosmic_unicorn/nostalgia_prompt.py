import time
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN as DISPLAY

'''
A collection of copies of classic terminal styles including
C64, MS-DOS, Spectrum, and more. Images and text are drawn
pixel by pixel from a pattern of Os and Xs.

You can adjust the brightness with LUX + and -.
'''

gu = GalacticUnicorn()
graphics = PicoGraphics(DISPLAY)


c64 = [
    "                                                     ",
    "                                                     ",
    " OOOOO   OOOOOO    OO    OOOO    OO  OO      XXXXXXX ",
    " OO  OO  OO       OOOO   OO OO   OO  OO      XXXXXXX ",
    " OO  OO  OO      OO  OO  OO  OO  OO  OO      XXXXXXX ",
    " OOOOO   OOOO    OOOOOO  OO  OO   OOOO       XXXXXXX ",
    " OOOO    OO      OO  OO  OO  OO    OO        XXXXXXX ",
    " OO OO   OO      OO  OO  OO OO     OO    OO  XXXXXXX ",
    " OO  OO  OOOOOO  OO  OO  OOOO      OO    OO  XXXXXXX ",
    "                                             XXXXXXX ",
    "                                                     "
]
FOREGROUND_C64 = (230, 210, 250)
BACKGROUND_C64 = (20, 20, 120)

spectrum = [
    "                                                     ",
    "                                                     ",
    " O        OOOO    OOOO   OOOOO     O O  O O XXXXXXXX ",
    " O       O    O  O    O  O    O    O O  O O X XXXXXX ",
    " O       O    O  O    O  O    O             X XXXXXX ",
    " O       O    O  OOOOOO  O    O             X XXXXXX ",
    " O       O    O  O    O  O    O             X XXXXXX ",
    " OOOOOO   OOOO   O    O  OOOOO              X XXXXXX ",
    "                                            X      X ",
    "                                            XXXXXXXX ",
    "                                                     "
]
FOREGROUND_SPECTRUM = (0, 0, 0)
BACKGROUND_SPECTRUM = (180, 150, 150)

bbc_micro = [
    "                                                     ",
    "                                                     ",
    " OOOOO    OO    OOOO   OOO    OOOO      O            ",
    " O    O  O  O  O    O   O    O    O      O           ",
    " O    O O    O O        O    O            O          ",
    " OOOOO  O    O  OOOO    O    O             O         ",
    " O    O OOOOOO      O   O    O            O          ",
    " O    O O    O O    O   O    O    O      O           ",
    " OOOOO  O    O  OOOO   OOO    OOOO      O            ",
    "                                             XXXXXXX ",
    "                                                     "
]
FOREGROUND_BBC_MICRO = (255, 255, 255)
BACKGROUND_BBC_MICRO = (0, 0, 0)

PROMPT_C64 = 0
PROMPT_SPECTRUM = 1
PROMPT_BBC_MICRO = 2
prompt = 0


@micropython.native  # noqa: F821
def draw(image, fg, bg, time_ms):
    fg_pen = graphics.create_pen(fg[0], fg[1], fg[2])
    bg_pen = graphics.create_pen(bg[0], bg[1], bg[2])
    for y in range(len(image)):
        row = image[y]
        for x in range(len(row)):
            pixel = row[x]
            # draw the prompt text
            if pixel == 'O':
                graphics.set_pen(fg_pen)

            # draw the caret blinking
            elif pixel == 'X' and (time_ms // 300) % 2:
                graphics.set_pen(fg_pen)

            else:
                graphics.set_pen(bg_pen)

            graphics.pixel(x, y)

    gu.update(graphics)


gu.set_brightness(0.5)

while True:

    time_ms = time.ticks_ms()
    prompt = (time_ms // 3000) % 3

    if gu.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_UP):
        gu.adjust_brightness(+0.01)

    if gu.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_DOWN):
        gu.adjust_brightness(-0.01)

    start = time.ticks_ms()

    if prompt == PROMPT_C64:
        draw(c64, FOREGROUND_C64, BACKGROUND_C64, time_ms)

    elif prompt == PROMPT_SPECTRUM:
        draw(spectrum, FOREGROUND_SPECTRUM, BACKGROUND_SPECTRUM, time_ms)

    elif prompt == PROMPT_BBC_MICRO:
        draw(bbc_micro, FOREGROUND_BBC_MICRO, BACKGROUND_BBC_MICRO, time_ms)

    # pause for a moment (important or the USB serial device will fail)
    time.sleep(0.001)

    print("total took: {} ms".format(time.ticks_ms() - start))
