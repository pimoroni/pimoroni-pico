import time
from pimoroni_i2c import PimoroniI2C
from breakout_ltr559 import BreakoutLTR559

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
ltr = BreakoutLTR559(i2c)

part_id = ltr.part_id()
print("Found LTR559. Part ID: 0x", '{:02x}'.format(part_id), sep="")

while True:
    reading = ltr.get_reading()
    if reading is not None:
        print("Lux:", reading[BreakoutLTR559.LUX], "Prox:", reading[BreakoutLTR559.PROXIMITY])

    time.sleep(0.1)
