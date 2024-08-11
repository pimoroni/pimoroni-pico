# Badger 2040 Examples <!-- omit in toc -->

:warning: This code has been deprecated in favour of a dedicated Badger 2040 project: https://github.com/pimoroni/badger2040

- [Function Examples](#function-examples)
  - [Battery](#battery)
  - [Button Test](#button-test)
  - [LED](#led)
  - [Pin interrupt](#pin-interrupt)
- [Application Examples](#application-examples)
  - [Badge](#badge)
  - [Checklist](#checklist)
  - [Clock](#clock)
  - [E-Book](#e-book)
  - [Fonts](#fonts)
  - [Image](#image)
  - [QR gen](#qr-gen)
  - [Launcher](#launcher)
  - [Conway](#conway)


## Function Examples

### Battery
[battery.py](battery.py)

An example of how to read the battery voltage and display a battery level indicator.

### Button Test
[button_test.py](button_test.py)

An example of how to read Badger2040's buttons and display a unique message for each.

### LED
[led.py](led.py)

Blinks Badger's LED on and off.

### Pin interrupt
[pin_interrupt.py](pin_interrupt.py)

An example of drawing text and graphics and using the buttons.

## Application Examples

### Badge
[badge.py](badge.py)

Create your own name badge! This application looks for two files on your MicroPython drive:
* `badge.txt` - A text file containing 6 lines, corresponding to the 6 different pieces of text on the badge
* `badge-image.bin` - A 104x128px 1-bit colour depth image to display alongside the text. You can use `examples/badger2040/image_converter/convert.py` to convert them:

```shell
python3 convert.py --binary --resize image_file_1.png image_file_2.png image_file_3.png
```

### Checklist
[list.py](list.py)

A checklist application, letting you navigate through items and tick each of them off.

* `checklist.txt` - A text file containing the titles of items for the list.

### Clock
[clock.py](clock.py)

A simple clock showing the time and date, that uses the E Ink's fast speed to update every second.

### E-Book
[ebook.py](ebook.py)

A mini text file e-reader. Comes pre-loaded with an excerpt of The Wind In the Willows.

### Fonts
[fonts.py](fonts.py)

A demonstration of the various fonts that can be used in your programs.

### Image
[image.py](image.py)

An image gallery. Displays and lets you cycle through any images stored within the MicroPython device's `/images` directory. Images must be 296x128 pixels with 1-bit colour depth. You can use `examples/badger2040/image_converter/convert.py` to convert them:

```shell
python3 convert.py --binary --resize image_file_1.png image_file_2.png image_file_3.png
```

### QR gen
[qrgen.py](qrgen.py)

Displays and lets you cycle through multiple QR codes, with configuration stored in text files within the MicroPython device's `/qrcodes` directory.

- `/qrcodes/qrcode.txt` - A text file containing 9 lines. The first line should be a URL which will be converted into and displayed as a QR code. Up to 8 more lines of information can be added, which will be shown as plain text to the right of the QR code.
- `/qrcodes/*.txt` - additional text files can be created using the same format. All text files can be cycled through.

### Launcher
[launcher.py](launcher.py)

A launcher-style application, providing a menu of other applications that can be loaded, as well as information such as battery level.

### Conway
[conway.py](conway.py)

Conway's classic Game of Life, implemented on the Badger. Note: this application is *not* linked from the Launcher by default - it can be run directly using Thonny or your MicroPython editor of choice, or you can modify the Launcher to add it (you'll want to update `launchericons.png` as well)
