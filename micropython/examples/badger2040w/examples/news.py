import badger2040w as badger2040
from badger2040w import WIDTH
import machine
from urllib import urequest
import gc
import qrcode
import badger_os

# URLS to use (Entertainment, Science and Technology)
URL = ["http://feeds.bbci.co.uk/news/entertainment_and_arts/rss.xml",
       "http://feeds.bbci.co.uk/news/science_and_environment/rss.xml",
       "http://feeds.bbci.co.uk/news/technology/rss.xml"]

code = qrcode.QRCode()

state = {
    "current_page": 0,
    "feed": 2
}

badger_os.state_load("news", state)

# Display Setup
display = badger2040.Badger2040W()
display.led(128)
display.set_update_speed(2)

# Setup buttons
button_a = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_b = machine.Pin(badger2040.BUTTON_B, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_c = machine.Pin(badger2040.BUTTON_C, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_down = machine.Pin(badger2040.BUTTON_DOWN, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_up = machine.Pin(badger2040.BUTTON_UP, machine.Pin.IN, machine.Pin.PULL_DOWN)


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
    display.set_pen(15)
    display.rectangle(ox, oy, size, size)
    display.set_pen(0)
    for x in range(size):
        for y in range(size):
            if code.get_module(x, y):
                display.rectangle(ox + x * module_size, oy + y * module_size, module_size, module_size)


# A function to get the data from an RSS Feed, this in case BBC News.
def get_rss(url):
    try:
        stream = urequest.urlopen(url)
        output = list(parse_xml_stream(stream, [b"title", b"description", b"guid", b"pubDate"], b"item"))
        return output

    except OSError as e:
        print(e)
        return False


# Connects to the wireless network. Ensure you have entered your details in WIFI_CONFIG.py :).
display.connect()

print(state["feed"])
feed = get_rss(URL[state["feed"]])


def draw_page():

    # Clear the display
    display.set_pen(15)
    display.clear()
    display.set_pen(0)

    # Draw the page header
    display.set_font("bitmap6")
    display.set_pen(0)
    display.rectangle(0, 0, WIDTH, 20)
    display.set_pen(15)
    display.text("News", 3, 4)
    display.text("Page: " + str(state["current_page"] + 1), WIDTH - display.measure_text("Page:  ") - 4, 4)
    display.set_pen(0)

    display.set_font("bitmap8")

    # Draw articles from the feed if they're available.
    if feed:
        page = state["current_page"]
        display.set_pen(0)
        display.text(feed[page]["title"], 2, 30, WIDTH - 130, 2)
        code.set_text(feed[page]["guid"])
        draw_qr_code(WIDTH - 100, 25, 100, code)

    else:
        display.set_pen(0)
        display.rectangle(0, 60, WIDTH, 25)
        display.set_pen(15)
        display.text("Unable to display news! Check your network settings in WIFI_CONFIG.py", 5, 65, WIDTH, 1)

    display.update()


draw_page()

while 1:

    changed = False

    if button_down.value():
        if state["current_page"] < 2:
            state["current_page"] += 1
            changed = True

    if button_up.value():
        if state["current_page"] > 0:
            state["current_page"] -= 1
            changed = True

    if button_a.value():
        state["feed"] = 0
        state["current_page"] = 0
        feed = get_rss(URL[state["feed"]])
        badger_os.state_save("news", state)
        changed = True

    if button_b.value():
        state["feed"] = 1
        state["current_page"] = 0
        feed = get_rss(URL[state["feed"]])
        badger_os.state_save("news", state)
        changed = True

    if button_c.value():
        state["feed"] = 2
        state["current_page"] = 0
        feed = get_rss(URL[state["feed"]])
        badger_os.state_save("news", state)
        changed = True

    if changed:
        draw_page()
