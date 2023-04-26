# Inky Frame (MicroPython) <!-- omit in toc -->

Most of your interaction with Inky Frame will be via the PicoGraphics library, but we have created an `inky_frame` module to help you read the onboard A, B, C, D and E buttons and control the LEDs.

- [Pico Graphics](#pico-graphics)
  - [Colour \& Dithering](#colour--dithering)
  - [Images \& JPEGs](#images--jpegs)
- [Buttons \& LEDs](#buttons--leds)
  - [Status LEDs](#status-leds)
- [Battery Power \& RTC](#battery-power--rtc)
  - [Function Reference](#function-reference)
    - [Wakeup States](#wakeup-states)
    - [RTC](#rtc)
    - [Other](#other)
- [RAM Usage](#ram-usage)
  - [Accessing The Framebuffer](#accessing-the-framebuffer)


## Pico Graphics

You can draw on Inky Frame using our PicoGraphics display library.
- [PicoGraphics MicroPython function reference](../modules/picographics)

### Colour & Dithering

Inky Frame supports eight native colours, those that it's physically capable of displaying. These are:

* `BLACK` = 0
* `WHITE` = 1
* `GREEN` = 2
* `BLUE` = 3
* `RED` = 4
* `YELLOW` = 5
* `ORANGE` = 6
* `TAUPE` = 7 (only used for dither on 4.0" and 5.7" displays †)

:info: † - the "taupe" colour (also known as clear) is the colour used to reset the display to a default, clear state. We misuse this as an extra colour on 4.0" and 5.7", but the 7.3" display clear colour is a sort of muddy, greenish gradient that's not consistent enough. You can stil use it, by all means, but it wont be considered for dithering.

These colours are available as constants in the `inky_frame` module so you don't have to remember which number corresponds to which, eg:

```python
display.set_pen(inky_frame.YELLOW)
```

You can use colours outside of these eight by using `create_pen(r, g, b)` or `create_pen_hsv(h, s, v)`. eg:

```python
my_colour = display.create_pen(255, 128, 0)
display.set_pen(my_colour)
```

PicoGraphics will do its best to match your colour choice by mixing the available colours with a dither pattern. This works best on large areas of colour such as backgrounds, big UI elements or chunky text. See the `inky_frame_dithering.py` example for a demonstration.

:info: Due to the lack of light blue or pink native colours, Inky Frame isn't very good at reproducing pink, purple, violet or light blue, cyan and teal.


### Images & JPEGs

You can use the `jpegdec` (JPEG decoding) module to display JPEG files on Inky, but you should be aware of some caveats:

1. JPEGs are compressed and lossy, a small JPEG displayed on Inky might show random specks of colour where you don't expect them as PicoGraphics tries its best to dither noise to the nearest available colours.
2. JPEGs are, by default, dithered to the 7 (or 8 on 4.0 and 5.7) available colours. This uses "ordered dither," which is fast, but not very pretty.
3. You can turn off dithering with `jpeg.decode(dither=False)` for a posterised effect.

## Buttons & LEDs

Inky Frame has five user buttons labelled A to E, with LEDs. The buttons are connected via a shift-register and available in the `inky_frame` module as `button_a`, `button_b`, `button_c`, `button_d` and `button_e`.

Each button has some convenient methods for checking if it's pressed:

* `raw()` - get the current state of the button with no debounce (returns `True` the first time it's called if the corresponding button)
* `read()` - read the current button state, observing debounce (50ms)

Additionally each button has some methods for controlling its associated LED:

* `led_on()` - turn the LED on
* `led_off()` - turn the LED off
* `led_toggle()` - toggle the LED
* `led_brightness(0.5)` - set the LED brightness (from 0.0 to 1.0)

### Status LEDs

In addition to the button LEDs there are two status LEDs, `busy` and `wifi`, which are available as:

* `led_busy` - connected to pin `LED_BUSY`
* `led_wifi` - connected to pin `LED_WIFI`

These are instances of the `pimoroni.PWMLED` class, and have the following methods:

* `on()` - turn the LED on
* `off()` - turn the LED off
* `toggle()` - toggle the LED
* `brightness(0.5)` - set the LED brightness (from 0.0 to 1.0)

:info: `toggle()` is provided for compatibility with the `Pin()` class. It's a little odd, since it changes the LED brightness to `1.0 - current_brightness`.

## Battery Power & RTC

When running on battery power, Inky Frame's buttons cause it to wake from a power-off state and start running your code. Additionally an onboard PCF85063A real-time clock runs continuously from battery and can wake up your Frame automatically.

The Inky Frame library includes a number of convenience functions to set the clock and sleep your device. If you want accurate time you must check if the clock is set - usually a simple check against the year works - connect to a network, and set via NTP (Network Time Protocol).

Inky Frame technically has *two* real-time clocks, the external RTC we've added - which remains continuously powered by battery - and the internal RTC of the Pico W. For convenience and compatibility with other code examples we recommend using the external RTC only to set the time on the internal one which makes functions like `time.localtime()` work as you'd expect.

```python
import time
import machine
import inky_frame

inky_frame.pcf_to_pico_rtc()  # Sync Inky RTC time to Pico's RTC

year, month, day, dow, hour, minute, second, _ = machine.RTC().datetime()

if year < 2023:
    # Connect to network
    inky_frame.set_time()  # Sets both the Inky and Pico RTC

print(time.localtime())
```

### Function Reference

#### Wakeup States

For your convenience these wakeup state functions also check the *current* state of their associated event, this allows you to run code from Thonny with a button held down - for example - to test how your deployed code will behave on battery-

* `inky_frame.woken_by_rtc()` - Returns `True` if the RTC caused a wakeup, or if the RTC ALARM is currently raised.
* `inky_frame.woken_by_button()` - Returns `True` if a button caused a wakeup, or if a button is currently pressed.
* `inky_frame.woken_by_ext_trigger()` Returns `True` if the external trigger caused a wakeup, or if the trigger is currently asserted.

#### RTC

* `inky_frame.set_time()` - Attempt to run `ntptime.settime()` and set the time on both RTCs
* `inky_frame.pcf_to_pico_rtc()` - Sync from Inky's RTC to the Pico W's RTC
* `inky_frame.pico_rtc_to_pcf()` - Sync from Pico W's RTC to Inky's RTC (sometime useful since Thonny sets the Pico W RTC automatically)
* `inky_frame.sleep_for(minutes)` - Set the RTC alarm for a number of minutes and cut the power. This will completely power off the Pico W, but leave the Inky RTC running to wake it back up.

:info: You can access all Inky RTC (PCF85063A) functions via `inky_frame.rtc`

#### Other

* `inky_frame.turn_off()` - Cut the power to the Pico W (on battery only), only an alarm event or a button press can wake it back up.

## RAM Usage

Both Inky 4.0" and 5.7" use only the Pico's onboard RAM. It's quite cozy. The frame buffers are 3-bits-per-pixel, making Inky 4.0" 

On 7.3" we had to add a PSRAM chip to act as the display's framebuffer. Right now it acts exclusively as a framebuffer, but that frees up some Pico RAM to work with so you can do more with that 7.3" panel.

The Inky frame buffer sizes are as follows:

* 4.0" - 640x400 - 96,000 bytes.
* 5.7" - 600x448 - 100,800 bytes.
* 7.3" - 800x480 - 144,000 bytes.

Since MicroPython on a Pico W has only 166k that would have left *just* 22k on 7.3", instead you get (almost) all 166k to play with since the PicoGraphics instance itself uses only 8544 bytes!

### Accessing The Framebuffer

PicoGraphics has undocumented support for accessing its raw framebuffer using `memoryview(graphics)`.

This is useful for copying raw binary images (effectively valid Inky frame buffers saved to a file) avoiding JPEG compression and so forth. For some dicussion about why and how you might do this, see: https://github.com/pimoroni/pimoroni-pico/issues/681

:warning: This *does not work* for Inky 7.3, since there is no framebuffer in memory. PicoGraphics will raise a `ValueError: No local framebuffer.` if you try. We aim to fix this with some hardfault handling sorcery.
