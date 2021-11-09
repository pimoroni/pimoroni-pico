# ICP1025 High-accuracy Barometric Pressure & Temperature Sensor <!-- omit in toc -->

The ICP1025 library is intended to drive the TDK InvenSense ICP10125 temperature and pressure sensor.

- [Getting Started](#getting-started)
- [Taking A Measurement](#taking-a-measurement)
- [Measurement Types](#measurement-types)
  - [Normal](#normal)
  - [Low Power](#low-power)
  - [Low Noise](#low-noise)
  - [Ultra Low Noise](#ultra-low-noise)

## Getting Started

Construct a new PimoroniI2C instance for your specific board. Breakout Garden uses pins 4 & 5 and Pico Explorer uses pins 20 & 21.

```python
import breakout_icp10125
import pimoroni_i2c

i2c = pimoroni_i2c.PimoroniI2C(4, 5)

icp10125 = breakout_icp10125.BreakoutICP10125(i2c)
```

## Taking A Measurement

The `measure` method triggers a measurement, blocks for enough time for the measurement to complete, and returns the result as a tuple with three values:

1. Temperature (degrees C)
2. Pressure (Pa)
3. Status

If the `status` is `icp10125.STATUS_OK` then the reading is valid. Otherwise you should probably discard it or attempt another reading.

For example, the following code will continuously poll the ICP10125 for `NORMAL` readings and print them out if they are valid:

```python
import breakout_icp10125
import pimoroni_i2c

i2c = pimoroni_i2c.PimoroniI2C(4, 5)

icp10125 = breakout_icp10125.BreakoutICP10125(i2c)

while True:
    t, p, status = icp10125.measure()
    if status == icp10125.STATUS_OK:
        print(t, p)
    time.sleep(1.0)
```

## Measurement Types

The ICP1025 has eight measurement commands. Four are supported by this library since the remaining four are identical save for the order of Temperature/Pressure data readout being reversed.

Each measurement type has a fixed duration.

### Normal

Normal measurements are the default type, and take 7ms to complete offering a good balance of stability and speed.

```python
result = icp10125.measure(icp10125.NORMAL)
```

### Low Power

Low-power measurements take just 2ms and trade stability for speed/power efficiency.

```python
result = icp10125.measure(icp10125.LOW_POWER)
```

### Low Noise

Low-noise measurements take 24ms (roughly 3.5x as long as normal) and trade speed for stability.

```python
result = icp10125.measure(icp10125.LOW_NOISE)
```

### Ultra Low Noise

Ultra Low-noise measurements take 95ms

```python
result = icp10125.measure(icp10125.ULTRA_LOW_NOISE)
```
