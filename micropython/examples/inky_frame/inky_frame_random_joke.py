import gc
import uos
import random
import machine
import jpegdec
import WIFI_CONFIG
import uasyncio
from network_manager import NetworkManager
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY      # 5.7"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"
from urllib import urequest


gc.collect()  # We're really gonna need that RAM!


def status_handler(mode, status, ip):
    print(mode, status, ip)


network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)
uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))


graphics = PicoGraphics(DISPLAY)

WIDTH, HEIGHT = graphics.get_bounds()
FILENAME = "/sd/random-joke.jpg"

JOKE_IDS = "https://pimoroni.github.io/feed2image/jokeapi-ids.txt"
JOKE_IMG = "https://pimoroni.github.io/feed2image/jokeapi-{}-{}x{}.jpg"

import sdcard  # noqa: E402 - putting this at the top causes an MBEDTLS OOM error!?
sd_spi = machine.SPI(0, sck=machine.Pin(18, machine.Pin.OUT), mosi=machine.Pin(19, machine.Pin.OUT), miso=machine.Pin(16, machine.Pin.OUT))
sd = sdcard.SDCard(sd_spi, machine.Pin(22))
uos.mount(sd, "/sd")
gc.collect()  # Claw back some RAM!

# We don't have the RAM to store the list of Joke IDs in memory.
# the first line of `jokeapi-ids.txt` is a COUNT of IDs.
# Grab it, then pick a random line between 0 and COUNT.
# Seek to that line and ...y'know... there's our totally random joke ID

socket = urequest.urlopen(JOKE_IDS)

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


jpeg = jpegdec.JPEG(graphics)
gc.collect()  # For good measure...

graphics.set_pen(1)
graphics.clear()

jpeg.open_file(FILENAME)
jpeg.decode()

graphics.update()
