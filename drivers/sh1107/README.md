# SH1107 Display Driver for Pimoroni Mono OLEDs <!-- omit in toc -->

The SH1107 driver supports I2C mono OLED displays and is intended for use with:

* 1.12" Mono OLED Breakout (128x128)

## Setup

Construct an instance of the SH1107 driver with a width, height and I2C bus:

```c++
I2C i2c(4, 5);

SH1107 sh1107(128, 128, i2c);
```

The default I2C address is `0x3c`. Boards strapped to the alternate address take it as a fourth argument:

```c++
SH1107 sh1107(128, 128, i2c, SH1107::ALTERNATE_I2C_ADDRESS);
```

## Reference

### Update

SH1107's `update` accepts an instance of `PicoGraphics`. The display is 1 bit per pixel, so use `PicoGraphics_Pen1Bit`:

```c++
PicoGraphics_Pen1Bit graphics(sh1107.width, sh1107.height, nullptr);

sh1107.update(&graphics);
```

### I2C Addresses

| Constant                        | Address |
| ------------------------------- | ------- |
| `SH1107::DEFAULT_I2C_ADDRESS`   | `0x3c`  |
| `SH1107::ALTERNATE_I2C_ADDRESS` | `0x3d`  |
