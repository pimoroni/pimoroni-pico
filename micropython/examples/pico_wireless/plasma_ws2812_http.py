import time
import plasma

try:
    import ppwhttp
except ImportError:
    raise RuntimeError("Cannot find ppwhttp. Have you copied ppwhttp.py to your Pico?")

"""
This example uses the Plasma WS2812 LED library to drive a string of LEDs alongside the built-in RGB LED.
You should wire your LEDs to VBUS/GND and connect the data pin to pin 27 (unused by Pico Wireless).

Go to: https://<address>:<port>/set_led/<index> to set a single LED
"""

NUM_LEDS = 30  # Number of connected LEDs
LED_PIN = 27   # LED data pin (27 is unused by Pico Wireless)
LED_PIO = 0    # Hardware PIO (0 or 1)
LED_SM = 0     # PIO State-Machine (0 to 3)


r = 0
g = 0
b = 0

led_strip = plasma.WS2812(NUM_LEDS, LED_PIO, LED_SM, LED_PIN)


# Edit your routes here
# Nothing fancy is supported, just plain ol' URLs and GET/POST methods
@ppwhttp.route("/", methods=["GET", "POST"])
def get_home(method, url, data):
    if method == "POST":
        global r, g, b
        r = int(data.get("r", 0))
        g = int(data.get("g", 0))
        b = int(data.get("b", 0))
        ppwhttp.set_led(r, g, b)
        for i in range(NUM_LEDS):
            led_strip.set_rgb(i, r, g, b)
        print("Set LED to {} {} {}".format(r, g, b))

    return """<form method="post" action="/">
    <input id="r" name="r" type="number" value="{r}" />
    <input name="g" type="number" value="{g}"  />
    <input name="b" type="number" value="{b}"  />
    <input type="submit" value="Set LED" />
</form>""".format(r=r, g=g, b=b)


# This wildcard route allows us to visit eg `/set_led/<index>`
# to get/set the state of LED <index>
# You should *probably* not modify state with GET, even though you can
# so we use a form and POST to handle changing things.
@ppwhttp.route("/set_led/<int:index>", methods=["GET", "POST"])
def set_led(method, url, data):
    i = int(data.get("index", 0))

    if method == "POST":
        r = int(data.get("r", 0))
        g = int(data.get("g", 0))
        b = int(data.get("b", 0))
        led_strip.set_rgb(i, r, g, b)
        print("Set LED to {} {} {}".format(r, g, b))
    else:
        # TODO Fix WS2812 / APA102 get methods to correct for colour order/alignment
        r, g, b, w = led_strip.get(i)
        r = int(r)
        g = int(g)
        b = int(b)

    return """LED: {i}<br /><form method="post" action="/set_led/{i}">
    <input id="r" name="r" type="number" value="{r}" />
    <input name="g" type="number" value="{g}"  />
    <input name="b" type="number" value="{b}"  />
    <input type="submit" value="Set LED" />
</form>""".format(i=i, r=r, g=g, b=b)


# This wildcard route allows us to visit eg `/get_led/<index>`
# to get the state of LED <index>
@ppwhttp.route("/get_led/<int:index>", methods="GET")
def get_led(method, url, data):
    i = data.get("index", 0)
    # TODO Fix WS2812 / APA102 get methods to correct for colour order/alignment
    r, g, b, w = led_strip.get(i)
    return "LED: {}<br />R: {:0.0f}<br />G: {:0.0f}<br />B: {:0.0f}".format(i, r, g, b)


@ppwhttp.route("/test", methods="GET")
def get_test(method, url):
    return "Hello World!"


ppwhttp.start_wifi()

led_strip.start()

server_sock = ppwhttp.start_server()
while True:
    ppwhttp.handle_http_request(server_sock)
    time.sleep(0.01)


# Whoa there! Did you know you could run the server polling loop
# on Pico's *other* core!? Here's how:
#
# import _thread
#
# def server_loop_forever():
#    # Start a server and continuously poll for HTTP requests
#    server_sock = ppwhttp.start_server()
#    while True:
#        ppwhttp.handle_http_request(server_sock)
#        time.sleep(0.01)
#
# Handle the server polling loop on the other core!
# _thread.start_new_thread(server_loop_forever, ())
#
# # Your very own main loop for fun and profit!
# while True:
#     print("Colour: {} {} {}".format(r, g, b))
#     time.sleep(5.0)
