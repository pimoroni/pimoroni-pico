# BME68X <!-- omit in toc -->

- [Getting Started](#getting-started)
- [Reading Data From The Sensor](#reading-data-from-the-sensor)
- [Configuring The Sensor](#configuring-the-sensor)
  - [Filter Settings](#filter-settings)
  - [Oversampling Settings](#oversampling-settings)
  - [Mode Settings](#mode-settings)
  - [Standby/Output Data Rate Settings](#standbyoutput-data-rate-settings)
  - [Defaults](#defaults)

## Getting Started

Construct new `PimoroniI2C` and `BreakoutBME68X` instances:

```python
from breakout_bme68x import BreakoutBME68X
from pimoroni_i2c import PimoroniI2C

PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

i2c = PimoroniI2C(**PINS_PICO_EXPLORER)
bme = BreakoutBME68X(i2c)
```

The `breakout_bme68x` module includes constants for the possible I2C addresses:

* `ADDRESS_DEFAULT`
* `ADDRESS_ALT`

## Reading Data From The Sensor

The `read` method will return a tuple containing Temperature (degrees C), Pressure (Pa), relative humidity (%rH), and gas resistance (Ω) values, plus the status code and gas/measurement indexes:

```python
temperature, pressure, humidity, gas_resistance, status, gas_index, meas_index = bme.read()
```

In all cases `gas_index` and `meas_index` will be zero, since these refer to the measurement profile used to gather the current readings. The MicroPython bindings currently only support a single measurement profile with a default of 300c for 100ms.

You can pass a custom temperature and duration into `read`:

```python
temperature, pressure, humidity, gas_resistance, status, gas_index, meas_index = bme.read(heater_temp=250, heater_duration=50)
```

## Configuring The Sensor

The `configure` method allows you to set up the oversampling, filtering and operation mode.

```python
bme.configure(filter, standby_time, os_pressure, os_temp, os_humidity)
```

The `breakout_bme68x` module includes constants for these:

### Filter Settings

* `FILTER_COEFF_OFF`
* `FILTER_COEFF_1`
* `FILTER_COEFF_3`
* `FILTER_COEFF_8`
* `FILTER_COEFF_15`
* `FILTER_COEFF_31`
* `FILTER_COEFF_63`
* `FILTER_COEFF_127`

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

* `STANDBY_TIME_0_59_MS`
* `STANDBY_TIME_62_5_MS`
* `STANDBY_TIME_125_MS`
* `STANDBY_TIME_250_MS`
* `STANDBY_TIME_500_MS`
* `STANDBY_TIME_1000_MS`
* `STANDBY_TIME_10_MS`
* `STANDBY_TIME_20_MS`

### Defaults

```python
bme.configure(FILTER_COEFF_3, STANDBY_TIME_1000_MS, OVERSAMPLING_16X, OVERSAMPLING_2X, OVERSAMPLING_1X)
```
