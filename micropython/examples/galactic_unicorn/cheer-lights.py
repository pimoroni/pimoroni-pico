# cheerlights
from time import sleep
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN as DISPLAY
import network
import urequests

try:
    from secrets import WIFI_SSID, WIFI_PASS
except ImportError:
    WIFI_SSID = None
    WIFI_PASS = None

# setup graphics
gu = GalacticUnicorn()
graphics = PicoGraphics(DISPLAY)
width = GalacticUnicorn.WIDTH
height = GalacticUnicorn.HEIGHT


# fill display
def draw(colour):
    for x in range(width):
        for y in range(height):
            graphics.set_pen(colour)
            graphics.pixel(x, y)

    gu.update(graphics)


# setup network connection
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect(WIFI_SSID, WIFI_PASS)

# Wait for connect or fail
max_wait = 10
while max_wait > 0:
    if wlan.status() < 0 or wlan.status() >= 3:
        break
    max_wait -= 1
    print('waiting for connection...')
    sleep(1)

# Handle connection error
if wlan.status() != 3:
    raise RuntimeError('network connection failed')
else:
    print('connected')
    status = wlan.ifconfig()
    print(f'ip = {status[0]}')

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
