import gc
import time
import ujson
import uasyncio
import WIFI_CONFIG
from urllib import urequest
from network_manager import NetworkManager
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY      # 5.7"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"


ENDPOINT = "https://en.wikiquote.org/w/api.php?format=json&action=expandtemplates&prop=wikitext&text={{{{Wikiquote:Quote%20of%20the%20day/{3}%20{2},%20{0}}}}}"
MONTHNAMES = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"]


last_date = ""


def parse_qotd(text):
    print(text)
    text = text.split("\n")
    author = text[8].split("|")[2][5:-4]
    text = text[6][2:]
    gc.collect()
    return text, author


def status_handler(mode, status, ip):
    print(mode, status, ip)


network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)

gc.collect()
graphics = PicoGraphics(DISPLAY)
WIDTH, HEIGHT = graphics.get_bounds()
graphics.set_font("bitmap8")
gc.collect()


BADCHARS = {
    "’": "'",
    "—": "",
    "…": "..."
}


def display_quote(text, ox, oy, scale, wordwrap):
    # Processing text is memory intensive
    # so we'll do it one char at a time as we draw to the screen
    line_height = 9 * scale
    html_tag = ""
    word = ""
    extra_text = ""
    space_width = graphics.measure_text(" ", scale=scale)
    x = ox
    y = oy
    i = -1
    while True:
        if len(extra_text) == 0:
            i += 1
        if i >= len(text):
            break

        if len(extra_text) > 0:
            char = extra_text[0]
            extra_text = extra_text[1:]
        else:
            char = text[i]

        if char in BADCHARS:
            word += BADCHARS[char]
            continue

        # Unpick stuff like [[word]] and [[disambiguation|word]]
        # and  [[w:wikipedia_page|word]]
        # test cases: July 8th 2022, July 12th 2022
        if char == "[":
            if text[i:i + 2] == "[[":
                link = False
                if text[i + 2:i + 4] == "w:":
                    link = True
                    i += 2
                end = text[i:].index("]]")
                if "|" in text[i + 2:i + end]:
                    parts = text[i + 2:i + end].split("|")
                    word = parts[1]
                    if not link:
                        extra_text = " (" + parts[0] + ")"
                else:
                    word = text[i + 2:i + end]
                i += end + 1
                continue

        if char == "&":
            if text[i:i + 5] == "&amp;":
                word += "&"
                i += 4
                continue

        if char == "<":
            j = i + text[i:].index(">")
            html_tag = text[i + 1:j].replace("/", "").strip()
            i = j
            continue

        if char in (" ", "\n") or html_tag == "br":
            w = graphics.measure_text(word, scale=scale)
            if x + w > wordwrap or char == "\n" or html_tag == "br":
                x = ox
                y += line_height

            graphics.text(word, x, y, scale=scale)
            word = ""
            html_tag = ""
            x += w + space_width
            continue

        word += char

    # Last word
    w = graphics.measure_text(word, scale=scale)
    if x + w > wordwrap:
        x = ox
        y += line_height

    graphics.text(word, x, y, scale=scale)


while True:
    gc.collect()

    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))

    date = list(time.localtime())[:3]
    date.append(MONTHNAMES[date[1] - 1])

    if "{3} {2}, {0}".format(*date) == last_date:
        time.sleep(60)
        continue

    url = ENDPOINT.format(*date)
    print("Requesting URL: {}".format(url))
    socket = urequest.urlopen(url)
    j = ujson.load(socket)
    socket.close()

    text = j['expandtemplates']['wikitext']
    del j
    gc.collect()

    text, author = parse_qotd(text)

    print(text)

    graphics.set_pen(1)
    graphics.clear()
    graphics.set_pen(0)
    graphics.text("QoTD - {2} {3} {0:04d}".format(*date), 10, 10, scale=3)

    display_quote(text, 10, 40, 2, wordwrap=WIDTH - 20)

    graphics.text(author, 10, HEIGHT - 20, scale=2)

    graphics.update()
    gc.collect()

    last_date = "{3} {2}, {0}".format(*date)

    time.sleep(60)
