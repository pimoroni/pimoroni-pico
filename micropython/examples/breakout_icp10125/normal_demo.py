import time

import breakout_icp10125
import pimoroni_i2c

i2c = pimoroni_i2c.PimoroniI2C(4, 5)

icp10125 = breakout_icp10125.BreakoutICP10125(i2c)

while True:
    t, p, status = icp10125.measure(icp10125.NORMAL)
    if status == icp10125.STATUS_OK:
        print(t, p)
    time.sleep(1.0)
