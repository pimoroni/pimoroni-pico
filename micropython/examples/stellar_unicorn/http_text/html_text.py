import time
from stellar import StellarUnicorn
from picographics import PicoGraphics, DISPLAY_STELLAR_UNICORN as DISPLAY
import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio as asyncio
import uasyncio.core
from tinyweb.server import webserver

'''
Display scrolling wisdom, quotes or greetz... from the internetz!

You can adjust the brightness with LUX + and -.

Requires network_manager.py , WIFI_CONFIG.py, logging.mpy and tinyweb from micropython/examples/common
You'll also need index.html to be saved alongside this file.
'''

# Server Settings
host = "0.0.0.0"
port = 80


def convert_colour(colour_str):
    colour_str = colour_str.split(',')
    print(colour_str)
    return colour_str[0], colour_str[1], colour_str[2]


class text:

    def get(self, data):
        global MESSAGE, MESSAGE_COLOUR, BACKGROUND_COLOUR
        print(data)
        if 'text' in data.keys():
            MESSAGE = data['text']
        if 'colourfg' in data.keys():
            MESSAGE_COLOUR = convert_colour(data['colourfg'])
        if 'colourbg' in data.keys():
            BACKGROUND_COLOUR = convert_colour(data['colourbg'])
        return {'message': 'text updated'}, 201

    def post(self, data):

        return {'message': 'text updated'}, 201


def status_handler(mode, status, ip):
    global MESSAGE
    print("Network: {}".format(WIFI_CONFIG.SSID))
    status_text = "Connecting..."
    if status is not None:
        if status:
            status_text = "Connection successful!"
        else:
            status_text = "Connection failed!"

    print(status_text)
    print("IP: {}".format(ip))
    MESSAGE = "{}".format(ip)


# Create web server application
app = webserver()


# Index page
@app.route('/')
async def index(request, response):
    # Send actual HTML page
    await response.send_file('index.html', content_type='text/html')


# HTTP redirection
@app.route('/redirect')
async def redirect(request, response):
    # Start HTTP response with content-type text/html
    await response.redirect('/')

# constants for controlling scrolling text
PADDING = 5
MESSAGE_COLOUR = (255, 255, 255)
OUTLINE_COLOUR = (0, 0, 0)
BACKGROUND_COLOUR = (10, 0, 96)
MESSAGE = "Connecting"
HOLD_TIME = 2.0
STEP_TIME = 0.075

# create galactic object and graphics surface for drawing
su = StellarUnicorn()
graphics = PicoGraphics(DISPLAY)

width = StellarUnicorn.WIDTH
height = StellarUnicorn.HEIGHT


# function for drawing outlined text
def outline_text(text, x, y):
    graphics.set_pen(graphics.create_pen(int(OUTLINE_COLOUR[0]), int(OUTLINE_COLOUR[1]), int(OUTLINE_COLOUR[2])))
    graphics.text(text, x - 1, y - 1, -1, 1)
    graphics.text(text, x, y - 1, -1, 1)
    graphics.text(text, x + 1, y - 1, -1, 1)
    graphics.text(text, x - 1, y, -1, 1)
    graphics.text(text, x + 1, y, -1, 1)
    graphics.text(text, x - 1, y + 1, -1, 1)
    graphics.text(text, x, y + 1, -1, 1)
    graphics.text(text, x + 1, y + 1, -1, 1)

    graphics.set_pen(graphics.create_pen(int(MESSAGE_COLOUR[0]), int(MESSAGE_COLOUR[1]), int(MESSAGE_COLOUR[2])))
    graphics.text(text, x, y, -1, 1)


def run():
    # Setup wifi
    network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)

    app.add_resource(text, '/update')

    # Connect to Wifi network
    asyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
    while (not network_manager.isconnected()):
        time.sleep(0.1)


su.set_brightness(0.5)

# Start wifi connection
run()


async def message_update():
    global MESSAGE
    last_time = time.ticks_ms()
    STATE_PRE_SCROLL = 0
    STATE_SCROLLING = 1
    STATE_POST_SCROLL = 2

    shift = 0
    state = STATE_PRE_SCROLL

    # set the font
    graphics.set_font("bitmap8")

    # calculate the message width so scrolling can happen
    msg_width = graphics.measure_text(MESSAGE, 1)
    while 1:

        msg_width = graphics.measure_text(MESSAGE, 1)
        time_ms = time.ticks_ms()

        if su.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_UP):
            su.adjust_brightness(+0.01)

        if su.is_pressed(StellarUnicorn.SWITCH_BRIGHTNESS_DOWN):
            su.adjust_brightness(-0.01)

        if state == STATE_PRE_SCROLL and time_ms - last_time > HOLD_TIME * 1000:
            if msg_width + PADDING * 2 >= width:
                state = STATE_SCROLLING
            last_time = time_ms

        if state == STATE_SCROLLING and time_ms - last_time > STEP_TIME * 1000:
            shift += 1
            if shift >= (msg_width + PADDING * 2) - width - 1:
                state = STATE_POST_SCROLL
            last_time = time_ms

        if state == STATE_POST_SCROLL and time_ms - last_time > HOLD_TIME * 1000:
            state = STATE_PRE_SCROLL
            shift = 0
            last_time = time_ms

        graphics.set_pen(graphics.create_pen(int(BACKGROUND_COLOUR[0]), int(BACKGROUND_COLOUR[1]), int(BACKGROUND_COLOUR[2])))
        graphics.clear()

        outline_text(MESSAGE, x=PADDING - shift, y=4)

        # update the display
        su.update(graphics)

        # pause for a moment (important or the USB serial device will fail)
        await asyncio.sleep(0.001)


# The following is required to run both the web server and the scrolling text coherently
app._server_coro = app._tcp_server(host, port, app.backlog)
loop = asyncio.get_event_loop()
t1 = loop.create_task(message_update())
t2 = loop.create_task(app._server_coro)
loop.run_forever()
