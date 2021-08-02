# BMP280 <!-- omit in toc -->

- [Getting Started](#getting-started)
- [Reading Temperature & Pressure](#reading-temperature--pressure)
- [Configuring The Sensor](#configuring-the-sensor)
  - [Filter Settings](#filter-settings)
  - [Oversampling Settings](#oversampling-settings)
  - [Mode Settings](#mode-settings)
  - [Standby/Output Data Rate Settings](#standbyoutput-data-rate-settings)
  - [Defaults](#defaults)

## Getting Started

Construct new `PimoroniI2C` and `BreakoutBMP280` instances:

```python
from breakout_bmp280 import BreakoutBMP280
from pimoroni_i2c import PimoroniI2C

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = PimoroniI2C(**PINS_BREAKOUT_GARDEN)
bmp = BreakoutBMP280(i2c)
```

## Reading Temperature & Pressure

The `read` method will return a tuple containing Temperature (degrees C) and Pressure (Pa) values:

```python
temperature, pressure = bmp.read()
```

## Configuring The Sensor

The `configure` method allows you to set up the oversampling, filtering and operation mode.

```python
bmp.configure(filter, standby_time, os_pressure, os_temp, mode)
```

The `breakout_bmp280` module includes constants for these:

### Filter Settings

* `FILTER_COEFF_OFF`
* `FILTER_COEFF_2`
* `FILTER_COEFF_4`
* `FILTER_COEFF_8`
* `FILTER_COEFF_16`

### Oversampling Settings

* `NO_OVERSAMPLING`
* `OVERSAMPLING_1X`
* `OVERSAMPLING_2X`
* `OVERSAMPLING_4X`
* `OVERSAMPLING_8X`
* `OVERSAMPLING_16X`

### Mode Settings

* `SLEEP_MODE`
* `FORCED_MODE`
* `NORMAL_MODE`

### Standby/Output Data Rate Settings

* `STANDBY_TIME_0_5_MS`
* `STANDBY_TIME_62_5_MS`
* `STANDBY_TIME_125_MS`
* `STANDBY_TIME_250_MS`
* `STANDBY_TIME_500_MS`
* `STANDBY_TIME_1000_MS`
* `STANDBY_TIME_2000_MS`
* `STANDBY_TIME_4000_MS`

### Defaults

```python
bmp.configure(FILTER_COEFF_2, STANDBY_TIME_1000_MS, OVERSAMPLING_4X, OVERSAMPLING_4X)
```
