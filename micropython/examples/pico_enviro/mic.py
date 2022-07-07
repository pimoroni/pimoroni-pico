'''
Audio Sensor Example

'''
from picographics import PicoGraphics, DISPLAY_ENVIRO_PLUS
from pimoroni import RGBLED
from machine import Pin, ADC
import time

print("""mic.py - displays a waveform on the attached screen
""")

display = PicoGraphics(display=DISPLAY_ENVIRO_PLUS)
display.set_backlight(1.0)

# Setup RGB Led
led = RGBLED(6, 7, 10, invert=True)
led.set_rgb(0, 0, 0)

# Setup background
BG = display.create_pen(0, 0, 0)
TEXT = display.create_pen(255, 255, 255)
MIC = display.create_pen(255, 0, 0)
display.set_pen(BG)
display.clear()

# Setup analogue Channel for Mic
MIC_PIN = 26
mic = ADC(Pin(26))

# array for storing
results = []

# Settings for bandwith and side
BANDWIDTH = 2000
SAMPLE_N = 240


# Drawing routines
def draw_background():
    display.set_pen(BG)
    display.clear()
    display.set_pen(TEXT)
    display.text("Sound Sensor", 5, 10, scale=3)


def draw_txt_overlay(sensor_data):
    display.set_pen(MIC)
    display.text("Peak: {0}".format(sensor_data), 5, 60, scale=3)


def draw_wave(results_array):

    result_index = 0
    for result in results_array:
        display.set_pen(MIC)
        display.pixel(result_index, int(120 + result))
        result_index += 1


def read_mic():
    return mic.read_u16()


def take_sample(frequency, length=240):
    buffer = []
    for index in range(length):
        buffer.append(read_mic())
        time.sleep(1 / frequency)
    return buffer


while True:

    results = take_sample(BANDWIDTH, SAMPLE_N)

    # Rescale for display
    for result_index in range(len(results)):
        results[result_index] = (results[result_index] - 33100) / 30
    # Display Upates
    draw_background()
    draw_wave(results)
    draw_txt_overlay(max(results))
    display.update()
    time.sleep(0.2)
