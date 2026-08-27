import time
from pimoroni_i2c import PimoroniI2C
from breakout_bme68x import BreakoutBME68X

# SDA and SCL pins for the Pico Plus 2 QW/ST connector
PINS_PICO_PLUS_2 = {"sda": 4, "scl": 5}

# Setup an instance of I2C and BME68X
i2c = PimoroniI2C(**PINS_PICO_PLUS_2)
bme = BreakoutBME68X(i2c)

while True:
    # Grab the readings
    temperature, pressure, humidity, gas_resistance, status, gas_index, meas_index = bme.read()

    # Print the current temperature, pressure and humidity. You'll be able to see this in Thonny's shell
    print("Temperature: {}°C\nPressure:{} Pa\nHumidity:{} %rH\n\n".format(temperature, pressure, humidity))

    # Now we take a little nap before the next reading ZzzzZz
    time.sleep(1)
