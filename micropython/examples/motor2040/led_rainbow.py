import time
from pimoroni import Button
from plasma import WS2812
from motor import motor2040

"""
Displays a rotating rainbow pattern on the Motor 2040's onboard LED.

Press "Boot" to exit the program.

NOTE: Plasma WS2812 uses the RP2040's PIO system, and as
such may have problems when running code multiple times.
If you encounter issues, try resetting your board.
"""

SPEED = 5           # The speed that the LED will cycle at
BRIGHTNESS = 0.4    # The brightness of the LED
UPDATES = 50        # How many times the LED will be updated per second

# Create the LED, using PIO 1 and State Machine 0
led = WS2812(motor2040.NUM_LEDS, 1, 0, motor2040.LED_DATA)

# Create the user button
user_sw = Button(motor2040.USER_SW)

# Start updating the LED
led.start()


hue = 0.0

# Make rainbows until the user button is pressed
while not user_sw.raw():

    hue += SPEED / 1000.0

    # Update the LED
    led.set_hsv(0, hue, 1.0, BRIGHTNESS)

    time.sleep(1.0 / UPDATES)

# Turn off the LED
led.clear()
