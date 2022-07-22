import time
from machine import Pin, ADC, UART
from picographics import PicoGraphics, DISPLAY_ENVIRO_PLUS
from pimoroni import RGBLED, Button
from breakout_bme68x import BreakoutBME68X, STATUS_HEATER_STABLE
from pimoroni_i2c import PimoroniI2C
from breakout_ltr559 import BreakoutLTR559
from pms5003 import PMS5003
import umqtt.simple
import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio

"""
This example reads from all the sensors on Enviro+.
(plus the optional particulate sensor)
Posts results via MQTT.
"""

# change this to adjust temperature compensation
TEMPERATURE_OFFSET = 3

# MQTT broker settings
CLIENT_ID = "EnviroPlus"
SERVER_ADDRESS = "broker_IP_goes_here"
MQTT_USERNAME = "broker_username_goes_here"
MQTT_PASSWORD = "broker_password_goes_here"
UPDATE_INTERVAL = 60  # how often to post MQTT data, in seconds


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
led.set_rgb(255, 0, 0)

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

# sets up MQTT
mqtt_client = umqtt.simple.MQTTClient(client_id=CLIENT_ID, server=SERVER_ADDRESS, user=MQTT_USERNAME, password=MQTT_PASSWORD, keepalive=30)

# some constants we'll use for drawing
WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)
RED = display.create_pen(255, 0, 0)
GREEN = display.create_pen(0, 255, 0)

WIDTH, HEIGHT = display.get_bounds()
display.set_font("bitmap8")

# some other variables we'll use to keep track of stuff
current_time = 0
mqtt_time = 0
mqtt_success = False
e = "Wait a minute"

# connect to wifi
uasyncio.get_event_loop().run_until_complete(network_manager.client(WIFI_CONFIG.SSID, WIFI_CONFIG.PSK))

while True:

    # read BME688
    temperature, pressure, humidity, gas, status, _, _ = bme.read()
    heater = "Stable" if status & STATUS_HEATER_STABLE else "Unstable"

    # correct temperature and humidity using an offset
    corrected_temperature = temperature - TEMPERATURE_OFFSET
    dewpoint = temperature - ((100 - humidity) / 5)
    corrected_humidity = 100 - (5 * (corrected_temperature - dewpoint))

    # read LTR559
    ltr_reading = ltr.get_reading()
    lux = ltr_reading[BreakoutLTR559.LUX]
    prox = ltr_reading[BreakoutLTR559.PROXIMITY]

    # read mic
    mic_reading = mic.read_u16()

    # read particle sensor
    particulate_reading = pms5003.read()

    if heater == "Stable" and ltr_reading is not None:
        led.set_rgb(0, 0, 0)
        current_time = time.ticks_ms()
        if (current_time - mqtt_time) / 1000 >= UPDATE_INTERVAL:
            # then do an MQTT
            try:
                mqtt_client.connect()
                mqtt_client.publish(topic="EnviroTemperature", msg=str(corrected_temperature))
                mqtt_client.publish(topic="EnviroHumidity", msg=str(corrected_humidity))
                mqtt_client.publish(topic="EnviroPressure", msg=str(pressure / 100))
                mqtt_client.publish(topic="EnviroGas", msg=str(gas))
                mqtt_client.publish(topic="EnviroLux", msg=str(lux))
                mqtt_client.publish(topic="EnviroMic", msg=str(mic_reading))
                mqtt_client.publish(topic="EnviroParticulates1_0", msg=str(particulate_reading.pm_ug_per_m3(1.0)))
                mqtt_client.publish(topic="EnviroParticulates2_5", msg=str(particulate_reading.pm_ug_per_m3(2.5)))
                mqtt_client.publish(topic="EnviroParticulates10", msg=str(particulate_reading.pm_ug_per_m3(10)))
                mqtt_client.disconnect()
                mqtt_success = True
                mqtt_time = time.ticks_ms()
                led.set_rgb(0, 50, 0)
            except Exception as e:
                print(e)
                mqtt_success = False
                led.set_rgb(255, 0, 0)
    else:
        # light up the LED red if there's a problem with MQTT or sensor readings
        led.set_rgb(255, 0, 0)

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
    display.text("Posting Enviro+ sensor data via MQTT", 10, 10, WIDTH, scale=3)
    if mqtt_success is True:
        current_time = time.ticks_ms()
        display.set_pen(GREEN)
        display.text(f"Last MQTTed {(current_time - mqtt_time) / 1000:.0f} seconds ago", 10, 130, WIDTH, scale=3)
    else:
        display.set_pen(RED)
        display.text(e, 10, 130, WIDTH, scale=3)
    display.update()

    time.sleep(1.0)
