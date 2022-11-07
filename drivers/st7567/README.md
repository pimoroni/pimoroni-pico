# ST7567 Display Driver for Pimoroni LCDs <!-- omit in toc -->

The ST7567 driver supports Serial (SPI) ST7567 displays and is intended for use with:

* Pico GFX Pack

## Setup

Construct an instance of the ST7567 driver with SPI pins.


SPI:

```c++
ST7567 st7567(WIDTH, HEIGHT, {
    PIMORONI_SPI_DEFAULT_INSTANCE, // SPI instance
    SPI_BG_FRONT_CS,               // Chip-select
    SPI_DEFAULT_SCK,               // SPI Clock
    SPI_DEFAULT_MOSI,              // SPI Out
    PIN_UNUSED,                    // SPI In
    SPI_DEFAULT_DC,                // SPI Data/Command
    PIN_UNUSED                     // Backlight
});
```

## Reference

### Update

ST7567's `update` accepts an instance of `PicoGraphics` in 1 bit colour mode:

```c++
st7567.update(&graphics);
```

### Set Backlight

If a backlight pin has been configured, you can set the backlight from 0 to 255:

```c++
st7567.set_backlight(128)
```