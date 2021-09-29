import time
import json
try:
    import xmltok  # https://pypi.org/project/micropython-xmltok/
    import io
except ImportError:
    xmltok = None
from micropython import const

try:
    import ppwhttp
except ImportError:
    raise RuntimeError("Cannot find ppwhttp. Have you copied ppwhttp.py to your Pico?")


HTTP_REQUEST_DELAY = const(60)
HTTP_REQUEST_PORT = const(80)
HTTP_REQUEST_HOST = "api.thingspeak.com"
HTTP_REQUEST_PATH = "/channels/1417/field/2/last.txt"


ppwhttp.start_wifi()
ppwhttp.set_dns(ppwhttp.GOOGLE_DNS)

# Get our own local IP!
my_ip = ppwhttp.get_ip_address()
print("Local IP: {}.{}.{}.{}".format(*my_ip))


def handler(head, body):
    if head["Status"] == "200 OK":
        if HTTP_REQUEST_PATH.endswith(".json"):
            # Parse as JSON
            data = json.loads(body)
            color = data['field2'][1:]

        elif HTTP_REQUEST_PATH.endswith(".xml") and xmltok is not None:
            # Parse as XML
            data = xmltok.tokenize(io.StringIO(body))
            color = xmltok.text_of(data, "field2")[1:]

        elif HTTP_REQUEST_PATH.endswith(".txt"):
            # Parse as plain text
            color = body[1:]

        else:
            print("Unable to parse API response!")
            return
        r = int(color[0:2], 16)
        g = int(color[2:4], 16)
        b = int(color[4:6], 16)
        ppwhttp.set_led(r, g, b)
        print("Set LED to {} {} {}".format(r, g, b))
    else:
        print("Error: {}".format(head["Status"]))


while True:
    ppwhttp.http_request(HTTP_REQUEST_HOST, HTTP_REQUEST_PORT, HTTP_REQUEST_HOST, HTTP_REQUEST_PATH, handler)
    time.sleep(HTTP_REQUEST_DELAY)
