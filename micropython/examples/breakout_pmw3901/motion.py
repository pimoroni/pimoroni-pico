import time

# Pick *one* sensor type by uncommenting the relevant line below:

# PMW3901
from breakout_pmw3901 import BreakoutPMW3901 as FlowSensor

# PAA5100
# from breakout_paa5100 import BreakoutPAA5100 as FlowSensor

flo = FlowSensor()
flo.set_rotation(FlowSensor.DEGREES_0)

tx = 0
ty = 0
x = 0
y = 0

while True:
    delta = flo.get_motion()
    if delta is not None:
        x = delta[0]
        y = delta[1]
        tx += x
        ty += y
        print("Relative: x {}, y {} | Absolute: tx {}, ty {}".format(x, y, tx, ty))
    time.sleep(0.1)
