import time
from pimoroni import RGBLED
from pimoroni_i2c import PimoroniI2C
from breakout_bme68x import BreakoutBME68X

# SDA and SCL pins for the Tiny 2350 QW/ST connector
PINS_TINY_2350 = {"sda": 12, "scl": 13}

# Setup an instance of I2C and BME68X
i2c = PimoroniI2C(**PINS_TINY_2350)
bme = BreakoutBME68X(i2c)

# Setup the RGB Led
led = RGBLED(18, 19, 20)

# Values for high (red) and low (blue) temperature. Adjust these if you like :)
HIGH = 24
LOW = 18


while True:
    # Grab the readings
    temperature, pressure, humidity, gas_resistance, status, gas_index, meas_index = bme.read()

    # Print the current temperature, you'll be able to see this in Thonny's shell
    print(temperature)

    # Set the LED based on the latest temperature reading
    if temperature >= HIGH:
        led.set_rgb(255, 0, 0)
    elif temperature <= LOW:
        led.set_rgb(0, 0, 255)
    else:
        led.set_rgb(0, 255, 0)

    # Now we take a little nap before the next reading ZzzzZz
    time.sleep(1)
