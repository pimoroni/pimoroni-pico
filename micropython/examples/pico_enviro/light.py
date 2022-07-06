import time
from picographics import PicoGraphics, DISPLAY_ENVIRO_PLUS
from pimoroni import RGBLED
from pimoroni_i2c import PimoroniI2C
from breakout_ltr559 import BreakoutLTR559

display = PicoGraphics(display=DISPLAY_ENVIRO_PLUS)
display.set_backlight(1.0)

led = RGBLED(6, 7, 10, invert=True)  # setting pins for the RGB led
led.set_rgb(0, 0, 0)


# setup background
BG = display.create_pen(0, 0, 0)
TEXT = display.create_pen(255, 255, 255)
PROX = display.create_pen(255, 0, 0)
LUX = display.create_pen(255, 255, 0)
display.set_pen(BG)
display.clear()


def draw_graph(lux_value, prox_value):
    scaled_lux = int(200 / 1600 * lux_value)
    scaled_prox = int(200 / 1600 * prox_value)
    display.set_pen(BG)
    display.clear()
    display.set_pen(LUX)
    display.rectangle(0, 240 - scaled_lux, 120, scaled_lux)
    display.text("PROX: {0}".format(prox_value), 125, 120, scale=2)
    display.set_pen(PROX)
    display.text("LUX: {0}".format(lux_value), 5, 120, scale=2)
    display.rectangle(120, 240 - scaled_prox, 120, scaled_prox)
    display.set_pen(TEXT)
    display.text("Light+Prox Sensor", 5, 10, scale=2)
    display.text("PROX: {0}".format(prox_value), 125, 120, scale=2)
    display.update()


PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
ltr = BreakoutLTR559(i2c)

part_id = ltr.part_id()
print("Found LTR559. Part ID: 0x", '{:02x}'.format(part_id), sep="")

# Draw blank graph
draw_graph(0, 0)


while True:
    reading = ltr.get_reading()
    if reading is not None:
        print("Lux:", reading[BreakoutLTR559.LUX], "Prox:", reading[BreakoutLTR559.PROXIMITY])
        draw_graph(reading[BreakoutLTR559.LUX], reading[BreakoutLTR559.PROXIMITY])
    time.sleep(0.1)
