import time

import pimoroni_i2c
import breakout_scd41

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = pimoroni_i2c.PimoroniI2C(**PINS_PICO_EXPLORER)

breakout_scd41.init(i2c)
breakout_scd41.start()

while True:
    if breakout_scd41.ready():
        co2, temperature, humidity = breakout_scd41.measure()
        print(co2, temperature, humidity)
        time.sleep(1.0)
