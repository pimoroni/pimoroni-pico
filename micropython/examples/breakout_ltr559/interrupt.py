import time
from machine import Pin
from pimoroni_i2c import PimoroniI2C
from breakout_ltr559 import BreakoutLTR559

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}
PIN_INTERRUPT = 22  # 3 for Breakout Garden

i2c = PimoroniI2C(**PINS_PICO_EXPLORER)
ltr = BreakoutLTR559(i2c, interrupt=PIN_INTERRUPT)
interrupt = Pin(PIN_INTERRUPT, Pin.IN, Pin.PULL_DOWN)

ltr.light_threshold(0, 10)  # COUNTS, NOT LUX!!!
ltr.proximity_threshold(0, 10)


def read(pin):  # noqa: ARG001
    reading = ltr.get_reading()
    if reading is not None:
        print("T: ", time.ticks_ms(), " Lux: ", reading[BreakoutLTR559.LUX], " Prox: ", reading[BreakoutLTR559.PROXIMITY])


interrupt.irq(trigger=Pin.IRQ_RISING, handler=read)

part_id = ltr.part_id()
print("Found LTR559. Part ID: 0x", "{:02x}".format(part_id), sep="")

while True:
    pass
