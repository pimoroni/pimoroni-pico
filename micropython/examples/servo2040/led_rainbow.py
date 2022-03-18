import time
from pimoroni import Button
from plasma import WS2812
from servo import servo2040

# Press "Boot" to exit the program.

SPEED = 5           # The speed that the LEDs will cycle at
BRIGHTNESS = 0.4    # The brightness of the LEDs
UPDATES = 50        # How many times the LEDs will be updated per second

# Create the LED bar, using PIO 1 and State Machine 0
led_bar = WS2812(servo2040.NUM_LEDS, 1, 0, servo2040.LED_DAT)

# Create the user button
user_sw = Button(servo2040.USER_SW)

# Start updating the LED bar
led_bar.start()


offset = 0.0

# Make rainbows until the user button is pressed
while user_sw.raw() is not True:

    offset += SPEED / 1000.0

    # Update all the LEDs
    for i in range(servo2040.NUM_LEDS):
        hue = float(i) / servo2040.NUM_LEDS
        led_bar.set_hsv(i, hue + offset, 1.0, BRIGHTNESS)

    time.sleep(1.0 / UPDATES)

# Turn off the LED bar
led_bar.clear()
