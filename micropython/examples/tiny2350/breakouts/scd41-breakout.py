import time
from pimoroni import RGBLED
from pimoroni_i2c import PimoroniI2C
from breakout_scd41 import BreakoutSCD41

# SDA and SCL pins for the Tiny 2350 QW/ST connector
PINS_TINY_2350 = {"sda": 12, "scl": 13}

# Setup an instance of I2C and SCD41
i2c = PimoroniI2C(**PINS_TINY_2350)
scd41 = BreakoutSCD41(i2c)

# Setup the RGB Led and then turn it off
led = RGBLED(18, 19, 20)
led.set_rgb(0, 0, 0)

# Start the SCD41 sensor
scd41.start()


while True:

    # Grab the readings if the sensor is ready.
    if scd41.ready():
        co2, temperature, humidity = scd41.measure()

        # Print the latest CO2 measurement, you'll be able to see this in Thonny's shell
        print(co2)

        # Change the onboard LED based on the current CO2 levels

        if co2 < 1000:
            led.set_rgb(0, 255, 0)
        elif co2 >= 1000 and co2 <= 2000:
            led.set_rgb(255, 165, 0)
        elif co2 > 2000:
            led.set_rgb(255, 0, 0)

    # Now we take a little nap before the next reading ZzzzZz
    time.sleep(3)
