# SCD41 CO2 Sensor Driver <!-- omit in toc -->

## Getting Started

Construct a new PimoroniI2C instance for your specific board. Breakout Garden uses pins 4 & 5 and Pico Explorer uses pins 20 & 21.

Since SCD41 has a fixed I2C address and the Sensirion SCD4x library is used under the hood, it's wrapped up as a module for Python.

Import the `breakout_scd41` and call `init` to set up I2C:

```python
import time

import pimoroni_i2c
import breakout_scd41

i2c = pimoroni_i2c.PimoroniI2C(4, 5)

breakout_scd41.init(i2c)
```

## Taking Measurements

Before taking a measurement you must start periodic measurement by calling `start()`.

Poll on `ready()` and use `measure()` to read the result when it's `True`:

```python
breakout_scd41.start()

while True:
    if breakout_scd41.ready():
        co2, temperature, humidity = breakout_scd41.measure()
        print(co2, temperature, humidity)
        time.sleep(1.0)
```

The `measure()` method will return a Tuple containing the CO2 reading, temperature in degrees C and humidity.

