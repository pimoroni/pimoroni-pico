# RGB Encoder Wheel Breakout Examples (Micropython) <!-- omit in toc -->

- [Function Examples](#function-examples)
  - [Buttons](#buttons)
  - [Encoder](#encoder)
  - [Interrupt](#interrupt)
- [LED Examples](#led-examples)
  - [LED Rainbow](#led-rainbow)
  - [Clock](#clock)
- [Interactive Examples](#interactive-examples)
  - [Colour Picker](#colour-picker)
  - [Stop Watch](#stop-watch)
  - [Chase Game](#chase-game)
- [GPIO Examples](#gpio-examples)
  - [GPIO PWM](#gpio-pwm)


## Function Examples

### Buttons
[buttons.py](buttons.py)

A demonstration of reading the 5 buttons on Encoder Wheel.


### Encoder
[encoder.py](encoder.py)

A demonstration of reading the rotary dial of the Encoder Wheel breakout.


### Interrupt
[interrupt.py](interrupt.py)

How to read the buttons and rotary dial of the Encoder Wheel breakout, only when an interrupt occurs.


## LED Examples

### LED Rainbow
[led_rainbow.py](led_rainbow.py)

Displays a rotating rainbow pattern on Encoder Wheel's LED ring.


### Clock
[clock.py](clock.py)

Displays a 12 hour clock on Encoder Wheel's LED ring, getting time from the system.


## Interactive Examples

### Colour Picker
[colour_picker.py](colour_picker.py)

Create a colour wheel on the Encoder Wheel's LED ring, and use all functions of the wheel to interact with it.


### Stop Watch
[stop_watch.py](stop_watch.py)

Display a circular stop-watch on the Encoder Wheel's LED ring.


### Chase Game
[chase_game.py](chase_game.py)

A simple alignment game. Use Encoder Wheel's rotary dial to align the coloured band to the white goal. The closer to the goal, the greener your coloured band will be. When you reach the goal, the goal will move to a new random position.


## GPIO Examples

### GPIO PWM
[gpio_pwm.py](gpio_pwm.py)

Output a sine wave PWM sequence on the Encoder Wheel's side GPIO pins.
