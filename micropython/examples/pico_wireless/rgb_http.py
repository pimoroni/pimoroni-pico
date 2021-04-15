import time
import picowireless
from micropython import const

WIFI_SSID = "your SSID here!"
WIFI_PASS = "Your PSK here!"

TCP_CLOSED = 0
TCP_LISTEN = 1

CLOUDFLARE_DNS = (1, 1, 1, 1)
GOOGLE_DNS = (8, 8, 8, 8)

TCP_MODE = const(0)
HTTP_REQUEST_DELAY = const(30)
HTTP_PORT = 80

routes = {}

r = 0
g = 0
b = 0


def start_wifi():
    picowireless.init()

    print("Connecting to {}...".format(WIFI_SSID))
    picowireless.wifi_set_passphrase(WIFI_SSID, WIFI_PASS)

    while True:
        if picowireless.get_connection_status() == 3:
            break
    print("Connected!")


def start_server(http_port, timeout=1.0):
    my_ip = picowireless.get_ip_address()
    print("Starting server...")
    server_sock = picowireless.get_socket()
    picowireless.server_start(http_port, server_sock, 0)

    t_start = time.time()

    while time.time() - t_start < timeout:
        state = picowireless.get_server_state(server_sock)
        if state == TCP_LISTEN:
            print("Server listening on {1}.{2}.{3}.{4}:{0}".format(http_port, *my_ip))
            return server_sock

    return None


def handle_http_request(server_sock, timeout=1.0):
    t_start = time.time()

    client_sock = picowireless.avail_server(server_sock)
    if client_sock in [server_sock, 255, -1]:
        return False

    print("Client connected!")

    avail_length = picowireless.avail_data(client_sock)
    if avail_length == 0:
        picowireless.client_stop(client_sock)
        return False

    request = b""

    while len(request) < avail_length:
        data = picowireless.get_data_buf(client_sock)
        request += data
        if time.time() - t_start > timeout:
            print("Client timed out getting data!")
            picowireless.client_stop(client_sock)
            return False

    request = request.decode("utf-8")

    if len(request) > 0:
        head, body = request.split("\r\n\r\n", 1)
        dhead = {}

        for line in head.split("\r\n")[1:]:
            key, value = line.split(": ", 1)
            dhead[key] = value

    method, url, _ = head.split("\r\n", 1)[0].split(" ")

    print("Serving {} on {}...".format(method, url))

    response = None

    # Dispatch the request to the relevant route
    if url in routes and method in routes[url] and callable(routes[url][method]):
        if method == "POST":
            data = {}
            for var in body.split("&"):
                key, value = var.split("=")
                data[key] = value
            response = routes[url][method](method, url, data)
        else:
            response = routes[url][method](method, url)

    if response is not None:
        response = "HTTP/1.1 200 OK\r\nContent-Length: {}\r\nContent-Type: text/html\r\n\r\n".format(len(response)) + response
        picowireless.send_data(client_sock, response)
        picowireless.client_stop(client_sock)
        print("Success! Sending 200 OK")
        return True
    else:
        picowireless.send_data(client_sock, "HTTP/1.1 501 Not Implemented\r\nContent-Length: 19\r\n\r\n501 Not Implemented")
        picowireless.client_stop(client_sock)
        print("Unhandled Request! Sending 501 OK")
        return False


def route(url, methods="GET"):
    if type(methods) is str:
        methods = [methods]

    def decorate(handler):
        for method in methods:
            if url not in routes:
                routes[url] = {}
            routes[url][method] = handler

    return decorate


# Edit your routes here
# Nothing fancy is supported, just plain ol' URLs and GET/POST methods
@route("/", methods=["GET", "POST"])
def get_home(method, url, data=None):
    if method == "POST":
        global r, g, b
        r = int(data.get("r", 0))
        g = int(data.get("g", 0))
        b = int(data.get("b", 0))
        picowireless.set_led(r, g, b)
        print("Set LED to {} {} {}".format(r, g, b))

    return """<form method="post" action="/">
    <input id="r" name="r" type="number" value="{r}" />
    <input name="g" type="number" value="{g}"  />
    <input name="b" type="number" value="{b}"  />
    <input type="submit" value="Set LED" />
</form>""".format(r=r, g=g, b=b)


@route("/test", methods="GET")
def get_test(method, url):
    return "Hello World!"


start_wifi()

server_sock = start_server(HTTP_PORT)
while True:
    handle_http_request(server_sock)
    time.sleep(0.01)


# Whoa there! Did you know you could run the server polling loop
# on Pico's *other* core!? Here's how:
#
# import _thread
#
# def server_loop_forever():
#    # Start a server and continuously poll for HTTP requests
#    server_sock = start_server(HTTP_PORT)
#    while True:
#        handle_http_request(server_sock)
#        time.sleep(0.01)
#
# Handle the server polling loop on the other core!
# _thread.start_new_thread(server_loop_forever, ())
#
# # Your very own main loop for fun and profit!
# while True:
#     print("Colour: {} {} {}".format(r, g, b))
#     time.sleep(5.0)
