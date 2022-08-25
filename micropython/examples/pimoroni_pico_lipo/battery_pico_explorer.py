# This example reads the voltage from a LiPo battery connected to Pimoroni Pico LiPo
# and uses this reading to calculate how much charge is left in the battery.
# It then displays the info on the screen of Pico Explorer.
# With Pimoroni Pico LiPo, you can read the battery percentage while it's charging.
# Save this code as main.py on your Pico if you want it to run automatically!

from machine import ADC, Pin
import time
from pimoroni import Buzzer
from picographics import PicoGraphics, DISPLAY_PICO_EXPLORER

display = PicoGraphics(display=DISPLAY_PICO_EXPLORER)

buzzer = Buzzer(0)

vsys = ADC(29)                      # reads the system input voltage
charging = Pin(24, Pin.IN)          # reading GP24 tells us whether or not USB power is connected
conversion_factor = 3 * 3.3 / 65535

full_battery = 4.2                  # reference voltages for a full/empty battery, in volts
empty_battery = 2.8                 # the values could vary by battery size/manufacturer so you might need to adjust them

# Create some pen colours for drawing with
BLACK = display.create_pen(0, 0, 0)
GREY = display.create_pen(190, 190, 190)
GREEN = display.create_pen(0, 255, 0)
RED = display.create_pen(255, 0, 0)

while True:
    buzzer.set_tone(0)

    # convert the raw ADC read into a voltage, and then a percentage
    voltage = vsys.read_u16() * conversion_factor
    percentage = 100 * ((voltage - empty_battery) / (full_battery - empty_battery))
    if percentage > 100:
        percentage = 100

    # draw the battery outline
    display.set_pen(BLACK)
    display.clear()
    display.set_pen(GREY)
    display.rectangle(0, 0, 220, 135)
    display.rectangle(220, 40, 20, 55)
    display.set_pen(BLACK)
    display.rectangle(3, 3, 214, 129)

    # draw a green box for the battery level
    display.set_pen(GREEN)
    display.rectangle(5, 5, int((210 / 100) * percentage), 125)

    # add text
    display.set_pen(RED)
    if charging.value() == 1:         # if it's plugged into USB power...
        display.text("Charging!", 15, 90, 240, 4)

    display.text('{:.2f}'.format(voltage) + "v", 15, 10, 240, 5)
    display.text('{:.0f}%'.format(percentage), 15, 50, 240, 5)

    # Low battery alarm! (you'll need to have GP0 and AUDIO connected with a jumper wire)
    if percentage < 5:              # if the battery is less than 5%
        buzzer.set_tone(262)        # then make an annoying noise

    display.update()
    time.sleep(0.5)
