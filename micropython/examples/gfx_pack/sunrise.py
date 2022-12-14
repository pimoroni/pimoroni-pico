'''
sunrise.py
This example is for the Pico W with GFX Pack.
This displays information from the Sunrise Sunset API:
Find out more here - https://sunrise-sunset.org/api
Also shows how to use a 16x16 animated sprite.
'''
import WIFI_CONFIG
import time
from math import radians, sin, cos
from random import randint
from gfx_pack import GfxPack
from network_manager import NetworkManager
import ntptime
import urequests
import uasyncio
import machine


# Helper class so that the different formats of time can be converted into one comparable format
# Makes up for the lack of a datetime module in MicroPython
class TimeObj:
    def __init__(self):
        self.secs = 0
        self.mins = 0
        self.hours = 0
        self.PM = False

    # Returns time variables as a tuple (h, m, s)
    def get_time(self):
        pm_hrs = 0
        if self.PM:
            pm_hrs = 12
        return (self.hours + pm_hrs, self.mins, self.secs)

    # Returns time variables as a single string
    def get_str(self):
        h, m, s = self.get_time()
        return "{0:02d}:{1:02d}:{2:02d}".format(h, m, s)

    # Set time variables from the sunrise-sunset API
    def parse_api(self, apiStr):
        strsplit = apiStr.split()
        if strsplit[1] == 'PM':
            self.PM = True
        timesplit = strsplit[0].split(':')
        self.hours = int(timesplit[0])
        self.mins = int(timesplit[1])
        self.secs = int(timesplit[2])

    # Set time variables form
    def parse_localtime(self, localtpl):
        yr, mo, dy, self.hours, self.mins, self.secs, un1, un2 = localtpl

    # Returns number of seconds since midnight
    def get_total_secs(self):
        seconds = 0
        if self.PM:
            seconds += 43200  # seconds in the first 12 hours
        seconds += (self.hours * 3600)
        seconds += (self.mins * 60)
        seconds += self.secs
        return seconds


# Instances of TimeObj helper class defined above
sunrise_obj = TimeObj()
sunset_obj = TimeObj()
currenttime = TimeObj()

# Coordinates for Sheffield-on-Sea, UK
lng = -1.4659
lat = 53.3829
# Coordinates for LA, USA
# lng = -118.2437
# lat = 34.0522
URL = 'https://api.sunrise-sunset.org/json?lat={0}&lng={1}&date=today'.format(lat, lng)
rtc = machine.RTC()

gp = GfxPack()
display = gp.display
sys_status = "Starting"
WIDTH, HEIGHT = display.get_bounds()
display.set_backlight(1)  # turn on the white component of the backlight

# Generate hill heights
hills1 = [randint(10, 18), randint(10, 18), randint(10, 18), randint(10, 18), randint(10, 18)]
hills2 = [randint(10, 18), randint(10, 18)]
hills3 = [randint(10, 18), randint(10, 18)]

# Sprite information for sun icon
sun = [
    [
        0b0000000100000000,
        0b0000000000000000,
        0b0010011111001000,
        0b0000100000100000,
        0b0001000000010000,
        0b0010000000001000,
        0b0010010001001000,
        0b0010000100001000,
        0b0010000000001000,
        0b0010000000001000,
        0b0001001110010000,
        0b0000100000100000,
        0b0010011111001000,
        0b0000000000000000,
        0b0000000100000000,
        0b0000000000000000
    ],
    [
        0b0000000000000000,
        0b0100000000000100,
        0b0000011111000000,
        0b0000100000100000,
        0b0001000000010000,
        0b0010010001001000,
        0b0010010001001000,
        0b1010000100001010,
        0b0010000000001000,
        0b0010010001001000,
        0b0001001110010000,
        0b0000100000100000,
        0b0000011111000000,
        0b0100000000000100,
        0b0000000000000000,
        0b0000000000000000
    ],
    [
        0b0000000100000000,
        0b0100000000000100,
        0b0010011111001000,
        0b0000100000100000,
        0b0001000000010000,
        0b0010010001001000,
        0b0010010001001000,
        0b1010000100001010,
        0b0010000000001000,
        0b0010010001001000,
        0b0001001110010000,
        0b0000100000100000,
        0b0010011111001000,
        0b0100000000000100,
        0b0000000100000000,
        0b0000000000000000
    ]
]


def get_data():
    # open the json file
    print(f'Requesting URL: {URL}')
    r = urequests.get(URL)
    # open the json data
    j = r.json()
    print('Data obtained!')
    r.close()
    return j


def get_sunrise():
    sun_json = get_data()
    sunrise = sun_json['results']['sunrise']
    sunrise_obj.parse_api(sunrise)
    sunset = sun_json['results']['sunset']
    sunset_obj.parse_api(sunset)


def display_status():
    global sys_status
    display.set_pen(0)
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


def calc_circle_points(ori_x, ori_y, r, deg):
    rads = radians(deg - 180)
    x = int(cos(rads) * r) + ori_x
    y = int(sin(rads) * r) + ori_y
    return x, y


def to_seconds(hour, mins, secs, isPM=False):
    seconds = 0
    if isPM:
        seconds += 43200  # Seconds in the first 12 hours
    seconds += (hour * 3600)
    seconds += (mins * 60)
    seconds += secs
    return seconds


def draw_hills():
    display.set_pen(12)
    for x in range(5):
        display.circle(30 * x, 64, hills1[x])
    display.set_pen(9)
    for x in range(2):
        display.circle(60 * x + 15, 64, hills2[x])
    display.set_pen(3)
    for x in range(2):
        display.circle(60 * x + 30 + 15, 64, hills3[x])


def draw_text():
    display.set_pen(15)
    display.text("Sun Rise-Set API demo", 0, 0, WIDTH, 1)
    display.text("Sunrise: {0}".format(sunrise_obj.get_str()), 0, 8, WIDTH, 1)
    display.text("Sunset: {0}".format(sunset_obj.get_str()), 0, 16, WIDTH, 1)
    display.text("{0}".format(currenttime.get_str()), 0, 24, WIDTH, 1)


def draw_sun(sunrise, sunset, time, cycle):
    sunlight_range = sunset - sunrise
    angle = int((180 / sunlight_range) * (time - sunrise)) % 360
    pos_x, pos_y = calc_circle_points(64 - 16, 54, 50, angle)
    display.set_pen(15)
    if angle > 180:
        gp.set_backlight(0, 0, 255)
    elif angle < 90:
        r = 255
        g = ((angle / 100) * 90)
        b = 0
        gp.set_backlight(r, g, b)
    elif angle > 90:
        r = 255
        g = 100 - (((angle - 90) / 100) * 90)
        b = 0
        gp.set_backlight(r, g, b)
    for y in range(16):
        for x in range(16):
            if sun[cycle][y] & (1 << x):
                display.pixel(x + pos_x, int((y + pos_y)))


try:
    network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
except Exception as e:
    print(f'Wifi connection failed! {e}')

get_sunrise()
ntptime.settime()
currenttime.parse_localtime(time.localtime())

count = 0  # Counter for animation
display.set_backlight(0.5)  # Dim white backlight to help colours show

while True:
    # Update current time class instance from RTC
    currenttime.parse_localtime(time.localtime())
    count += 1

    display.set_pen(0)
    display.clear()

    # Uncomment for the animation
    # draw_sun(0, 180, count % 180, count % 3)
    draw_sun(sunrise_obj.get_total_secs(), sunset_obj.get_total_secs(), currenttime.get_total_secs(), count % 3)
    draw_hills()
    draw_text()
    display.update()
    time.sleep(0.2)
