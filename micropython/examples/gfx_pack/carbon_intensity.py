'''
carbon_intensity.py
This example is for the Pico W with GFX Pack.
Displays near realtime information about the different types of electricity
generation for a given UK postcode area.  Uses the backlight and a bar graph
to show the current mix of generation sources, and updates itself periodically
or when button E is pressed.
Find out more about the Carbon Intensity API here:
https://carbon-intensity.github.io/api-definitions/#carbon-intensity-api-v2-0-0
'''

import time
import uasyncio
import urequests
import WIFI_CONFIG
from gfx_pack import GfxPack, SWITCH_E
from network_manager import NetworkManager


gp = GfxPack()
display = gp.display

# Replace S9 below with the first part of your postcode e.g. OX1, NG7, LS12
CARBON_INTENSITY_URL = "https://api.carbonintensity.org.uk/regional/postcode/S9"
# Time in seconds between automatic updates...
CARBON_INTENSITY_UPDATE_FREQUENCY = 60

DISPLAY_WIDTH, DISPLAY_HEIGHT = display.get_bounds()
sys_status = "STATUS"


# Utility function to clear the screen.
def clear_screen():
    display.set_pen(0)
    display.clear()
    display.set_pen(15)


# Utility function to clear a rectangular area of the screen.
# Used to update the progress bar.
def clear_rect(start_x, start_y, end_x, end_y, height):
    display.set_pen(0)
    display.line(start_x, start_y, end_x, end_y, height)
    display.set_pen(15)
    display.update()


# Utility function to set the backlight to a given rgbw colour.
def set_backlight(r, g, b, w):
    gp.set_backlight(r, g, b, w)


# Utility function to display text centred on the screen.
def display_centered(text_to_display, y_pos, scale):
    width = display.measure_text(text_to_display, scale)
    x_pos = (DISPLAY_WIDTH - width) // 2
    display.text(text_to_display, x_pos, y_pos, DISPLAY_WIDTH, scale)
    return x_pos


# Read values from the API and update the display.
def refresh_intensity_display():
    BAR_MIN_X = 50
    BAR_MAX_X = 120
    BAR_HEIGHT = 4

    clear_screen()
    set_backlight(0, 0, 0, 80)
    display_centered("UPDATING...", 25, 2)
    display.update()

    response_doc = urequests.get(CARBON_INTENSITY_URL).json()

    # Get the region name
    region_name = response_doc["data"][0]["shortname"]

    # Get the intensity index (very low, low, moderate, high, very high)
    intensity_index = response_doc["data"][0]["data"][0]["intensity"]["index"]

    # Pick out values from the generation mix data...
    # We want solar, wind, gas, nuclear, then other is 100% minus the total of those.

    solar_pct = 0
    wind_pct = 0
    nuclear_pct = 0
    gas_pct = 0

    for g in response_doc["data"][0]["data"][0]["generationmix"]:
        if g["fuel"] == "solar":
            solar_pct = g["perc"]
        elif g["fuel"] == "wind":
            wind_pct = g["perc"]
        elif g["fuel"] == "gas":
            gas_pct = g["perc"]
        elif g["fuel"] == "nuclear":
            nuclear_pct = g["perc"]

    # Forms of generation that are not solar, wind, gas, nuclear...
    others_pct = 100 - solar_pct - wind_pct - nuclear_pct - gas_pct

    # Figure out the bar lengths.
    one_percent_length = (BAR_MAX_X - BAR_MIN_X) / 100
    solar_width = round(one_percent_length * solar_pct)
    wind_width = round(one_percent_length * wind_pct)
    nuclear_width = round(one_percent_length * nuclear_pct)
    gas_width = round(one_percent_length * gas_pct)
    others_width = round(one_percent_length * others_pct)

    # Sort so that the longest bar is first.
    sorted_generators = sorted([
        (solar_width, "SOLAR"),
        (wind_width, "WIND"),
        (nuclear_width, "NUCLEAR"),
        (gas_width, "GAS"),
        (others_width, "OTHERS")
    ], reverse=True)

    clear_screen()

    # Set backlight according to carbon intensity...
    if intensity_index == "very low":
        set_backlight(0, 64, 0, 0)
    elif intensity_index == "low":
        set_backlight(128, 64, 0, 0)
    elif intensity_index == "moderate":
        set_backlight(128, 16, 0, 0)
    elif intensity_index == "high" or intensity_index == "very high":
        set_backlight(128, 0, 0, 0)

    # Centre the region name on screen.
    display_centered(region_name, 2, 1)

    # Draw the graph.
    v_pos = 10
    for g in sorted_generators:
        display.text(g[1], 5, v_pos, DISPLAY_WIDTH, 1)
        display.line(BAR_MIN_X, v_pos + 3, BAR_MIN_X + g[0], v_pos + 3, BAR_HEIGHT)
        v_pos += 10

    display.text("E: Update", 80, 50, DISPLAY_WIDTH, 1)
    display.update()


# Utility function used when connecting to wifi.
def display_status():
    display.set_pen(0)  # Set pen to white
    display.clear()
    display.set_pen(15)
    display.text(sys_status, 0, 0, DISPLAY_WIDTH, 1)
    display.update()


# Utility function used when connecting to wifi.
def status_handler(mode, status, ip):
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


try:
    # Attempt to connect to the wifi.
    network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)
    uasyncio.get_event_loop().run_until_complete(
        network_manager.client(WIFI_CONFIG.SSID,
                               WIFI_CONFIG.PSK))

    # Update the display, then count down to the next update drawing a
    # countdown bar periodically.  Update immediately if button E pressed.
    refresh_intensity_display()

    last_updated = time.ticks_ms()
    ticks_before = last_updated
    bar_width = DISPLAY_WIDTH
    BAR_UPDATE_FREQUENCY = 1000

    while True:
        time.sleep(0.01)

        if gp.switch_pressed(SWITCH_E):
            # Update immediately.
            refresh_intensity_display()
            last_updated = time.ticks_ms()
            bar_width = DISPLAY_WIDTH

        ticks_now = time.ticks_ms()

        if time.ticks_diff(ticks_now, ticks_before) > BAR_UPDATE_FREQUENCY:
            bar_width = bar_width - (DISPLAY_WIDTH // CARBON_INTENSITY_UPDATE_FREQUENCY)
            ticks_before = time.ticks_ms()

        if time.ticks_diff(ticks_now, last_updated) > CARBON_INTENSITY_UPDATE_FREQUENCY * 1000:
            # It is time for a scheduled update.
            refresh_intensity_display()
            last_updated = time.ticks_ms()
            bar_width = DISPLAY_WIDTH

        # Update the countdown to next update bar at the bottom
        # of the screen.
        clear_rect(0, 61, DISPLAY_WIDTH, 61, 2)
        display.line(0, 61, bar_width, 61, 2)
        display.update()

except Exception as e:
    # Failed to connect to wifi - check credentials in WIFI_CONFIG.py
    print(f'Wifi connection failed! {e}')
