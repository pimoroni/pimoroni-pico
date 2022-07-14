import WIFI_CONFIG
from network_manager import NetworkManager
import time
import uasyncio
import ujson
from urllib import urequest
from picographics import PicoGraphics, DISPLAY_INKY_PACK


"""
Grab the quote of the day from Wikipedia.
"""


graphics = PicoGraphics(DISPLAY_INKY_PACK)

WIDTH, HEIGHT = graphics.get_bounds()
ENDPOINT = "https://en.wikiquote.org/w/api.php?format=json&action=expandtemplates&prop=wikitext&text={{{{Wikiquote:Quote%20of%20the%20day/{3}%20{2},%20{0}}}}}"
MONTHNAMES = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"]


last_date = ""


def parse_qotd(text):
    text = text.split("\n")
    return (
        text[6][2:].replace("[[", "").replace("]]", "").replace("<br>", '\n').replace("<br />", '\n'),  # Quote
        text[8].split("|")[2][5:-4]                                                                     # Author
    )


def status_handler(mode, status, ip):
    graphics.set_update_speed(2)
    graphics.set_pen(15)
    graphics.clear()
    graphics.set_pen(0)
    graphics.text("Network: {}".format(WIFI_CONFIG.SSID), 10, 10, scale=2)
    status_text = "Connecting..."
    if status is not None:
        if status:
            status_text = "Connection successful!"
        else:
            status_text = "Connection failed!"

    graphics.text(status_text, 10, 30, scale=2)
    graphics.text("IP: {}".format(ip), 10, 60, scale=2)
    graphics.update()


network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)

while True:
    graphics.set_font("bitmap8")
    graphics.set_update_speed(1)

    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))

    date = list(time.localtime())[:3]
    date.append(MONTHNAMES[date[1] - 1])

    if "{3} {2}, {0}".format(*date) == last_date:
        time.sleep(60)
        continue

    url = ENDPOINT.format(*date)
    print("Requesting URL: {}".format(url))
    j = ujson.load(urequest.urlopen(url))

    text = j['expandtemplates']['wikitext']

    text, author = parse_qotd(text)

    print(text)

    graphics.set_pen(15)
    graphics.clear()
    graphics.set_pen(0)
    graphics.text("QoTD - {2} {3} {0:04d}".format(*date), 10, 10, scale=2)
    graphics.text(text, 10, 30, wordwrap=WIDTH - 20, scale=1)
    graphics.text(author, 10, 108, scale=1)

    graphics.update()

    last_date = "{3} {2}, {0}".format(*date)

    time.sleep(60)
