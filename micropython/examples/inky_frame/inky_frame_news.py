# from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY      # 5.7"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"
from network_manager import NetworkManager
import uasyncio
from urllib import urequest
import WIFI_CONFIG
import gc
import qrcode
from machine import Pin
from pimoroni_i2c import PimoroniI2C
from pcf85063a import PCF85063A
import time

I2C_SDA_PIN = 4
I2C_SCL_PIN = 5
HOLD_VSYS_EN_PIN = 2

# set up and enable vsys hold so we don't go to sleep
hold_vsys_en_pin = Pin(HOLD_VSYS_EN_PIN, Pin.OUT)
hold_vsys_en_pin.value(True)

# Uncomment one URL to use (Top Stories, World News and technology)
# URL = "http://feeds.bbci.co.uk/news/rss.xml"
# URL = "http://feeds.bbci.co.uk/news/world/rss.xml"
URL = "http://feeds.bbci.co.uk/news/technology/rss.xml"

# Length of time between updates in Seconds.
# Frequent updates will reduce battery life!
UPDATE_INTERVAL = 60 * 1

graphics = PicoGraphics(DISPLAY)
WIDTH, HEIGHT = graphics.get_bounds()
graphics.set_font("bitmap8")
code = qrcode.QRCode()

# intialise the pcf85063a real time clock chip
i2c = PimoroniI2C(I2C_SDA_PIN, I2C_SCL_PIN, 100000)
rtc = PCF85063A(i2c)


def status_handler(mode, status, ip):
    print(mode, status, ip)


network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)


def read_until(stream, char):
    result = b""
    while True:
        c = stream.read(1)
        if c == char:
            return result
        result += c


def discard_until(stream, c):
    while stream.read(1) != c:
        pass


def parse_xml_stream(s, accept_tags, group_by, max_items=3):
    tag = []
    text = b""
    count = 0
    current = {}
    while True:
        char = s.read(1)
        if len(char) == 0:
            break

        if char == b"<":
            next_char = s.read(1)

            # Discard stuff like <?xml vers...
            if next_char == b"?":
                discard_until(s, b">")
                continue

            # Detect <![CDATA
            elif next_char == b"!":
                s.read(1)  # Discard [
                discard_until(s, b"[")  # Discard CDATA[
                text = read_until(s, b"]")
                discard_until(s, b">")  # Discard ]>
                gc.collect()

            elif next_char == b"/":
                current_tag = read_until(s, b">")
                top_tag = tag[-1]

                # Populate our result dict
                if top_tag in accept_tags:
                    current[top_tag.decode("utf-8")] = text.decode("utf-8")

                # If we've found a group of items, yield the dict
                elif top_tag == group_by:
                    yield current
                    current = {}
                    count += 1
                    if count == max_items:
                        return
                tag.pop()
                text = b""
                gc.collect()
                continue

            else:
                current_tag = read_until(s, b">")
                tag += [next_char + current_tag.split(b" ")[0]]
                text = b""
                gc.collect()

        else:
            text += char


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


def get_rss():
    try:
        stream = urequest.urlopen(URL)
        output = list(parse_xml_stream(stream, [b"title", b"description", b"guid", b"pubDate"], b"item"))
        return output

    except OSError as e:
        print(e)
        return False


rtc.enable_timer_interrupt(True)

while True:
    # Connect to WiFi
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))

    # Gets Feed Data
    feed = get_rss()

    # Clear the screen
    graphics.set_pen(1)
    graphics.clear()
    graphics.set_pen(0)

    # Title
    graphics.text("Headlines from BBC News:", 10, 10, 300, 2)

    # Draws 3 articles from the feed if they're available.
    if feed:
        graphics.set_pen(4)
        graphics.text(feed[0]["title"], 10, 40, WIDTH - 150, 3 if graphics.measure_text(feed[0]["title"]) < 650 else 2)
        graphics.text(feed[1]["title"], 130, 180, WIDTH - 140, 3 if graphics.measure_text(feed[1]["title"]) < 650 else 2)
        graphics.text(feed[2]["title"], 10, 320, WIDTH - 150, 3 if graphics.measure_text(feed[2]["title"]) < 650 else 2)

        graphics.set_pen(3)
        graphics.text(feed[0]["description"], 10, 110 if graphics.measure_text(feed[0]["title"]) < 650 else 90, WIDTH - 150, 2)
        graphics.text(feed[1]["description"], 130, 250 if graphics.measure_text(feed[1]["title"]) < 650 else 230, WIDTH - 145, 2)
        graphics.text(feed[2]["description"], 10, 395 if graphics.measure_text(feed[2]["title"]) < 650 else 375, WIDTH - 150, 2)

        code.set_text(feed[0]["guid"])
        draw_qr_code(WIDTH - 110, 40, 100, code)
        code.set_text(feed[1]["guid"])
        draw_qr_code(10, 180, 100, code)
        code.set_text(feed[2]["guid"])
        draw_qr_code(WIDTH - 110, 320, 100, code)

    else:
        graphics.set_pen(4)
        graphics.text("Error: Unable to get feed :(", 10, 40, WIDTH - 150, 4)

    graphics.update()

    # Time to have a little nap until the next update
    rtc.set_timer(UPDATE_INTERVAL)
    hold_vsys_en_pin.init(Pin.IN)
    time.sleep(UPDATE_INTERVAL)
