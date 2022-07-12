#!/usr/bin/env micropython

import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
from automation import *
import time


board = Automation2040W()

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
    import tinyweb
    
except ImportError:
    # WIFI settings
    WIFI_COUNTRY = "GB"  # Changeme!
    network_manager = NetworkManager(WIFI_COUNTRY, status_handler=status_handler)
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
    #install missing module
    import upip
    upip.install('logging')
    import tinyweb
    
    

# Create web server application
app = tinyweb.webserver()


#static page
html_file = open('index.html', 'r')

# WIFI settings
WIFI_COUNTRY = "GB"  # Changeme!

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
            board.output(2, int(data['three']))
        return {'message': 'outputs updated'}, 201
        


class inputs:
    
    def not_exists(self):
        return {'message': 'no data provided'}, 404
    
    def get(self, data):
        return {"one" : board.read_input(0), "two" : board.read_input(1), "three" : board.read_input(2), "four" : board.read_input(3)}, 201
            
    def post(self, data):
        return {'message': 'outputs updated'}, 201

class buttons:
    
    def not_exists(self):
        return {'message': 'no data provided'}, 404
    
    def get(self, data):      
        return {"SW_A" : board.switch_pressed(0), "SW_B" : board.switch_pressed(1)}, 201
            
    def post(self, data): 
        return {'message': 'outputs updated'}, 201
    
class ADCs:
    
    def not_exists(self):
        return {'message': 'no data provided'}, 404
    
    def get(self, data):   
        return {"one" : board.read_adc(0), "two" : board.read_adc(1), "three" : board.read_adc(2) }, 201
            
    def post(self, data):
    
        return {'message': 'outputs updated'}, 201

class outputs:
    
    def not_exists(self):
        return {'message': 'no data provided'}, 404
    
    def get(self, data):
        if 'one' in data.keys():            
            board.output(0, int(data['one']))
        if 'two' in data.keys():      
            board.output(1, int(data['two']))
        if 'three' in data.keys():      
            board.output(2, int(data['three']))
        return {"one" : board.output(0), "two" : board.output(1), "three" : board.output(2)}, 201
            
    def post(self, data):
        if 'one' in data.keys():            
            board.output(0, int(data['one']))
        if 'two' in data.keys():      
            board.output(1, int(data['two']))
        if 'three' in data.keys():      
            board.output(2, int(data['three']))
        return {'message': 'outputs updated'}, 201
        

class relays:
    
    def not_exists(self):
        return {'message': 'no data provided'}, 404
    
    def get(self, data):
        if 'one' in data.keys():            
            board.relay(0, int(data['one']))
        if 'two' in data.keys():      
            board.relay(1, int(data['two']))
        if 'three' in data.keys():      
            board.relay(2, int(data['three']))
        return {"one" : board.relay(0), "two" : board.relay(1), "three" : board.relay(2)}, 201
            
    def post(self, data):
        if 'one' in data.keys():            
            board.relay(0, int(data['one']))
        if 'two' in data.keys():      
            board.relay(1, int(data['two']))
        if 'three' in data.keys():      
            board.relay(2, int(data['three']))
        return {'message': 'outputs updated'}, 201
    


# Index page
@app.route('/')
async def index(request, response):
    # Start HTTP response with content-type text/html
    await response.start_html()
    # Send actual HTML page
    await response.send(html_file.read())


# HTTP redirection
@app.route('/redirect')
async def redirect(request, response):
    # Start HTTP response with content-type text/html
    await response.redirect('/')
    




def run():
    # set up wifi
    network_manager = NetworkManager(WIFI_COUNTRY, status_handler=status_handler)

    app.add_resource(outputs, '/outputs')
    app.add_resource(relays, '/relays')
    app.add_resource(inputs, '/inputs')
    app.add_resource(ADCs, '/adcs')
    app.add_resource(LEDs, '/leds')
    app.add_resource(buttons, '/buttons')
    #Connect to Wifi network
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
    while (not network_manager.isconnected()):
        time.sleep(0.1)
    app.run(host='0.0.0.0', port=80)


if __name__ == '__main__':
    run()