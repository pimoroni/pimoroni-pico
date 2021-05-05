import time
from breakout_ltr559 import BreakoutLTR559

ltr = BreakoutLTR559()

x = 0
y = 0
light = True

part_id = ltr.part_id()
print("Found LTR559. Part ID: 0x", '{:02x}'.format(part_id), sep="")

while True:
    reading = ltr.get_reading()
    if reading is not None:
        print("Lux:", reading[BreakoutLTR559.LUX], "Prox:", reading[BreakoutLTR559.PROXIMITY])

    time.sleep(0.1)
