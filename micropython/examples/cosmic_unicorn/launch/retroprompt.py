import time

graphics = None
palette = None
prompt_x = 0
prompt_y = 4

c64 = [
    "                                ",
    "                                ",
    " OOOOO   OOOOOO    OO    OOOO   ",
    " OO  OO  OO       OOOO   OO OO  ",
    " OO  OO  OO      OO  OO  OO  OO ",
    " OOOOO   OOOO    OOOOOO  OO  OO ",
    " OOOO    OO      OO  OO  OO  OO ",
    " OO OO   OO      OO  OO  OO OO  ",
    " OO  OO  OOOOOO  OO  OO  OOOO   ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    " OO  OO      XXXXXXX            ",
    " OO  OO      XXXXXXX            ",
    " OO  OO      XXXXXXX            ",
    "  OOOO       XXXXXXX            ",
    "   OO        XXXXXXX            ",
    "   OO    OO  XXXXXXX            ",
    "   OO    OO  XXXXXXX            ",
    "             XXXXXXX            ",
    "                                "
]
FOREGROUND_C64 = (230, 210, 250)
BACKGROUND_C64 = (20, 20, 120)

spectrum = [
    "                                ",
    "                                ",
    " O        OOOO    OOOO   OOOOO  ",
    " O       O    O  O    O  O    O ",
    " O       O    O  O    O  O    O ",
    " O       O    O  OOOOOO  O    O ",
    " O       O    O  O    O  O    O ",
    " OOOOOO   OOOO   O    O  OOOOO  ",
    "                                ",
    "                                ",
    "                                ",
    "                     ",
    "                     ",
    "   O O  O O XXXXXXXX ",
    "   O O  O O X XXXXXX ",
    "            X XXXXXX ",
    "            X XXXXXX ",
    "            X XXXXXX ",
    "            X XXXXXX ",
    "            X      X ",
    "            XXXXXXXX ",
    "                     "
]
FOREGROUND_SPECTRUM = (0, 0, 0)
BACKGROUND_SPECTRUM = (180, 150, 150)

bbc_micro = [
    "                           ",
    "                           ",
    " OOOOO    OO    OOOO   OOO ",
    " O    O  O  O  O    O   O  ",
    " O    O O    O O        O  ",
    " OOOOO  O    O  OOOO    O  ",
    " O    O OOOOOO      O   O  ",
    " O    O O    O O    O   O  ",
    " OOOOO  O    O  OOOO   OOO ",
    "                           ",
    "                           ",
    "                          ",
    "                          ",
    "   OOOO      O            ",
    "  O    O      O           ",
    "  O            O          ",
    "  O             O         ",
    "  O            O          ",
    "  O    O      O           ",
    "   OOOO      O            ",
    "                  XXXXXXX ",
    "                          "
]
FOREGROUND_BBC_MICRO = (255, 255, 255)
BACKGROUND_BBC_MICRO = (0, 0, 0)

PROMPT_C64 = 0
PROMPT_SPECTRUM = 1
PROMPT_BBC_MICRO = 2
prompt = 0


def init():
    pass


@micropython.native  # noqa: F821
def draw():
    time_ms = time.ticks_ms()
    prompt = (time_ms // 3000) % 3

    if prompt == PROMPT_C64:
        image = c64
        fg = FOREGROUND_C64
        bg = BACKGROUND_C64
    elif prompt == PROMPT_SPECTRUM:
        image = spectrum
        fg = FOREGROUND_SPECTRUM
        bg = BACKGROUND_SPECTRUM
    elif prompt == PROMPT_BBC_MICRO:
        image = bbc_micro
        fg = FOREGROUND_BBC_MICRO
        bg = BACKGROUND_BBC_MICRO

    fg_pen = graphics.create_pen(fg[0], fg[1], fg[2])
    bg_pen = graphics.create_pen(bg[0], bg[1], bg[2])
    graphics.set_pen(bg_pen)
    graphics.clear()
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

            graphics.pixel(x + prompt_x, y + prompt_y)
