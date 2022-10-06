import time
from machine import Pin, ADC, UART
from picographics import PicoGraphics, DISPLAY_ENVIRO_PLUS
from pimoroni import RGBLED, Button
from breakout_bme68x import BreakoutBME68X, STATUS_HEATER_STABLE
from pimoroni_i2c import PimoroniI2C
from breakout_ltr559 import BreakoutLTR559
from pms5003 import PMS5003
import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
import urequests
import math

"""
This example reads from all the sensors on Enviro+.
(plus the optional particulate sensor)
Posts results into adafruit.io using HTTP requests.
"""

# change this to adjust temperature compensation
TEMPERATURE_OFFSET = 3

# Adafruit.io settings
AIO_USERNAME = "username_goes_here"
AIO_KEY = "key_goes_here"
DEVICE_NICKNAME = "enviroplus"  # friendly name for your device
GROUP = "default"  # if you change this you'll need to create this group in adafruit.io yourself
UPDATE_INTERVAL = 5  # how often to post data, in minutes


def status_handler(mode, status, ip):
    display.set_pen(BLACK)
    display.clear()
    display.set_pen(WHITE)
    display.text("Network: {}".format(WIFI_CONFIG.SSID), 10, 10, scale=2)
    status_text = "Connecting..."
    if status is not None:
        if status:
            status_text = "Connection successful!"
        else:
            status_text = "Connection failed!"

    display.text(status_text, 10, 30, scale=2)
    display.text("IP: {}".format(ip), 10, 60, scale=2)
    display.update()


# set up wifi
network_manager = NetworkManager(WIFI_CONFIG.COUNTRY, status_handler=status_handler)

# set up the display
display = PicoGraphics(display=DISPLAY_ENVIRO_PLUS)
display.set_backlight(1.0)

# set up the LED
led = RGBLED(6, 7, 10, invert=True)
led.set_rgb(255, 255, 0)

# set up the buttons
button_a = Button(12, invert=True)
button_b = Button(13, invert=True)

# set up the Pico W's I2C
PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)

# set up BME688 and LTR559 sensors
bme = BreakoutBME68X(i2c, address=0x77)
ltr = BreakoutLTR559(i2c)

# set up analog channel for microphone
mic = ADC(Pin(26))

# configure the PMS5003 for Enviro+
pms5003 = PMS5003(
    uart=UART(1, tx=Pin(8), rx=Pin(9), baudrate=9600),
    pin_enable=Pin(3),
    pin_reset=Pin(2),
    mode="active"
)

# some constants we'll use for drawing
WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)
RED = display.create_pen(255, 0, 0)
GREEN = display.create_pen(0, 255, 0)

WIDTH, HEIGHT = display.get_bounds()
display.set_font("bitmap8")

# some other variables we'll use to keep track of stuff
current_time = 0
send_time = -60 * 1000 * UPDATE_INTERVAL  # to send on start up, initial send_time needs to be in the past
send_success = False

# connect to wifi
uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))

while True:
    # read BME688
    temperature, pressure, humidity, gas, status, _, _ = bme.read()
    heater = "Stable" if status & STATUS_HEATER_STABLE else "Unstable"
    gas_resistance = round(gas)
    # an approximate air quality calculation that accounts for the effect of
    # humidity on the gas sensor
    # https://forums.pimoroni.com/t/bme680-observed-gas-ohms-readings/6608/25
    aqi = round(math.log(gas_resistance) + 0.04 * humidity, 1)

    # correct temperature and humidity using an offset
    corrected_temperature = temperature - TEMPERATURE_OFFSET
    dewpoint = temperature - ((100 - humidity) / 5)
    corrected_humidity = 100 - (5 * (corrected_temperature - dewpoint))

    # read LTR559
    ltr_reading = ltr.get_reading()
    lux = ltr_reading[BreakoutLTR559.LUX]

    # read mic
    sample_time_ms = 500
    start = time.ticks_ms()
    min_value = 1.65
    max_value = 1.65
    while time.ticks_ms() - start < sample_time_ms:
        value = mic.read_u16() / (3.3 * 65535)
        min_value = min(min_value, value)
        max_value = max(max_value, value)

    noise_vpp = round((max_value - min_value), 3)

    # read particle sensor
    particulate_reading = pms5003.read()

    # format our data for adafruit.io
    # can only send 9 datapoints at once for free accounts?
    headers = {'X-AIO-Key': AIO_KEY, 'Content-Type': 'application/json'}
    url = f"http://io.adafruit.com/api/v2/{AIO_USERNAME}/groups/{GROUP}/data"
    payload = {
        "feeds": [
            {"key": f"{DEVICE_NICKNAME}-temperature", "value": corrected_temperature},
            {"key": f"{DEVICE_NICKNAME}-humidity", "value": corrected_humidity},
            {"key": f"{DEVICE_NICKNAME}-pressure", "value": pressure/100},
            {"key": f"{DEVICE_NICKNAME}-luminance", "value": lux},
            {"key": f"{DEVICE_NICKNAME}-aqi", "value": aqi},
            {"key": f"{DEVICE_NICKNAME}-noise", "value": noise_vpp},
            {"key": f"{DEVICE_NICKNAME}-pm1", "value": particulate_reading.pm_ug_per_m3(1.0)},
            {"key": f"{DEVICE_NICKNAME}-pm2-5", "value": particulate_reading.pm_ug_per_m3(2.5)},
            {"key": f"{DEVICE_NICKNAME}-pm10", "value": particulate_reading.pm_ug_per_m3(10)}
            ]
        }

    # if the sensors are ready
    if heater == "Stable" and ltr_reading is not None:
        led.set_rgb(0, 0, 0)
        # and if there's been a long enough interval
        current_time = time.ticks_ms()
        if (current_time - send_time) / 1000 >= UPDATE_INTERVAL * 60:
            # then try to send data
            try:
                result = urequests.post(url, json=payload, headers=headers)
                result.close()
                if result.status_code == 200:  # successful send!
                    send_success = True
                    send_time = time.ticks_ms()
                    led.set_rgb(0, 50, 0)
                else:
                    # attempt to handle send errors somewhat gracefully
                    print(f"Send error: {result.status_code} {result.reason}")
                    send_success = False
                    led.set_rgb(255, 0, 0)
                    time.sleep(30)  # back off for 30 secs if the send fails
            except Exception as e:
                print(f"Error: {e}")

    # turn off the backlight with A and turn it back on with B
    # things run a bit hotter when screen is on, so we're applying a different temperature offset
    if button_a.is_pressed:
        display.set_backlight(1.0)
        TEMPERATURE_OFFSET = 5
        time.sleep(0.5)
    elif button_b.is_pressed:
        display.set_backlight(0)
        TEMPERATURE_OFFSET = 3
        time.sleep(0.5)

    # draw some stuff on the screen
    display.set_pen(BLACK)
    display.clear()
    display.set_pen(WHITE)
    display.text("POSTing Enviro+ sensor data to adafruit.io", 10, 10, WIDTH, scale=3)
    if send_success is True:
        current_time = time.ticks_ms()
        display.set_pen(GREEN)
        display.text(f"Last sent {(current_time - send_time) / 1000:.0f} seconds ago", 10, 130, WIDTH, scale=3)
    display.update()

    time.sleep(1.0)
