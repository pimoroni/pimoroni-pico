'''
Particle Sensor Example

This example requires seperate MicroPython drivers for the PMS5003 particulate sensor.
(You can find it at https://github.com/pimoroni/pms5003-micropython )
or install from PyPi by searching for 'pms5003-micropython' in Thonny's 'Tools > Manage Packages'

'''
from picographics import PicoGraphics, DISPLAY_ENVIRO_PLUS
from pimoroni import RGBLED
from pms5003 import PMS5003
import machine
import time

print("""particle.py - Continuously print all data values.
and draw a pretty histogram on display
""")


# Configure the PMS5003 for Enviro+
pms5003 = PMS5003(
    uart=machine.UART(1, tx=machine.Pin(8), rx=machine.Pin(9), baudrate=9600),
    pin_enable=machine.Pin(3),
    pin_reset=machine.Pin(2),
    mode="active"
)

display = PicoGraphics(display=DISPLAY_ENVIRO_PLUS)
display.set_backlight(1.0)

# Setup RGB Led
led = RGBLED(6, 7, 10, invert=True)
led.set_rgb(0, 0, 0)

# Setup background
BG = display.create_pen(0, 0, 0)
TEXT = display.create_pen(255, 255, 255)
PM10 = display.create_pen(255, 0, 0)
PM25 = display.create_pen(255, 255, 0)
PM100 = display.create_pen(0, 255, 0)
PM125 = display.create_pen(255, 255, 0)
PM1000 = display.create_pen(255, 255, 0)
display.set_pen(BG)
display.clear()

# array for storing
results = []


# Drawing routines
def draw_background():
    display.set_pen(BG)
    display.clear()
    display.set_pen(TEXT)
    display.text("PMS5003 Sensor", 5, 10, scale=3)


def draw_txt_overlay(sensor_data):
    display.set_pen(PM10)
    display.text("PM1.0: {0}".format(sensor_data.pm_ug_per_m3(1.0)), 5, 60, scale=3)
    display.set_pen(PM25)
    display.text("PM2.5: {0}".format(sensor_data.pm_ug_per_m3(2.5)), 5, 80, scale=3)
    display.set_pen(PM100)
    display.text("PM10: {0}".format(sensor_data.pm_ug_per_m3(10)), 5, 100, scale=3)


def draw_hist(results_array):

    result_index = 0
    for result in results_array:
        display.set_pen(PM100)
        display.rectangle(2 * result_index, 240 - result.pm_ug_per_m3(10), 2, 240)
        display.set_pen(PM25)
        display.rectangle(2 * result_index, 240 - result.pm_ug_per_m3(2.5), 2, 240)
        display.set_pen(PM10)
        display.rectangle(2 * result_index, 240 - result.pm_ug_per_m3(1.0), 2, 240)
        result_index += 1


while True:
    draw_background()
    data = pms5003.read()
    print(data)
    results.append(data)

    if (len(results) > 120):  # Scroll the result list by removing the first value
        results.pop(0)

    draw_hist(results)
    draw_txt_overlay(data)
    display.update()
    time.sleep(0.5)
