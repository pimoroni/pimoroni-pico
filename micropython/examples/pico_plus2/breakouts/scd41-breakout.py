import time
from pimoroni_i2c import PimoroniI2C
from breakout_scd41 import BreakoutSCD41

# SDA and SCL pins for the Pico Plus 2 QW/ST connector
PINS_PICO_PLUS_2 = {"sda": 4, "scl": 5}

# Setup an instance of I2C and BME68X
i2c = PimoroniI2C(**PINS_PICO_PLUS_2)
scd41 = BreakoutSCD41(i2c)

# Start the SCD41 sensor
scd41.start()


while True:

    # Grab the readings if the sensor is ready.
    if scd41.ready():
        co2, temperature, humidity = scd41.measure()

        # Print the latest measurements, you'll be able to see this in Thonny's shell.
        print("CO2: {} PPM\nTemperature: {}°C\nHumidity: {} %RH\n\n".format(co2, temperature, humidity))

    # Now we take a little nap before the next reading ZzzzZz
    time.sleep(3)
