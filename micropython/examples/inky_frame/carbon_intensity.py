"""
This example connects to the Carbon Intensity API to give you a regional
forecast of how your (UK) electricity is being generated and its carbon impact.

Carbon Intensity API only reports data from the UK National Grid.

Find out more about what the numbers mean at:
https://carbonintensity.org.uk/

Make sure to uncomment the correct size for your display!

"""

from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY    # 5.7"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"
import urequests
import inky_frame
import uasyncio
from network_manager import NetworkManager
import WIFI_CONFIG

# Set (the first half) of your UK postcode here
POSTCODE = "S9"

URL = "https://api.carbonintensity.org.uk/regional/postcode/" + str(POSTCODE)


def get_data():
    global region, forecast, index, power_list, datetime_to, datetime_from
    print(f"Requesting URL: {URL}")
    r = urequests.get(URL)
    # open the json data
    j = r.json()
    print("Data obtained!")
    print(j)

    # parse the json data
    region = j["data"][0]["shortname"]

    forecast = j["data"][0]["data"][0]["intensity"]["forecast"]
    index = j["data"][0]["data"][0]["intensity"]["index"]

    power_list = []
    for power in j["data"][0]["data"][0]["generationmix"]:
        power_list.append(power['perc'])

    datetime_to = j["data"][0]["data"][0]["to"].split("T")
    datetime_from = j["data"][0]["data"][0]["from"].split("T")

    # close the socket
    r.close()


def draw():
    global graphics
    # we're setting up our PicoGraphics buffer after we've made our RAM intensive https request
    graphics = PicoGraphics(DISPLAY)
    w, h = graphics.get_bounds()
    graphics.set_pen(inky_frame.WHITE)
    graphics.clear()

    # draw lines
    graphics.set_pen(inky_frame.BLACK)
    graphics.line(0, int((h / 100) * 0), w, int((h / 100) * 0))
    graphics.line(0, int((h / 100) * 50), w, int((h / 100) * 50))
    graphics.set_font("bitmap8")
    graphics.text('100%', w - 40, 10, scale=2)
    graphics.text('50%', w - 40, int((h / 100) * 50 + 10), scale=2)

    # draw bars
    bar_colours = [
        inky_frame.ORANGE,
        inky_frame.RED,
        inky_frame.ORANGE,
        inky_frame.RED,
        inky_frame.BLUE,
        inky_frame.ORANGE,
        inky_frame.GREEN,
        inky_frame.GREEN,
        inky_frame.GREEN
    ]
    for p in power_list:
        graphics.set_pen(bar_colours[power_list.index(p)])
        graphics.rectangle(int(power_list.index(p) * w / 9), int(h - p * (h / 100)),
                           int(w / 9), int(h / 100 * p))

    # draw labels
    graphics.set_font('sans')
    # once in white for a background
    graphics.set_pen(inky_frame.WHITE)
    labels = ['biomass', 'coal', 'imports', 'gas', 'nuclear', 'other', 'hydro', 'solar', 'wind']
    graphics.set_thickness(4)
    for label in labels:
        graphics.text(f'{label}', int((labels.index(label) * w / 9) + (w / 9) / 2), h - 10, angle=270, scale=1)
    # again in black
    graphics.set_pen(inky_frame.BLACK)
    labels = ['biomass', 'coal', 'imports', 'gas', 'nuclear', 'other', 'hydro', 'solar', 'wind']
    graphics.set_thickness(2)
    for label in labels:
        graphics.text(f'{label}', int((labels.index(label) * w / 9) + (w / 9) / 2), h - 10, angle=270, scale=1)

    # draw header
    graphics.set_thickness(3)
    graphics.set_pen(inky_frame.GREEN)
    if index in ['high', 'very high']:
        graphics.set_pen(inky_frame.RED)
    if index in ['moderate']:
        graphics.set_pen(inky_frame.ORANGE)
    graphics.set_font("sans")
    graphics.text('Carbon Intensity', 10, 35, scale=1.2, angle=0)

    # draw small text
    graphics.set_pen(inky_frame.BLACK)
    graphics.set_font("bitmap8")
    graphics.text(f'Region: {region}', int((w / 2) + 30), 10, scale=2)
    graphics.text(f'{forecast} gCO2/kWh ({index})', int((w / 2) + 30), 30, scale=2)
    graphics.text(f'{datetime_from[0]} {datetime_from[1]} to {datetime_to[1]}', int((w / 2) + 30), 50, scale=2)

    graphics.update()


def status_handler(mode, status, ip):
    print(mode, status, ip)


inky_frame.led_busy.on()
network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)

# connect to wifi
try:
    uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))
except ImportError:
    print("Add WIFI_CONFIG.py with your WiFi credentials")

get_data()
draw()

# Go to sleep if on battery power
inky_frame.turn_off()

# Or comment out the line above and uncomment this one to wake up and update every half hour
# inky_frame.sleep_for(30)

"""
Pico W RAM seems insufficient to decode a https request whilst having a PicoGraphics instance active.
If you are running off USB and want this to update periodically, you could incorporate a machine.reset()
to reset the Pico and start afresh every time.
"""
