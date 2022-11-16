import WIFI_CONFIG
import time
from gfx_pack import GfxPack, SWITCH_A, SWITCH_B, SWITCH_C, SWITCH_D, SWITCH_E 
from network_manager import NetworkManager
import urequests
import uasyncio
import json
# Cooridantes for Sheffield UK
Lat = -1.4659
Lng = 53.38297
URL = 'https://api.sunrise-sunset.org/json?lat={0}&lng={1}&date=today'.format(lat, lng)

gp = GfxPack()
 display = gp.display

WIDTH, HEIGHT = display.get_bounds()
display.set_backlight(0.5)  # turn off the white component of the backlight

sun =[
    0b11011011,
    0b00111100,
    0b11100111,
    0b11100111,
    

