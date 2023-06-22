import breakout_scd41
from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS  # or PICO_EXPLORER_I2C_PINS or HEADER_I2C_PINS
import time

i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)  # or PICO_EXPLORER_I2C_PINS or HEADER_I2C_PINS

breakout_scd41.init(i2c)
breakout_scd41.start()

while True:
    if breakout_scd41.ready():
        co2, temperature, humidity = breakout_scd41.measure()
        print(co2, temperature, humidity)
        time.sleep(1.0)
