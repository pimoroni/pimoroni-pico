import time

import pimoroni_i2c
import breakout_scd41

i2c = pimoroni_i2c.PimoroniI2C(4, 5)

breakout_scd41.init(i2c)
breakout_scd41.start()

while True:
    if breakout_scd41.ready():
        co2, temperature, humidity = breakout_scd41.measure()
        print(co2, temperature, humidity)
        time.sleep(1.0)
