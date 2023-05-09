#!/usr/bin/env micropython

import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
from automation import Automation2040W
import time


board = Automation2040W()
is_mini = False

# Uncomment for Automation2040WMini
'''
from automation import Automation2040WMini
board = Automation2040WMini()
is_mini = True
'''


def status_handler(mode, status, ip):

    print("Network: {}".format(WIFI_CONFIG.SSID))
    status_text = "Connecting..."
    board.conn_led(20)
    if status is not None:
        if status:
            status_text = "Connection successful!"
            board.conn_led(True)
        else:
            status_text = "Connection failed!"
            board.conn_led(False)

    print(status_text)
    print("IP: {}".format(ip))


try:
    from tinyweb.server import webserver

except ImportError:
    network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
    # Install missing module
    import upip
    upip.install('logging')
    from tinyweb.server import webserver


# Create web server application
app = webserver()


class LEDs:

    def not_exists(self):
        return {'message': 'no data provided'}, 404

    def get(self, data):
        if 'SW_A' in data.keys():
            board.switch_led(0, int(data['SW_A']))
        if 'SW_B' in data.keys():
            board.switch_led(1, int(data['SW_B']))
        if 'CONN' in data.keys():
            board.conn_led(int(data['CONN']))
        return {'message': 'leds updated'}, 201

    def post(self, data):
        if 'one' in data.keys():
            board.output(0, int(data['one']))
        if 'two' in data.keys():
            board.output(1, int(data['two']))
        if 'three' in data.keys():
            if not is_mini:
                board.output(2, int(data['three']))

        return {'message': 'outputs updated'}, 201


class inputs:

    def not_exists(self):
        return {'message': 'no data provided'}, 404

    def get(self, data):
        if not is_mini:
            return {"one": board.read_input(0), "two": board.read_input(1), "three": board.read_input(2), "four": board.read_input(3)}, 201
        else:
            return {"one": board.read_input(0), "two": board.read_input(1), "three": "N/A", "four": "N/A"}, 201

    def post(self, data):
        return {'message': 'outputs updated'}, 201


class buttons:

    def not_exists(self):
        return {'message': 'no data provided'}, 404

    def get(self, data):
        return {"SW_A": board.switch_pressed(0), "SW_B": board.switch_pressed(1)}, 201

    def post(self, data):
        return {'message': 'outputs updated'}, 201


class ADCs:

    def not_exists(self):
        return {'message': 'no data provided'}, 404

    def get(self, data):
        return {"one": board.read_adc(0), "two": board.read_adc(1), "three": board.read_adc(2)}, 201

    def post(self, data):
        return {'message': 'outputs updated'}, 201


class outputs:

    def not_exists(self):
        return {'message': 'no data provided'}, 404

    def get(self, data):

        if 'one' in data.keys():
            print(int(data['one']))
            board.output(0, bool(int(data['one'])))
        if 'two' in data.keys():
            board.output(1, bool(int(data['two'])))
        if 'three' in data.keys():
            if not is_mini:
                board.output(2, bool(int(data['three'])))
        if not is_mini:
            return {"one": bool(board.output(0)), "two": bool(board.output(1)), "three": bool(board.output(2))}, 201
        else:
            return {"one": bool(board.output(0)), "two": bool(board.output(1)), "three": "N/A"}, 201

    def post(self, data):
        if 'one' in data.keys():
            board.output(0, bool(int(data['one'])))
        if 'two' in data.keys():
            board.output(1, bool(int(data['two'])))
        if 'three' in data.keys():
            if not is_mini:
                board.output(2, int(data['three']))
        return {'message': 'outputs updated'}, 201


class relays:

    def not_exists(self):
        return {'message': 'no data provided'}, 404

    def get(self, data):
        if 'one' in data.keys():
            if not is_mini:
                board.relay(0, int(data['one']))
            else:
                board.relay(int(data['one']))
        if 'two' in data.keys():
            if not is_mini:
                board.relay(1, int(data['two']))
        if 'three' in data.keys():
            if not is_mini:
                board.relay(2, int(data['three']))
        if not is_mini:
            return {"one": bool(board.relay(0)), "two": bool(board.relay(1)), "three": bool(board.relay(2))}, 201
        else:
            return {"one": bool(board.relay()), "two": "N/A", "three": "N/A"}, 201

    def post(self, data):
        if 'one' in data.keys():
            board.relay(0, int(data['one']))
        if 'two' in data.keys():
            if not is_mini:
                board.relay(1, int(data['two']))
        if 'three' in data.keys():
            if not is_mini:
                board.relay(2, int(data['three']))
        return {'message': 'outputs updated'}, 201


# Index page
@app.route('/')
async def index(request, response):
    if is_mini:
        await response.send_file('index_mini.html', content_type='text/html')
    else:
        await response.send_file('index.html', content_type='text/html')


# HTTP redirection
@app.route('/redirect')
async def redirect(request, response):
    # Start HTTP response with content-type text/html
    await response.redirect('/')


def run():
    # Setup wifi
    network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)

    app.add_resource(outputs, '/outputs')
    app.add_resource(relays, '/relays')
    app.add_resource(inputs, '/inputs')
    app.add_resource(ADCs, '/adcs')
    app.add_resource(LEDs, '/leds')
    app.add_resource(buttons, '/buttons')
    # Connect to Wifi network
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
    while not network_manager.isconnected():
        time.sleep(0.1)
    app.run(host='0.0.0.0', port=80)


if __name__ == '__main__':
    run()
