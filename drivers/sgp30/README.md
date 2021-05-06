# The SGP30 Environmental Sensor

The SGP30 environmental sensor breakout gives CO2 (eCO2) and Volatile Compound (TVOC) readings.

## Code description

This is a C++ library which creates a class to manage the SGP30 breakout. In the calls below, the functions returning a `bool` will return `true` if the request is successful, `false` if something went wrong.

## Instantiation

Create an SGP30 instance with default I2C connection parameters with:

```c++
sgp30 = SGP30();
```

To specify the i2c channel (`i2c0` or `i2c1` on a Pico), the SDA and SCL GPIO pins:

```c++
sgp30 = SGP30(i2c0, 20, 21);
```

## Initialisation

Before using the SGP30, you must initialise it. This sets up the GPIO pins, resets the device, retrieves the unique ID for the chip, and verifies that the required features are supported by the chip.

```c++
bool sgp30.init()
```

## Soft Reset

This clears the chip ready for measurement, It is done as part of the `init()` function, but can be called later if desired. After calling `soft_reset()`, you must issue a new `start_measurement()` call.

```c++
bool sgp30.soft_reset()
```

## Get the chip's Unique ID

This returns the unique ID that was retrieved during initialistion, as a 48-bit value (in a 64-bit integer). You can print this out (in hex) by using the printf format "%012llx".

```c++
uint64_t sgp30.get_unique_id()
```

## Measure air quality

To start the measurement process, call this.
The chip then runs a dynamic baseline compensation algorithm for up to 15 seconds to improve the accuracy of readings.
You call `get_air_quality()` to get the current eCO2 and TVOC values.
While the algortihm is running, (i.e. for those first 15 seconds) it returns default values (eCO2=400, TVOC=0).
The `wait_for_setup` parameter is `false` if you want to return as soon as the command is given - you should then write code to issue `get_air_quality()` once per second for up to 15 seconds.
If true, the function itself issues `get_air_quality()` for up to 15 seconds, until values other than 400 and 0 are returned.

```c++
bool sgp30.start_measurement(bool wait_for_setup)
```

To retrieve the current readings, pass in the address of two 16-bit unsigned integer variables.
remember, these values will be 400 and 0 respectively while the chip is establishing its baseline.

```c++
bool sgp30.get_air_quality(uint16_t * eCO2, uint16_t * TVOC)
```

The above values are modified to make them more accurate, based on chip performance. To get the raw values that the above values are based on, you can use this (though it is really only there for testing the chip)

```c++
bool sgp30.get_air_quality_raw(uint16_t * rawH2, uint16_t * rawEthanol)
```

## Manage baseline readings

The initial initialisation process sets baseline values. To allow the baseline to be reached more quickly on subsequent start-ups, you can copy out the current baseline values and store them safely. Later, after a reset or restart, you can write those baseline values back.

```c++
bool sgp30.get_baseline(uint16_t *eco2, uint16_t *tvoc)
void sgp30.set_baseline(uint16_t eco2, uint16_t tvoc)
```

## Manage humidity

The CO2 and Volatile Compounds values can be affected by the humidity around the chip. To allow for this, the absolute humidy (for example, as returned by the SHTXX chip series) can be reported to the chip. To reset the humidity to the default, use 0 (zero) as the parameter.

```c++
bool sgp30.set_humidity(uint32_t absolute_humidity)
```

## Private functions

The following functions are used within the SGP30 class to retrieve information from and send commands to the chip. The crc calculation functions generate a chip-specific CRC value from 2 8-bit or 1 16-bit values.

```c++
bool retrieve_unique_id()
bool read_reg_1_word(uint16_t reg, uint16_t delayms, uint16_t * value)
bool read_reg_2_words(uint16_t reg, uint16_t delayms, uint16_t * value1, uint16_t * value2)
bool read_reg_3_words(uint16_t reg, uint16_t delayms, uint16_t * value1, uint16_t * value2, uint16_t * value3)
bool write_reg_1_word(uint16_t reg, uint16_t delayms, uint16_t value)
bool write_reg_2_words(uint16_t reg, uint16_t delayms, uint16_t value1, uint16_t value2)
bool write_reg(uint16_t reg, uint16_t delayms)
uint8_t calculate_crc(uint16_t value)
uint8_t calculate_crc(uint8_t value1, uint8_t value2)
```

