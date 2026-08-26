# SCD41 CO2 Sensor Driver <!-- omit in toc -->

## Getting Started

Construct a new PimoroniI2C instance for your specific board. Breakout Garden uses pins 4 & 5 and Pico Explorer uses pins 20 & 21.

Import `BreakoutSCD41` and construct one with your I2C instance:

```python
import time

import pimoroni_i2c
from breakout_scd41 import BreakoutSCD41

i2c = pimoroni_i2c.PimoroniI2C(4, 5)

scd41 = BreakoutSCD41(i2c)
```

Keep a reference to the object for as long as you're using the sensor. The Sensirion
library keeps the I2C bus in a global, and the object is what keeps it alive.

## Taking Measurements

Before taking a measurement you must start periodic measurement by calling `start()`.

Poll on `ready()` and use `measure()` to read the result when it's `True`:

```python
scd41.start()

while True:
    if scd41.ready():
        co2, temperature, humidity = scd41.measure()
        print(co2, temperature, humidity)
        time.sleep(1.0)
```

The `measure()` method will return a Tuple containing the CO₂ reading, temperature in degrees C and humidity.

## Compensating For Ambient Conditions

The SCD41 self-heats, so its temperature reading runs high. Set an offset in degrees C to
compensate, and read it back:

```python
scd41.set_temperature_offset(4.0)
print(scd41.get_temperature_offset())
```

The offset applies to the temperature and humidity readings, not to CO2. Valid range is
0.0 to 175.0.

For a more accurate CO2 reading you can also supply the altitude in metres, or the ambient
pressure in hPa. Set one or the other, not both - pressure takes precedence:

```python
scd41.set_sensor_altitude(150)
scd41.set_ambient_pressure(1013)
```

## Changing Calibration

By default the SCD41 will perform automatic self calibration, which could lead to drift in readings over time.

You can stop this with `scd41.set_automatic_self_calibration(False)`.

You can then use force recalibration with a known good CO₂ PPM baseline to calibrate your sensor:

```python
correction_amount = scd41.perform_forced_recalibration(target_co2_concentration)
```

`correction_amount` is the resulting correction in CO₂ PPM.

To successfully conduct an accurate forced recalibration, the following steps must be carried out:

1. Operate the SCD4x in a periodic measurement mode for > 3 minutes in an environment with homogenous and constant CO₂ concentration. (read: don't breathe)
2. Stop periodic measurement.
3. Wait 500 ms.
4. Issue the perform_forced_recalibration command.
