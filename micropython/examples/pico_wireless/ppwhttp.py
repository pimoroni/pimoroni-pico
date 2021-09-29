"""Pimoroni Pico Wireless HTTP

A super-simple HTTP server library for Pico Wireless.
"""
import time
import picowireless

from micropython import const

try:
    from secrets import WIFI_SSID, WIFI_PASS
except ImportError:
    WIFI_SSID = None
    WIFI_PASS = None


TCP_CLOSED = const(0)
TCP_LISTEN = const(1)

CLOUDFLARE_DNS = (1, 1, 1, 1)
GOOGLE_DNS = (8, 8, 8, 8)

DEFAULT_HTTP_PORT = const(80)


routes = {}
sockets = []
hosts = {}


def set_led(r, g, b):
    """Set """
    picowireless.set_led(r, g, b)


def get_socket(force_new=False):
    global sockets
    if force_new or len(sockets) == 0:
        socket = picowireless.get_socket()
        sockets.append(socket)
        return socket
    return sockets[0]


def get_ip_address():
    return picowireless.get_ip_address()


def set_dns(dns):
    picowireless.set_dns(dns)


def get_host_by_name(hostname, no_cache=False):
    # Already an IP
    if type(hostname) is tuple and len(hostname) == 4:
        return hostname

    # Get from cached hosts
    if hostname in hosts and not no_cache:
        return hosts[hostname]

    ip = picowireless.get_host_by_name(hostname)
    hosts[hostname] = ip
    return ip


def start_wifi(wifi_ssid=WIFI_SSID, wifi_pass=WIFI_PASS):
    if wifi_ssid is None or wifi_pass is None:
        raise RuntimeError("WiFi SSID/PASS required. Set them in secrets.py and copy it to your Pico, or pass them as arguments.")
    picowireless.init()

    print("Connecting to {}...".format(wifi_ssid))
    picowireless.wifi_set_passphrase(wifi_ssid, wifi_pass)

    while True:
        if picowireless.get_connection_status() == 3:
            break
    print("Connected!")


def start_server(http_port=DEFAULT_HTTP_PORT, timeout=5000):
    my_ip = picowireless.get_ip_address()
    print("Starting server...")
    server_sock = picowireless.get_socket()
    picowireless.server_start(http_port, server_sock, 0)

    t_start = time.ticks_ms()

    while time.ticks_ms() - t_start < timeout:
        state = picowireless.get_server_state(server_sock)
        if state == TCP_LISTEN:
            print("Server listening on {1}.{2}.{3}.{4}:{0}".format(http_port, *my_ip))
            return server_sock

    return None


def connect_to_server(host_address, port, client_sock, timeout=5000):
    picowireless.client_start(host_address, port, client_sock, TCP_CLOSED)

    t_start = time.ticks_ms()

    while time.ticks_ms() - t_start < timeout:
        state = picowireless.get_client_state(client_sock)
        if state == 4:
            return True
        time.sleep(1.0)

    return False


def http_request(host_address, port, request_host, request_path, handler, timeout=5000, client_sock=None):
    if client_sock is None:
        client_sock = get_socket()

    host_address = get_host_by_name(host_address)

    print("Connecting to {1}.{2}.{3}.{4}:{0}...".format(port, *host_address))
    if not connect_to_server(host_address, port, client_sock):
        print("Connection failed!")
        return False
    print("Connected!")

    http_request = """GET {} HTTP/1.1
Host: {}
Connection: close

""".format(request_path, request_host).replace("\n", "\r\n")

    picowireless.send_data(client_sock, http_request)

    t_start = time.ticks_ms()

    while True:
        if time.ticks_ms() - t_start > timeout:
            picowireless.client_stop(client_sock)
            print("HTTP request to {}:{} timed out...".format(host_address, port))
            return False

        avail_length = picowireless.avail_data(client_sock)
        if avail_length > 0:
            break

    print("Got response: {} bytes".format(avail_length))

    response = b""

    while len(response) < avail_length:
        data = picowireless.get_data_buf(client_sock)
        response += data

    response = response.decode("utf-8")

    head, body = response.split("\r\n\r\n", 1)
    dhead = {}

    for line in head.split("\r\n")[1:]:
        key, value = line.split(": ", 1)
        dhead[key] = value

    # Fudge to handle JSON data type, which is prefixed with a content length.
    # This ignores the charset, if specified, since we've already assumed utf-8 above!
    # TODO maybe pay attention to Content-Type charset
    if "Content-Type" in dhead and dhead["Content-Type"].startswith("application/json"):
        length, body = body.split("\n", 1)
        length = int(length, 16)
        body = body[:length]

    handler(dhead, body)

    picowireless.client_stop(client_sock)


def handle_http_request(server_sock, timeout=5000):
    t_start = time.ticks_ms()

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
        if time.ticks_ms() - t_start > timeout:
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
