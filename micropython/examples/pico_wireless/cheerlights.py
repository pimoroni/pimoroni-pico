import time
import picowireless
from micropython import const

WIFI_SSID = "your SSID here!"
WIFI_PASS = "Your PSK here!"

CLOUDFLARE_DNS = (1, 1, 1, 1)
GOOGLE_DNS = (8, 8, 8, 8)
USE_DNS = CLOUDFLARE_DNS

TCP_MODE = const(0)
HTTP_REQUEST_DELAY = const(30)
HTTP_PORT = 80
HTTP_REQUEST_HOST = "api.thingspeak.com"
HTTP_REQUEST_PATH = "/channels/1417/field/2/last.txt"


def connect(host_address, port, client_sock, timeout=1000):
    picowireless.client_start(host_address, port, client_sock, TCP_MODE)

    t_start = time.time()
    timeout /= 1000.0

    while time.time() - t_start < timeout:
        state = picowireless.get_client_state(client_sock)
        if state == 4:
            return True
        time.sleep(1.0)

    return False


def http_request(client_sock, host_address, port, request_host, request_path, handler):
    print("Connecting to {1}.{2}.{3}.{4}:{0}...".format(port, *host_address))
    if not connect(host_address, port, client_sock):
        print("Connection failed!")
        return False
    print("Connected!")

    http_request = """GET {} HTTP/1.1
Host: {}
Connection: close

""".format(request_path, request_host).replace("\n", "\r\n")

    picowireless.send_data(client_sock, http_request)

    while True:
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

    handler(dhead, body)

    picowireless.client_stop(client_sock)


picowireless.init()

print("Connecting to {}...".format(WIFI_SSID))
picowireless.wifi_set_passphrase(WIFI_SSID, WIFI_PASS)

while True:
    if picowireless.get_connection_status() == 3:
        break
print("Connected!")

# Get our own local IP!
my_ip = picowireless.get_ip_address()
print("Local IP: {}.{}.{}.{}".format(*my_ip))

# Resolve and cache the IP address
picowireless.set_dns(USE_DNS)
http_address = picowireless.get_host_by_name(HTTP_REQUEST_HOST)
print("Resolved {} to {}.{}.{}.{}".format(HTTP_REQUEST_HOST, *http_address))

client_sock = picowireless.get_socket()


def handler(head, body):
    if head["Status"] == "200 OK":
        color = body[1:]
        r = int(color[0:2], 16)
        g = int(color[2:4], 16)
        b = int(color[4:6], 16)
        picowireless.set_led(r, g, b)
        print("Set LED to {} {} {}".format(r, g, b))
    else:
        print("Error: {}".format(head["Status"]))


while True:
    http_request(client_sock, http_address, HTTP_PORT, HTTP_REQUEST_HOST, HTTP_REQUEST_PATH, handler)
    time.sleep(60.0)
