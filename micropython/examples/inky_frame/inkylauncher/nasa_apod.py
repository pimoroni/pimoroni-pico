import gc
import jpegdec
from urllib import urequest
from ujson import load

gc.collect()

graphics = None

WIDTH, HEIGHT = 0, 0

FILENAME = "nasa-apod-640x400-daily"
IMG_URL = "https://pimoroni.github.io/feed2image/nasa-apod-640x400-daily.jpg"
# A Demo Key is used in this example and is IP rate limited. You can get your own API Key from https://api.nasa.gov/
API_URL = "https://api.nasa.gov/planetary/apod?api_key=CgQGiTiyzQWEfkPgZ4btNM1FTLZQP5DeSfEwbVr7"

# Length of time between updates in minutes.
# Frequent updates will reduce battery life!
UPDATE_INTERVAL = 1

# Variable for storing the NASA APOD Title
apod_title = None


def show_error(text):
    graphics.set_pen(4)
    graphics.rectangle(0, 10, 640, 35)
    graphics.set_pen(1)
    graphics.text(text, 5, 16, 400, 2)


def update():
    global apod_title

    try:
        # Grab the data
        socket = urequest.urlopen(API_URL)
        gc.collect()
        j = load(socket)
        socket.close()
        apod_title = j['title']
        gc.collect()
    except OSError as e:
        print(e)
        apod_title = "Image Title Unavailable"

    try:
        # Grab the image
        socket = urequest.urlopen(IMG_URL)

        gc.collect()

        data = bytearray(1024)
        with open(FILENAME, "wb") as f:
            while True:
                if socket.readinto(data) == 0:
                    break
                f.write(data)
        socket.close()
        del data
        gc.collect()
    except OSError as e:
        print(e)
        show_error("Unable to download image")


def draw():
    jpeg = jpegdec.JPEG(graphics)
    gc.collect()  # For good measure...

    graphics.set_pen(1)
    graphics.clear()

    try:
        jpeg.open_file(FILENAME)
        jpeg.decode()
    except OSError:
        graphics.set_pen(4)
        graphics.rectangle(0, 170, 640, 25)
        graphics.set_pen(1)
        graphics.text("Unable to display image! :(", 5, 175, 400, 2)

    graphics.set_pen(0)
    graphics.rectangle(0, 375, 640, 25)
    graphics.set_pen(1)
    graphics.text(apod_title, 5, 380, 400, 2)

    gc.collect()

    graphics.update()
