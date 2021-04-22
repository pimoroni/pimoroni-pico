# Pico Explorer Base

Pico Explorer Base straps a whole host of physical computing goodies to your Pico - a vibrant 1.14" (240x240) IPS LCD screen, four switches, a piezo buzzer/speaker and a DRV8833 motor driver, as well as handy accessible general purpose inputs and outputs and a built in breadboard.

We've included helper functions to handle drawing to the screen and interfacing with the buttons, piezo and motor driver. 

Check out [tonygo2's Explorer Base workout](https://www.instructables.com/Raspberry-Pi-Pico-Pico-Explorer-Workout/) for a comprehensive demo and walk-through of the functions!

  * [Example Program](#example-program)
  * [Function Reference](#function-reference)
    + [Display](#display)
      - [init](#init)
      - [update](#update)
      - [set_pen](#set-pen)
      - [create_pen](#create-pen)
      - [clear](#clear)
      - [pixel](#pixel)
      - [pixel_span](#pixel-span)
      - [rectangle](#rectangle)
      - [circle](#circle)
      - [character](#character)
      - [text](#text)
      - [set_clip](#set-clip)
      - [remove_clip](#remove-clip)
    + [Buttons](#buttons)
      - [is_pressed](#is-pressed)
    + [ADC](#adc)
      - [get_adc](#get-adc)
    + [Motors](#motors)
      - [set_motor](#set-motor)
    + [Audio](#audio)
      - [set_audio_pin](#set-audio-pin)
      - [set_tone](#set-tone)
    + [GPIO](#gpio)

## Example Program

This simple example sets up Pico Explorer, displays some basic demo text and displays a message when the A button is pressed. Check out [the examples folder](/micropython/examples/pico_explorer) for more!

```python
import utime
import picoexplorer

# Initialise Picoexplorer with a bytearray display buffer
buf = bytearray(picoexplorer.get_width() * picoexplorer.get_height() * 2)
picoexplorer.init(buf)

picoexplorer.set_pen(255, 0, 0)                      # Set a red pen
picoexplorer.clear()                                 # Fill the screen in red
picoexplorer.set_pen(255, 255, 255)                  # Set a white pen
picoexplorer.text("pico explorer", 10, 10, 240, 5)   # Add some text
picoexplorer.update()                                # Update the display with our changes

while picoexplorer.is_pressed(picoexplorer.BUTTON_A) == False:
    pass

picoexplorer.set_pen(0, 255, 0)                      # Set a green pen
picoexplorer.clear()                                 # Fill the screen in green
picoexplorer.set_pen(0, 0, 0)                        # Set a black pen
picoexplorer.text("button a pushed", 10, 10, 240, 5) # Add some text
picoexplorer.update()                                # Update the display with our changes
```

## Function Reference

### Display

Pico Explorer uses a shared Pico Graphics library to draw graphics and text on its screen. As a result, code written for our other Pico add-on boards with a display should be easy to convert to run on Explorer, but make sure to call `picoexplorer` to get the correct screen dimensions and orientation.

Please note that the backlight on Pico Explorer is not dimmable (we needed the pins to hook up other functions) so there's no `set_backlight` function for this board.

#### init

Sets up Pico Explorer. `init` **must** be called before any other functions (even if you're not using the screen) since it configures the required PWM and GPIO. `init()` needs a bytearray type display buffer that MicroPython's garbage collection isn't going to eat, make sure you create one and pass it in like so:

```python
buf = bytearray(picoexplorer.get_width() * picoexplorer.get_height() * 2)
picoexplorer.init(buf)
```

#### update

Once you've finished drawing stuff with the functions below, you need to call `update` to display your changes on Pico Explorer's screen .

```python
picoexplorer.update()
```

#### set_pen

Sets the colour to be used by subsequent calls to drawing functions.  The values for `r`, `g` and `b` should be from 0-255 inclusive.

```python
picoexplorer.set_pen(r, g, b)
```

#### create_pen

Creates a pen which can be stored as a variable for faster re-use of the same colour through calls to `set_pen`.  The values for `r`, `g` and `b` should be from 0-255 inclusive.

```python
pen_colour = picoexplorer.create_pen(r, g, b)
picoexplorer.set_pen(pen_colour)
```

#### clear

Fills the display buffer with the currently set pen colour.

```python
picoexplorer.clear()
```

#### pixel

Sets a single pixel in the display buffer to the current pen colour.  The `x` and `y` parameters determine the X and Y coordinates of the drawn pixel in the buffer.

```python
picoexplorer.pixel(x, y)
```

#### pixel_span

Draws a horizontal line of pixels to the buffer.  The `x` and `y` parameters specify the coordinates of the first pixel of the line.  The `l` parameter describes the length of the line in pixels.  This function will only extend the line towards the end of the screen, i.e. the `x` coordinate should specify the left hand extreme of the line.

```python
picoexplorer.pixel_span(x, y, l)
```

#### rectangle

Draws a rectangle filled with the current pen colour to the buffer.  The `x` and `y` parameters specify the upper left corner of the rectangle, `w` specifies the width in pixels, and `h` the height.

```python
picoexplorer.rectangle(x, y, w, h)
```

![Rectangle function explanation image](/micropython/modules/pico_display/images/rectangle.png)

#### circle

Draws a circle filled with the current pen colour to the buffer.  The `x` and `y` parameters specify the centre of the circle, `r` specifies the radius in pixels.

```python
picoexplorer.circle(x, y, r)
```

![Circle function explanation image](/micropython/modules/pico_display/images/circle.png)

#### character

Draws a single character to the display buffer in the current pen colour.  The `c` parameter should be the ASCII numerical representation of the character to be printed, `x` and `y` describe the top-left corner of the character's drawing field.  The `character` function can also be given an optional 4th parameter, `scale`, describing the scale of the character to be drawn.  Default value is 2.

```python
char_a = ord('a')
picoexplorer.character(char_a, x, y)
picoexplorer.character(char_a, x, y, scale)
```

#### text

Draws a string of text to the display buffer in the current pen colour.  The `string` parameter is the string of text to be drawn, and `x` and `y` specify the upper left corner of the drawing field.  The `wrap` parameter describes the width, in pixels, after which the next word in the string will be drawn on a new line underneath the current text.  This will wrap the string over multiple lines if required.  This function also has an optional parameter, `scale`, which describes the size of the characters to be drawn.  The default `scale` is 2.

```python
picoexplorer.text(string, x, y, wrap)
picoexplorer.text(string, x, y, wrap, scale)
```

![Text scale explanation image](/micropython/modules/pico_display/images/text_scale.png)

#### set_clip

This function defines a rectangular area outside which no drawing actions will take effect.  If a drawing action crosses the boundary of the clip then only the pixels inside the clip will be drawn.  Note that `clip` does not remove pixels which have already been drawn, it only prevents new pixels being drawn outside the described area.  A more visual description of the function of clips can be found below.  Only one clip can be active at a time, and defining a new clip replaces any previous clips.  The `x` and `y` parameters describe the upper-left corner of the clip area, `w` and `h` describe the width and height in pixels.

```python
picoexplorer.set_clip(x, y, w, h)
```

![Clip function explanation image](/micropython/modules/pico_display/images/clip.png)

#### remove_clip

This function removes any currently implemented clip.

### Buttons

The four buttons, A, B, X and Y have corresponding constants set to their respective GPIO pins. 

#### is_pressed

Reads the GPIO pin connected to one of Pico Explorer's buttons, returning `True` if it's pressed and `False` if it is released.

```python
picoexplorer.is_pressed(button)
```

The button value should be a number denoting a pin, and constants `picoexplorer.BUTTON_A`, `picoexplorer.BUTTON_B`, `picoexplorer.BUTTON_X` and `picoexplorer.BUTTON_Y` are supplied to make it easier. e:

```python
is_a_button_pressed = picoexplorer.is_pressed(picoexplorer.BUTTON_A)
```

### ADC

Pico Explorer's ADC channels are connected to Pico's ADC-capable pins 26, 27 and 28 which correspond to channels 0, 1 and 2 respectively. `get_adc` will perform a 12-bit ADC read and return the result as a float scaled from `0.0` to `1.0`.

#### get_adc

```python
picoexplorer.get_adc(channel)
```

The three ADC channels are defined as `0`, `1` and `2`, and should be used with `get_adc`, eg:

```python
adc0 = picoexplorer.get_adc(0)
```

This value can be plugged directly into a motor, eg:

```python
adc0 = picoexplorer.get_adc(0)
picoexplorer.set_motor(1, 1, adc0)
```

### Motors

Motors are driven by PWM via an onboard DRV8833. `set_motor` will set the PWM values for the corresponding channel.

The red LED next to the motor connectors is part of the motor driver circuit - it will light up if the overvoltage/undervoltage/short circuit auto shutdown functions of the motor driver are triggered. It's not user controllable.

#### set_motor

```python
picoexplorer.set_motor(channel, action, speed)
```

Channel should be one of `0` (motor 1) or `1` (motor 2).

Action should be `0` (forwards) or `1` (backwards).

Speed should be given as a number between `0.0` and `1.0`, eg:

```python
picoexplorer.set_motor(0, 0, 0.5)    # motor 1 forwards
picoexplorer.set_motor(1, 1, 0.5)    # motor 2 backwards
```

And to stop the motor:

```python
picoexplorer.set_motor(0, 0, 0)      # motor 1 stop
picoexplorer.set_motor(1, 0, 0)      # motor 2 stop
```

### Audio

To make noise with Explorer, you must first select one of the GP0 to GP7  pins to PWM for audio. You'll then need to connect this pin to AUDIO with a jumper wire.

#### set_audio_pin

```python
picoexplorer.set_audio_pin(channel)
```

`set_audio_pin` configures the PIN that Pico Explorer uses for audio output. It should be one of `GP0` through `GP7`, eg:

```python
picoexplorer.set_audio_pin(0)
```

This pin must be bridged to the `AUDIO` pin on the Pico Explorer header in order to drive the onboard Piezo.

#### set_tone

```python
picoexplorer.set_tone(frequency)
```

`set_tone` will play an audio tone out of your chosen audio pin.

```python
frequency = 440
picoexplorer.set_tone(frequency)
```

To make the buzzer be quiet, you can:

``` python
picoexplorer.set_tone(-1)
```

### GPIO

The 8 general purpose IO pins on the lower Pico Explorer are GP0 through GP7. You can use `machine` to read a pin in the same way as you would if you were using a Pico on its own.

```python
import machine

GP0 = machine.Pin(0, machine.Pin.IN, machine.Pin.PULL_DOWN)
```

There's lots more info about how to use `machine` in the [Raspberry Pi documentation](https://www.raspberrypi.org/documentation/rp2040/getting-started/#getting-started-with-micropython).