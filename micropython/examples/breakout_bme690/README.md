# BME69X <!-- omit in toc -->

- [Getting Started](#getting-started)
- [Reading Data From The Sensor](#reading-data-from-the-sensor)
- [Configuring The Sensor](#configuring-the-sensor)
  - [Filter Settings](#filter-settings)
  - [Oversampling Settings](#oversampling-settings)
  - [Mode Settings](#mode-settings)
  - [Standby/Output Data Rate Settings](#standbyoutput-data-rate-settings)
  - [Defaults](#defaults)

## Getting Started

Construct new `I2C` and `BreakoutBME69X` instances:

```python
from breakout_bme69x import BreakoutBME69X

i2c = machine.I2C()
bme = BreakoutBME69X(i2c)
```

The default I2C address is `0x76`. If you've cut the trace to change the I2C address of the sensor, you can specify the alternate I2C address like this:

``` python
bme = BreakoutBME69X(i2c, 0x77)
```

The `breakout_bme69x` module also includes constants for the possible I2C addresses:

* `I2C_ADDRESS_DEFAULT`
* `I2C_ADDRESS_ALT`

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

The `breakout_bme69x` module includes constants for these:

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
