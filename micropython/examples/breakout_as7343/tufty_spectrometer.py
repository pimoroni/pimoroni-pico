from picographics import PicoGraphics, DISPLAY_TUFTY_2040, PEN_P4
from breakout_as7343 import BreakoutAS7343
from pimoroni_i2c import PimoroniI2C
import time


display = PicoGraphics(DISPLAY_TUFTY_2040, pen_type=PEN_P4, rotate=90)

WIDTH, HEIGHT = display.get_bounds()

BLACK = display.create_pen(0, 0, 0)

FZ = display.create_pen(0, 0, 128)
FY = display.create_pen(0, 255, 0)
FXL = display.create_pen(255, 128, 0)
NIR = display.create_pen(128, 0, 0)
F2 = display.create_pen(128, 0, 128)
F3 = display.create_pen(0, 0, 255)
F4 = display.create_pen(0, 128, 255)
F6 = display.create_pen(128, 64, 0)

F1 = display.create_pen(196, 0, 196)
F5 = display.create_pen(64, 255, 64)
F7 = display.create_pen(255, 0, 0)
F8 = display.create_pen(196, 0, 0)

WHITE = display.create_pen(255, 255, 255)

i2c = PimoroniI2C(sda=4, scl=5)
as7343 = BreakoutAS7343(i2c)

as7343.set_channels(18)
as7343.set_gain(1024)
as7343.set_measurement_time(33) # Roughly 30fps at 16ms/measurement
as7343.set_integration_time(27800)

as7343.set_illumination_current(4)
as7343.set_illumination_led(True)


BAR_WIDTH = 18
BAR_SPACING = 4
MARGIN = display.measure_text("NIR") + 2
BAR_HEIGHT = WIDTH - MARGIN

OFFSET_LEFT = int((HEIGHT - ((BAR_WIDTH + BAR_SPACING) * 12)) / 2)

# Satrting max values for auto-ranging
# From figure 8 of the datasheet
MAX_VALUES = [
    2711,
    4684,
    5970,
    13226,
    2371,
    962,
    3926,
    4170,
    7760,
    1967,
    6774,
    1166
]

LABELS = [
    "FZ",
    "FY",
    "FXL",
    "NIR",
    "F2",
    "F3",
    "F4",
    "F6",
    "F1",
    "F5",
    "F7",
    "F8"
]


while True:
    display.set_pen(0)
    display.clear()
    readings = as7343.read()
    for i, reading in enumerate(readings):
        MAX_VALUES[i] = max(reading, MAX_VALUES[i])
        scaled = int(reading / MAX_VALUES[i] * BAR_HEIGHT)

        y = i * (BAR_WIDTH + BAR_SPACING)
        y += OFFSET_LEFT

        display.set_pen(i + 1)
        display.rectangle(MARGIN, y, scaled, BAR_WIDTH)

        display.set_pen(WHITE)
        display.text(LABELS[i], 0, y + 1)

    display.update()
