import gc
import ujson
from urllib import urequest

# Length of time between updates in Seconds.
# Frequent updates will reduce battery life!
UPDATE_INTERVAL = 240

# API URL
URL = "https://www.boredapi.com/api/activity"

graphics = None
text = None

gc.collect()


def display_quote(text, ox, oy, scale, wordwrap):
    # Processing text is memory intensive
    # so we'll do it one char at a time as we draw to the screen
    line_height = 8 * scale
    html = False
    html_tag = ""
    word = ""
    space_width = graphics.measure_text(" ", scale=scale)
    x = ox
    y = oy
    for char in text:
        if char in "[]":
            continue
        if char == "<":
            html = True
            html_tag = ""
            continue
        if char == ">":
            html = False
            continue
        if html:
            if char in "/ ":
                continue
            html_tag += char
            continue
        if char in (" ", "\n") or html_tag == "br":
            w = graphics.measure_text(word, scale=scale)
            if x + w > wordwrap or char == "\n" or html_tag == "br":
                x = ox
                y += line_height

            graphics.text(word, x, y, scale=scale)
            word = ""
            html_tag = ""
            x += w + space_width
            continue

        word += char

    # Last word
    w = graphics.measure_text(word, scale=scale)
    if x + w > wordwrap:
        x = ox
        y += line_height

    graphics.text(word, x, y, scale=scale)


def update():
    global text
    gc.collect()

    try:
        # Grab the data
        socket = urequest.urlopen(URL)
        j = ujson.load(socket)
        socket.close()
        text = [j['activity'], j['type'], j['participants']]
        gc.collect()
    except OSError:
        pass


def draw():
    global text

    WIDTH, HEIGHT = graphics.get_bounds()

    # Clear the screen
    graphics.set_pen(1)
    graphics.clear()
    graphics.set_pen(0)

    # Page lines!
    graphics.set_pen(3)
    graphics.line(0, 65, WIDTH, 65)
    for i in range(2, 13):
        graphics.line(0, i * 35, WIDTH, i * 35)

    gc.collect()

    # Page margin
    graphics.set_pen(4)
    graphics.line(50, 0, 50, HEIGHT)
    graphics.set_pen(0)

    # Main text
    graphics.set_font("cursive")
    graphics.set_pen(4)
    graphics.set_font("cursive")
    graphics.text("Activity Idea", 55, 30, WIDTH - 20, 2)
    graphics.set_pen(0)
    graphics.set_font("bitmap8")

    if text:
        display_quote(text[0], 55, 170, 5, WIDTH - 20)
        gc.collect()
        graphics.set_pen(2)
        graphics.text("Activity Type: " + text[1], 55, HEIGHT - 45, WIDTH - 20, 2)
        graphics.text("Participants: " + str(text[2]), 400, HEIGHT - 45, WIDTH - 20, 2)
    else:
        graphics.set_pen(4)
        graphics.rectangle(0, (HEIGHT // 2) - 20, WIDTH, 40)
        graphics.set_pen(1)
        graphics.text("Unable to get activity data!", 5, (HEIGHT // 2) - 15, WIDTH, 2)
        graphics.text("Check your network settings in secrets.py", 5, (HEIGHT // 2) + 2, WIDTH, 2)

    graphics.update()

    gc.collect()
