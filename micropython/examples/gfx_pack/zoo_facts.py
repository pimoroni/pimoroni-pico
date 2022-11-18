'''
zoo_facts.py
This example is for the Pico W with GFX Pack.
It uses the Zoo Animal API to download a list of 5 animals,
then displays them on the GFX Pack
A = Next animal
B = Last animal
D = Show stats
E = Fetch a different 5 animals
Find out more about Zoo Animal API here:
https://zoo-animal-api.herokuapp.com/
'''
import WIFI_CONFIG
import time
from gfx_pack import GfxPack, SWITCH_A, SWITCH_B, SWITCH_D, SWITCH_E
from network_manager import NetworkManager
import urequests
import uasyncio

URL = 'https://zoo-animal-api.herokuapp.com/animals/rand/5'

gp = GfxPack()
display = gp.display

WIDTH, HEIGHT = display.get_bounds()
display.set_backlight(0.5)  # turn off the white component of the backlight
animals = []
animal_number = 0
stat_page = False
sys_status = "STATUS"


# Data class for containing the animal facts
class Animal:
    def __init__(self):
        self.name = ""
        self.latin_name = ""
        self.animal_type = ""
        self.habitat = ""
        self.diet = ""
        self.length_max = ""
        self.weight_max = ""
        self.lifespan = ""

    def process_json(self, json):
        print(json['name'])
        self.name = json['name']
        self.latin_name = json['latin_name']
        self.animal_type = json['animal_type']
        self.habitat = json['habitat']
        self.diet = json['diet']
        self.length_max = json['length_max']
        self.weight_max = json['weight_max']
        self.lifespan = json['lifespan']


def get_data():
    # open the json file
    print(f'Requesting URL: {URL}')
    r = urequests.get(URL)
    # open the json data
    j = r.json()
    print('Data obtained!')
    r.close()
    return j


def get_animals():
    global sys_status
    animals = []
    sys_status = 'Getting Animals'
    display_status()
    display.update()
    json_data = get_data()
    for index in range(len(json_data)):
        new_animal = Animal()
        new_animal.process_json(json_data[index])
        animals.append(new_animal)
    return animals


def display_status():
    global sys_status
    display.set_pen(0)  # Set pen to white
    display.clear()
    display.set_pen(15)
    display.text(sys_status, 0, 0, WIDTH, 1)
    display.update()


def status_handler(mode, status, ip):
    # reports wifi connection status
    global sys_status
    print(mode, status, ip)
    sys_status = 'Mode: {0} Connected: {1} IP: {2}'.format(mode, status, ip)
    display_status()
    display.update()
    print('Connecting to wifi...')
    if status is not None:
        if status:
            print('Wifi connection successful!')
        else:
            print('Wifi connection failed!')


def display_animal(animal, stat_page):
    display.set_pen(0)  # Set pen to white
    display.clear()
    display.set_pen(15)
    if stat_page is False:
        display.text('Animal Info {0}'.format(animal_number), 0, 0, WIDTH, 1)
        display.text('Name: {0}'.format(animal.name[:19]), 0, 10, WIDTH, 1)
        display.text('Latin: {0}'.format(animal.latin_name[:19]), 0, 20, WIDTH, 1)
        display.text('Type: {0}'.format(animal.animal_type[:19]), 0, 30, WIDTH, 1)
        display.text('Habitat: {0}'.format(animal.habitat[:19]), 0, 40, WIDTH, 1)
        display.text('Diet: {0}'.format(animal.diet[:19]), 0, 50, WIDTH, 1)

    else:
        display.text('Animal Stats {0}'.format(animal_number), 0, 0, WIDTH, 1)
        display.text('Max Length: {0}'.format(animal.length_max), 0, 10, WIDTH, 1)
        display.text('Max Weight: {0}'.format(animal.weight_max), 0, 20, WIDTH, 1)
        display.text('Lifespan: {0}'.format(animal.lifespan), 0, 30, WIDTH, 1)
    display.update()


try:
    network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
except Exception as e:
    print(f'Wifi connection failed! {e}')


# From CPython Lib/colorsys.py
def hsv_to_rgb(h, s, v):
    if s == 0.0:
        return v, v, v
    i = int(h * 6.0)
    f = (h * 6.0) - i
    p = v * (1.0 - s)
    q = v * (1.0 - s * f)
    t = v * (1.0 - s * (1.0 - f))
    i = i % 6
    if i == 0:
        return v, t, p
    if i == 1:
        return q, v, p
    if i == 2:
        return p, v, t
    if i == 3:
        return p, q, v
    if i == 4:
        return t, p, v
    if i == 5:
        return v, p, q


# some variables to keep track of rainbow background
h = 0

display.set_font("bitmap8")

animals = get_animals()
display.set_backlight(0)
while True:

    h += 1
    r, g, b = [int(255 * c) for c in hsv_to_rgb(h / 360.0, 1.0, 1.0)]  # rainbow magic
    gp.set_backlight(r, g, b)  # Set backlight to a converted HSV value
    display.set_pen(0)  # Set pen to white
    display.clear()
    display.set_pen(15)  # Set pen to black
    # Draw text
    display_animal(animals[animal_number], stat_page)

    if gp.switch_pressed(SWITCH_B):
        animal_number += 1
        if animal_number > 4:
            animal_number = 0
        display_animal(animals[animal_number], stat_page)
        time.sleep(0.4)

    elif gp.switch_pressed(SWITCH_A):
        animal_number -= 1
        if animal_number < 0:
            animal_number = 4
        display_animal(animals[animal_number], stat_page)
        time.sleep(0.4)

    elif gp.switch_pressed(SWITCH_D):
        stat_page = not stat_page
        display_animal(animals[animal_number], stat_page)
        time.sleep(0.4)

    elif gp.switch_pressed(SWITCH_E):
        animals = get_animals()
        display_animal(animals[animal_number], stat_page)
