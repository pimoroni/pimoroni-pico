import time

try:
    import ppwhttp
except ImportError:
    raise RuntimeError("Cannot find ppwhttp. Have you copied ppwhttp.py to your Pico?")


r = 0
g = 0
b = 0


# Edit your routes here
# Nothing fancy is supported, just plain ol' URLs and GET/POST methods
@ppwhttp.route("/", methods=["GET", "POST"])
def get_home(method, url, data=None):
    if method == "POST":
        global r, g, b
        r = int(data.get("r", 0))
        g = int(data.get("g", 0))
        b = int(data.get("b", 0))
        ppwhttp.set_led(r, g, b)
        print("Set LED to {} {} {}".format(r, g, b))

    return """<form method="post" action="/">
    <input id="r" name="r" type="number" value="{r}" />
    <input name="g" type="number" value="{g}"  />
    <input name="b" type="number" value="{b}"  />
    <input type="submit" value="Set LED" />
</form>""".format(r=r, g=g, b=b)


@ppwhttp.route("/test", methods="GET")
def get_test(method, url):
    return "Hello World!"


ppwhttp.start_wifi()

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
