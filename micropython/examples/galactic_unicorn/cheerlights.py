# cheerlights
from time import sleep
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN as DISPLAY
import uasyncio
from network_manager import NetworkManager
import urequests
import WIFI_CONFIG


# setup graphics
gu = GalacticUnicorn()
graphics = PicoGraphics(DISPLAY)
width = GalacticUnicorn.WIDTH
height = GalacticUnicorn.HEIGHT


# fill display
def draw(colour):

    graphics.set_pen(colour)
    graphics.clear()
    gu.update(graphics)


def status_handler(mode, status, ip):
    # reports wifi connection status
    print(mode, status, ip)
    print('Connecting to wifi...')
    if status is not None:
        if status:
            print('Wifi connection successful!')
        else:
            print('Wifi connection failed!')


# set up wifi
try:
    network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
except Exception as e:
    print(f'Wifi connection failed! {e}')


# cheerlights

# cheerlights colours
colours = {b'red': graphics.create_pen(0xff, 0x00, 0x00), b'green': graphics.create_pen(0x00, 0x80, 0x00),
           b'blue': graphics.create_pen(0x00, 0x00, 0xff),
           b'cyan': graphics.create_pen(0x00, 0xff, 0xff), b'white': graphics.create_pen(0xff, 0xff, 0xff),
           b'oldlace': graphics.create_pen(0xfd, 0xf5, 0xe6),
           b'purple': graphics.create_pen(0x80, 0x00, 0x80), b'magenta': graphics.create_pen(0xff, 0x00, 0xff),
           b'yellow': graphics.create_pen(0xff, 0xff, 0x00),
           b'orange': graphics.create_pen(0xff, 0xa5, 0x00), b'pink': graphics.create_pen(0xff, 0xc0, 0xcb)}

# get request
while True:
    r = urequests.get('http://api.thingspeak.com/channels/1417/field/1/last.txt')
    print(f'Colour: {r.content}')
    if r.content in colours:
        draw(colours[r.content])
    r.close()
    sleep(15)
