# This example reads the voltage from a battery connected to Tufty 2040
# and uses this reading to calculate how much charge is left in the battery.

from picographics import PicoGraphics, DISPLAY_TUFTY_2040
from machine import ADC, Pin
import time

display = PicoGraphics(display=DISPLAY_TUFTY_2040)

display.set_backlight(0.8)

# set up the ADCs for measuring battery voltage
vbat_adc = ADC(29)
vref_adc = ADC(28)
vref_en = Pin(27)
vref_en.init(Pin.OUT)
vref_en.value(0)
usb_power = Pin(24, Pin.IN)         # reading GP24 tells us whether or not USB power is connected

# Reference voltages for a full/empty battery, in volts
# the values could vary by battery size/manufacturer so you might need to adjust them
# Values for a Galleon 400mAh LiPo:
full_battery = 3.7
empty_battery = 2.5

# set up some colours to draw with
WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)
GREY = display.create_pen(190, 190, 190)
GREEN = display.create_pen(0, 255, 0)
RED = display.create_pen(255, 0, 0)

display.set_font("bitmap8")

while True:
    # The voltage reference on Tufty means we can measure battery voltage precisely, even when batteries are low.
    # Enable the onboard voltage reference
    vref_en.value(1)

    # Calculate the logic supply voltage, as will be lower that the usual 3.3V when running off low batteries
    vdd = 1.24 * (65535 / vref_adc.read_u16())
    vbat = (
        (vbat_adc.read_u16() / 65535) * 3 * vdd
    )  # 3 in this is a gain, not rounding of 3.3V

    # Disable the onboard voltage reference
    vref_en.value(0)

    # Print out the voltage
    print("Battery Voltage = ", vbat, "V", sep="")

    # convert the raw ADC read into a voltage, and then a percentage
    percentage = 100 * ((vbat - empty_battery) / (full_battery - empty_battery))
    if percentage > 100:
        percentage = 100
    if percentage < 0:
        percentage = 0

    # draw the battery outline
    display.set_pen(BLACK)
    display.clear()
    display.set_pen(GREY)
    display.rectangle(0, 0, 220, 135)
    display.rectangle(220, 40, 20, 55)
    display.set_pen(WHITE)
    display.rectangle(3, 3, 214, 129)

    # draw a green box for the battery level
    display.set_pen(GREEN)
    display.rectangle(5, 5, int((210 / 100) * percentage), 125)

    # add text
    display.set_pen(RED)
    if usb_power.value() == 1:         # if it's plugged into USB power...
        display.text("USB power!", 15, 90, 240, 4)

    display.text('{:.2f}'.format(vbat) + "v", 15, 10, 240, 5)
    display.text('{:.0f}%'.format(percentage), 15, 50, 240, 5)

    display.update()
    time.sleep(0.5)
