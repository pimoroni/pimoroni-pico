import time
from breakout_bmp280 import BreakoutBMP280
from pimoroni_i2c import PimoroniI2C

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
bmp = BreakoutBMP280(i2c)

while True:
    reading = bmp.read()
    print(reading)
    time.sleep(1.0)
