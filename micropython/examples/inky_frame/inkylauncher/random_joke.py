import gc
import random
from urllib import urequest
from ujson import load
import qrcode

gc.collect()  # We're really gonna need that RAM!

graphics = None

WIDTH = 0
HEIGHT = 0

FILENAME = "random-joke.jpg"

JOKE_IDS = "https://pimoroni.github.io/feed2image/jokeapi-ids.txt"
JOKE_IMG = "https://pimoroni.github.io/feed2image/jokeapi-{}.json"

UPDATE_INTERVAL = 60

gc.collect()  # Claw back some RAM!

# We don't have the RAM to store the list of Joke IDs in memory.
# the first line of `jokeapi-ids.txt` is a COUNT of IDs.
# Grab it, then pick a random line between 0 and COUNT.
# Seek to that line and ...y'know... there's our totally random joke ID

joke = []


def measure_qr_code(size, code):
    w, h = code.get_size()
    module_size = int(size / w)
    return module_size * w, module_size


def draw_qr_code(ox, oy, size, code):
    size, module_size = measure_qr_code(size, code)
    graphics.set_pen(1)
    graphics.rectangle(ox, oy, size, size)
    graphics.set_pen(0)
    for x in range(size):
        for y in range(size):
            if code.get_module(x, y):
                graphics.rectangle(ox + x * module_size, oy + y * module_size, module_size, module_size)


def update():
    global joke

    try:
        socket = urequest.urlopen(JOKE_IDS)
    except OSError:
        return

    # Get the first line, which is a count of the joke IDs
    number_of_lines = int(socket.readline().decode("ascii"))
    print("Total jokes {}".format(number_of_lines))

    # Pick a random joke (by its line number)
    line = random.randint(0, number_of_lines)
    print("Getting ID from line {}".format(line))

    for x in range(line):  # Throw away lines to get where we need
        socket.readline()

    # Read our chosen joke ID!
    random_joke_id = int(socket.readline().decode("ascii"))
    socket.close()

    print("Random joke ID: {}".format(random_joke_id))

    url = JOKE_IMG.format(random_joke_id)

    gc.collect()

    # Grab the data
    try:
        socket = urequest.urlopen(url)
        gc.collect()
        j = load(socket)
        socket.close()
        joke = j
        del j
        gc.collect()
    except OSError as e:
        print(e)


def draw():

    code = qrcode.QRCode()
    graphics.set_pen(1)
    graphics.clear()

    if joke:
        if joke['type'] == "single":
            graphics.set_pen(4)
            graphics.text(joke['joke'], 10, 10, WIDTH - 75, 5)
        if joke['type'] == "twopart":
            graphics.set_pen(4)
            graphics.text(joke['setup'], 10, 10, WIDTH - 75, 5)
            graphics.set_pen(3)
            graphics.text(joke['delivery'], 10, 250, WIDTH - 75, 5)

        graphics.set_pen(0)
        # Donate link QR
        code.set_text("https://github.com/sponsors/Sv443")
        draw_qr_code(WIDTH - 75, HEIGHT - 75, 75, code)

        graphics.text("curated by jokeapi.dev", 10, HEIGHT - 15, WIDTH, 1)
        graphics.text("donate <3", WIDTH - 65, HEIGHT - 12, WIDTH, 1)

    else:
        graphics.set_pen(4)
        graphics.rectangle(0, (HEIGHT // 2) - 20, WIDTH, 40)
        graphics.set_pen(1)
        graphics.text("Unable to display random joke!", 5, (HEIGHT // 2) - 15, WIDTH, 2)
        graphics.text("Check your network settings in secrets.py", 5, (HEIGHT // 2) + 2, WIDTH, 2)

    graphics.update()
