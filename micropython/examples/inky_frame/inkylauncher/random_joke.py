import gc
import random
import jpegdec
from urllib import urequest

gc.collect()  # We're really gonna need that RAM!

graphics = None

WIDTH = 0
HEIGHT = 0

FILENAME = "random-joke.jpg"

JOKE_IDS = "https://pimoroni.github.io/feed2image/jokeapi-ids.txt"
JOKE_IMG = "https://pimoroni.github.io/feed2image/jokeapi-{}-{}x{}.jpg"

UPDATE_INTERVAL = 60

gc.collect()  # Claw back some RAM!


# We don't have the RAM to store the list of Joke IDs in memory.
# the first line of `jokeapi-ids.txt` is a COUNT of IDs.
# Grab it, then pick a random line between 0 and COUNT.
# Seek to that line and ...y'know... there's our totally random joke ID

def update():

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

    url = JOKE_IMG.format(random_joke_id, WIDTH, HEIGHT)

    socket = urequest.urlopen(url)

    # Stream the image data from the socket onto disk in 1024 byte chunks
    # the 600x448-ish jpeg will be roughly ~24k, we really don't have the RAM!
    data = bytearray(1024)
    with open(FILENAME, "wb") as f:
        while True:
            if socket.readinto(data) == 0:
                break
            f.write(data)
    socket.close()
    del data
    gc.collect()  # We really are tight on RAM!


def draw():
    jpeg = jpegdec.JPEG(graphics)
    gc.collect()  # For good measure...

    graphics.set_pen(1)
    graphics.clear()

    try:
        jpeg.open_file(FILENAME)
        jpeg.decode()
    except OSError:
        return

    graphics.update()
