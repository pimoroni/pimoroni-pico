# This example grabs current weather details from Open Meteo and displays them on Badger 2040 W.
# Find out more about the Open Meteo API at https://open-meteo.com

import badger2040w as badger2040
from badger2040w import WIDTH
import urequests
import jpegdec

# Set your latitude/longitude here (find yours by right clicking in Google Maps!)
LAT = 53.38609085276884
LNG = -1.4239983439328177
TIMEZONE = "auto"  # determines time zone from lat/long

URL = "http://api.open-meteo.com/v1/forecast?latitude=" + str(LAT) + "&longitude=" + str(LNG) + "&current_weather=true&timezone=" + TIMEZONE

# Display Setup
display = badger2040.Badger2040W()
display.led(128)
display.set_update_speed(2)

jpeg = jpegdec.JPEG(display.display)

# Connects to the wireless network. Ensure you have entered your details in WIFI_CONFIG.py :).
display.connect()


def get_data():
    global weathercode, temperature, windspeed, winddirection, date, time
    print(f"Requesting URL: {URL}")
    r = urequests.get(URL)
    # open the json data
    j = r.json()
    print("Data obtained!")
    print(j)

    # parse relevant data from JSON
    current = j["current_weather"]
    temperature = current["temperature"]
    windspeed = current["windspeed"]
    winddirection = calculate_bearing(current["winddirection"])
    weathercode = current["weathercode"]
    date, time = current["time"].split("T")

    r.close()


def calculate_bearing(d):
    # calculates a compass direction from the wind direction in degrees
    dirs = ['N', 'NNE', 'NE', 'ENE', 'E', 'ESE', 'SE', 'SSE', 'S', 'SSW', 'SW', 'WSW', 'W', 'WNW', 'NW', 'NNW']
    ix = round(d / (360. / len(dirs)))
    return dirs[ix % len(dirs)]


def draw_page():
    # Clear the display
    display.set_pen(15)
    display.clear()
    display.set_pen(0)

    # Draw the page header
    display.set_font("bitmap6")
    display.set_pen(0)
    display.rectangle(0, 0, WIDTH, 20)
    display.set_pen(15)
    display.text("Weather", 3, 4)
    display.set_pen(0)

    display.set_font("bitmap8")

    if temperature is not None:
        # Choose an appropriate icon based on the weather code
        # Weather codes from https://open-meteo.com/en/docs
        # Weather icons from https://fontawesome.com/
        if weathercode in [71, 73, 75, 77, 85, 86]:  # codes for snow
            jpeg.open_file("examples/icon-snow.jpg")
        elif weathercode in [51, 53, 55, 56, 57, 61, 63, 65, 66, 67, 80, 81, 82]:  # codes for rain
            jpeg.open_file("examples/icon-rain.jpg")
        elif weathercode in [1, 2, 3, 45, 48]:  # codes for cloud
            jpeg.open_file("examples/icon-cloud.jpg")
        elif weathercode in [0]:  # codes for sun
            jpeg.open_file("examples/icon-sun.jpg")
        elif weathercode in [95, 96, 99]:  # codes for storm
            jpeg.open_file("examples/icon-storm.jpg")
        jpeg.decode(13, 40, jpegdec.JPEG_SCALE_FULL)
        display.set_pen(0)
        display.text(f"Temperature: {temperature}°C", int(WIDTH / 3), 28, WIDTH - 105, 2)
        display.text(f"Wind Speed: {windspeed}kmph", int(WIDTH / 3), 48, WIDTH - 105, 2)
        display.text(f"Wind Direction: {winddirection}", int(WIDTH / 3), 68, WIDTH - 105, 2)
        display.text(f"Last update: {date}, {time}", int(WIDTH / 3), 88, WIDTH - 105, 2)

    else:
        display.set_pen(0)
        display.rectangle(0, 60, WIDTH, 25)
        display.set_pen(15)
        display.text("Unable to display weather! Check your network settings in WIFI_CONFIG.py", 5, 65, WIDTH, 1)

    display.update()


get_data()
draw_page()

# Call halt in a loop, on battery this switches off power.
# On USB, the app will exit when A+C is pressed because the launcher picks that up.
while True:
    display.halt()
