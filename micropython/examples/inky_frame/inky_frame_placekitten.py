import gc
import uos
import random
import machine
import jpegdec
import uasyncio
import sdcard
import WIFI_CONFIG
from urllib import urequest
from network_manager import NetworkManager
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY      # 5.7"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"

"""
random placekitten (from a very small set)

You *must* insert an SD card into Inky Frame!
We need somewhere to save the jpg for display.
"""

gc.collect()  # We're really gonna need that RAM!


def status_handler(mode, status, ip):
    print(mode, status, ip)


network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)
uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))


graphics = PicoGraphics(DISPLAY)

WIDTH, HEIGHT = graphics.get_bounds()
FILENAME = "/sd/placekitten.jpg"
ENDPOINT = "http://placekitten.com/{0}/{1}"


sd_spi = machine.SPI(0, sck=machine.Pin(18, machine.Pin.OUT), mosi=machine.Pin(19, machine.Pin.OUT), miso=machine.Pin(16, machine.Pin.OUT))
sd = sdcard.SDCard(sd_spi, machine.Pin(22))
uos.mount(sd, "/sd")
gc.collect()  # Claw back some RAM!

url = ENDPOINT.format(WIDTH, HEIGHT + random.randint(0, 10))

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
gc.collect()  # We really are tight on RAM!


jpeg = jpegdec.JPEG(graphics)
gc.collect()  # For good measure...

graphics.set_pen(1)
graphics.clear()

jpeg.open_file(FILENAME)
jpeg.decode()

graphics.update()
