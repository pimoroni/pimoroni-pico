# ST7701 Display Driver for Pimoroni LCDs <!-- omit in toc -->

The ST7701 driver supports both Parallel and Serial (SPI) ST7701 displays and is intended for use with:

* Redacted
* Wouldn't you like to know
* Super Sekret

## Setup

Construct an instance of the ST7701 driver with either Parallel or SPI pins.

Parallel:

```c++
ST7701 st7701(WIDTH, HEIGHT, ROTATE_0, {
    Tufty2040::LCD_CS,   // Chip-Select
    Tufty2040::LCD_DC,   // Data-Command
    Tufty2040::LCD_WR,   // Write
    Tufty2040::LCD_RD,   // Read
    Tufty2040::LCD_D0,   // Data 0 (start of a bank of 8 pins)
    Tufty2040::BACKLIGHT // Backlight
});
```

SPI:

```c++
ST7701 st7701(WIDTH, HEIGHT, ROTATE_0, false, {
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

ST7701's `update` accepts an instance of `PicoGraphics` in any colour mode:

```c++
st7701.update(&graphics);
```

### Set Backlight

If a backlight pin has been configured, you can set the backlight from 0 to 255:

```c++
st7701.set_backlight(128)
```