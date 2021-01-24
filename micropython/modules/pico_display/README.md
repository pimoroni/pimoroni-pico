# Pico Display Pack - MicroPython <!-- omit in toc -->

Our Pico Display Pack offers a vibrant 1.14" (240x135) IPS LCD screen for your Raspberry Pi Pico it also includes four switches and and an RGB LED!

We've included helper functions to handle every aspect of drawing to the screen and interfacing with the buttons and LED. See the [function reference](#function-reference) for details.

- [Example Program](#example-program)
- [Function Reference](#function-reference)
  - [init](#init)
  - [set_backlight](#set_backlight)
  - [set_led](#set_led)
  - [is_pressed](#is_pressed)
  - [update](#update)

## Example Program

The following example sets up Pico Display, displays some basic demo text and graphics and will illuminate the RGB LED green if the A button is presse

```python
import picodisplay

# Initialise Picodisplay with a bytearray display buffer
buf = bytearray(picodisplay.get_width() * picodisplay.get_height() * 2)
picodisplay.init(buf)

picodisplay.set_pen(255, 0, 0)  # Set a red pen
picodisplay.clear()             # Clear the display buffer
picodisplay.update()            # Update the display with our changes

picodisplay.set_led(255, 0, 0)  # Set the RGB LED to Red
picodisplay.set_led(0, 255, 0)  # Set the RGB LED to Green
picodisplay.set_led(0, 0, 255)  # Set the RGB LED to Blue

while not picodisplay.is_pressed(picodisplay.BUTTON_A):  # Wait for Button A to be pressed
    pass
```

## Function Reference

### init

Sets up Pico Display. `init` must be called before any other functions since it configures the required PWM and GPIO. `init()` needs a bytearray type display buffer that MicroPython's garbage collection isn't going to eat, make sure you create one and pass it in like so:

```python
buf = bytearray(picodisplay.get_width() * picodisplay.get_height() * 2)
picodisplay.init(buf)
```

### set_backlight

Set the display backlight from 0.0 to 1.0

```python
picodisplay.set_backlight(brightness)
```

Uses hardware PWM to dim the display backlight, dimming values are gamma-corrected to provide smooth brightness transitions across the full range of intensity. This may result in some low values mapping as "off."

### set_led

Sets the RGB LED on Pico Display with an RGB triplet:

```python
picodisplay.set_led(r, g, b)
```

Uses hardware PWM to drive the LED. Values are automatically gamma-corrected to provide smooth brightness transitions and low values may map as "off."

### is_pressed

Reads the GPIO pin connected to one of Pico Display's buttons, returning `True` if it's pressed and `False` if it is released.

```python
picodisplay.is_pressed(button)
```

The button vaule should be a number denoting a pin, and constants `picodisplay.BUTTON_A`, `picodisplay.BUTTON_B`, `picodisplay.BUTTON_X` and `picodisplay.BUTTON_Y` are supplied to make it easier. e:

```python
is_a_button_pressed = picodisplay.is_pressed(picodisplay.BUTTON_A)
```

### update

To display your changes on Pico Display's screen you need to call `update`:

```python
picodisplay.update()
```