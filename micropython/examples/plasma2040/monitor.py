import plasma
from plasma import plasma2040

# Import helpers for RGB LEDs and Buttons
from pimoroni import RGBLED, Button

# Import bme68x and I2C helper
from breakout_bme68x import BreakoutBME68X
from pimoroni_i2c import PimoroniI2C

# Uses a BME68x to monitor the ambient temperature, pressure and humidity, and show them as bars on an LED strip.
# Press "A" to cycle to the next mode.
# Press "B" to cycle to the previous mode.

# Set how many LEDs you have
NUM_LEDS = 30

# How many times the LEDs will be updated per second
UPDATES = 60

# The temperature range to show (in degrees celsius)
TEMPERATURE_C_MIN = 20.0
TEMPERATURE_C_MAX = 35.0

# The pressure range to show (in pascals)
PRESSURE_PA_MIN = 87000.0
PRESSURE_PA_MAX = 108500.0

# The humidity range to show (in percent)
HUMIDITY_MIN = 0.0
HUMIDITY_MAX = 100.0


# The start and end hues for the temperature range
TEMPERATURE_HUE_START = 0.667
TEMPERATURE_HUE_END = 1.0

# The start and end hues for the pressure range
PRESSURE_HUE_START = 0.333
PRESSURE_HUE_END = 0.0

# The start and end hues for the humidity range
HUMIDITY_HUE_START = 0.333
HUMIDITY_HUE_END = 0.667


# Pick *one* LED type by uncommenting the relevant line below:

# APA102 / DotStar™ LEDs
# led_strip = plasma.APA102(NUM_LEDS, 0, 0, plasma2040.DAT, plasma2040.CLK)

# WS2812 / NeoPixel™ LEDs
led_strip = plasma.WS2812(NUM_LEDS, 0, 0, plasma2040.DAT)


button_a = Button(plasma2040.BUTTON_A, repeat_time=0)
button_b = Button(plasma2040.BUTTON_B, repeat_time=0)
led = RGBLED(plasma2040.LED_R, plasma2040.LED_G, plasma2040.LED_B)

PINS_PLASMA2040 = {"sda": plasma2040.SDA, "scl": plasma2040.SCL}

i2c = PimoroniI2C(**PINS_PLASMA2040)
bme = BreakoutBME68X(i2c)


ALL, TEMPERATURE, PRESSURE, HUMIDITY = range(4)


# Maps a value from one range to another
def map(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min


# Sets a section of the led strip to show a hue gradient based on the provided percent
def colour_gauge(percent, start_led, end_led, start_hue, end_hue):
    if end_led > start_led:
        length = end_led - start_led
        light_pixels = percent * float(length)

        for i in range(length):
            h = map(i, 0.0, float(length - 1), start_hue, end_hue)

            i2 = i + 1
            if i2 <= light_pixels:
                led_strip.set_hsv(i + start_led, h, 1.0, 1.0)
            elif i <= light_pixels:
                scale = map(light_pixels, float(i), float(i2), 0.0, 1.0)
                led_strip.set_hsv(i + start_led, h, 1.0, scale)
            else:
                led_strip.set_hsv(i + start_led, 0.0, 0.0, 0.0)


first_third = int(NUM_LEDS / 3)
second_third = int((NUM_LEDS * 2) / 3)

mode = ALL

# Start updating the LED strip
led_strip.start()

while True:
    temperature, pressure, humidity, _, _, _, _ = bme.read()
    print("{:0.2f}c, {:0.2f}Pa, {:0.2f}%".format(
        temperature, pressure, humidity))

    if mode == ALL:
        t = map(temperature, TEMPERATURE_C_MIN, TEMPERATURE_C_MAX, 0.0, 1.0)
        colour_gauge(t, 0, first_third, TEMPERATURE_HUE_START, TEMPERATURE_HUE_END)

        t = map(pressure, PRESSURE_PA_MIN, PRESSURE_PA_MAX, 0.0, 1.0)
        colour_gauge(t, first_third, second_third, PRESSURE_HUE_START, PRESSURE_HUE_END)

        t = map(humidity, HUMIDITY_MIN, HUMIDITY_MAX, 0.0, 1.0)
        colour_gauge(t, second_third, NUM_LEDS, HUMIDITY_HUE_START, HUMIDITY_HUE_END)

    elif mode == TEMPERATURE:
        t = map(temperature, TEMPERATURE_C_MIN, TEMPERATURE_C_MAX, 0.0, 1.0)
        colour_gauge(t, 0, NUM_LEDS, TEMPERATURE_HUE_START, TEMPERATURE_HUE_END)

    elif mode == PRESSURE:
        t = map(pressure, PRESSURE_PA_MIN, PRESSURE_PA_MAX, 0.0, 1.0)
        colour_gauge(t, 0, NUM_LEDS, PRESSURE_HUE_START, PRESSURE_HUE_END)

    elif mode == HUMIDITY:
        t = map(humidity, HUMIDITY_MIN, HUMIDITY_MAX, 0.0, 1.0)
        colour_gauge(t, 0, NUM_LEDS, HUMIDITY_HUE_START, HUMIDITY_HUE_END)

    a_pressed = button_a.read()
    b_pressed = button_b.read()

    if mode == ALL:
        led.set_rgb(127, 127, 127)
        if a_pressed:
            mode = TEMPERATURE
        elif b_pressed:
            mode = HUMIDITY

    elif mode == TEMPERATURE:
        led.set_rgb(255, 0, 255)
        if a_pressed:
            mode = PRESSURE
        elif b_pressed:
            mode = ALL

    elif mode == PRESSURE:
        led.set_rgb(255, 255, 0)
        if a_pressed:
            mode = HUMIDITY
        elif b_pressed:
            mode = TEMPERATURE

    elif mode == HUMIDITY:
        led.set_rgb(0, 255, 255)
        if a_pressed:
            mode = ALL
        elif b_pressed:
            mode = PRESSURE
