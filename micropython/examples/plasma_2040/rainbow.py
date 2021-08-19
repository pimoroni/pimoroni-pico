import plasma
from plasma import plasma2040
import time

# Import helpers for RGB LEDs, Buttons, and Analog
from pimoroni import RGBLED, Button, Analog

# Press "B" to speed up the LED cycling effect.
# Press "A" to slow it down again.
# Press "Boot" to reset the speed back to default.

# Set how many LEDs you have
NUM_LEDS = 30

# The speed that the LEDs will start cycling at
DEFAULT_SPEED = 10

# How many times the LEDs will be updated per second
UPDATES = 60


# Pick *one* LED type by uncommenting the relevant line below:

# APA102 / DotStar™ LEDs
# led_strip = plasma.APA102(NUM_LEDS, 0, 0, plasma2040.DAT, plasma2040.CLK)

# WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma2040.DAT)

user_sw = Button(plasma2040.USER_SW)
button_a = Button(plasma2040.BUTTON_A)
button_b = Button(plasma2040.BUTTON_B)
led = RGBLED(plasma2040.LED_R, plasma2040.LED_G, plasma2040.LED_B)
sense = Analog(plasma2040.CURRENT_SENSE, plasma2040.ADC_GAIN, plasma2040.SHUNT_RESISTOR)

# Start updating the LED strip
led_strip.start()

speed = DEFAULT_SPEED
offset = 0.0

count = 0
# Make rainbows
while True:
    sw = user_sw.read()
    a = button_a.read()
    b = button_b.read()

    if sw:
        speed = DEFAULT_SPEED
    else:
        if a:
            speed -= 1
        if b:
            speed += 1

    speed = min(255, max(1, speed))

    offset += float(speed) / 2000.0

    for i in range(NUM_LEDS):
        hue = float(i) / NUM_LEDS
        led_strip.set_hsv(i, hue + offset, 1.0, 1.0)

    led.set_rgb(speed, 0, 255 - speed)

    count += 1
    if count >= UPDATES:
        # Display the current value once every second
        print("Current =", sense.read_current(), "A")
        count = 0

    time.sleep(1.0 / UPDATES)
