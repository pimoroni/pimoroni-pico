# Badger 2040 Examples <!-- omit in toc -->

- [Function Examples](#function-examples)
  - [Battery](#battery)
  - [Button Test](#button-test)
- [Application Examples](#application-examples)
  - [Badge](#badge)
  - [Checklist](#checklist)
  - [Clock](#clock)
  - [E-Reader](#e-reader)
  - [Fonts](#fonts)
  - [Image](#image)
  - [Launcher](#launcher)


## Function Examples

### Battery
[battery.py](battery.py)

An example of how to read the battery voltage and display a battery level indicator.

### Button Test
[button_test.py](button_test.py)

An example of how to read Badger2040's buttons and display a unique message for each.

### LED
[led.py](led.py)

Blinks Badger's LED on and off. A lot.


## Application Examples

### Badge
[badge.py](badge.py)

Create your own name badge! This application looks for two files on your MicroPython drive:
* "badge.txt" - A text file containing 6 lines, corresponding to the 6 different piece of text on the badge
* "badge-image.bin" - A 108x128px 1bit colour depth image to display alongside the text. You can use examples/badger2040/image_converter/convert.py to convert them:

python3 convert.py --binary --resize image_file_1.png image_file_2.png image_file_3.png

### Checklist
[checklist.py](checklist.py)

A checklist application, letting you navigate through items and tick each of them off.

### Clock
[clock.py](clock.py)

A simple clock showing the time and date, that uses the E Ink's fast speed to update every second

### E-Reader
[e-reader.py](e-reader.py)

A mini text file e-reader. Comes pre-loaded with an excerpt of The Wind In the Willows.

### Fonts
[Fonts.py](checklist.py)

A demonstration of the various fonts that can be used in your programs.

### Image
[image.py](image.py)

An image gallery. Displays and lets you cycle through any images stored within the MicroPython device's `/images` directory. Images must be 296x128 pixels with 1bit colour depth. You can use examples/badger2040/image_converter/convert.py to convert them:

python3 convert.py --binary --resize image_file_1.png image_file_2.png image_file_3.png

### Launcher
[Launcher.py](launcher.py)

A launcher style application, that gives a menu of other applications that can be loaded, as well as information such as battery level.