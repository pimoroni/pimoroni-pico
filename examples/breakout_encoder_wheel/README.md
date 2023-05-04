# Encoder Wheel Breakout C++ Examples <!-- omit in toc -->

- [Function Examples](#function-examples)
  - [Buttons](#buttons)
  - [Encoder](#encoder)
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
[buttons/buttons.cpp](buttons/buttons.cpp)

A demonstration of reading the 5 buttons on Encoder Wheel.


### Encoder
[encoder/encoder.cpp](encoder/encoder.cpp)

A demonstration of reading the rotary dial of the Encoder Wheel breakout.


## LED Examples

### LED Rainbow
[led_rainbow/led_rainbow.cpp](led_rainbow/led_rainbow.cpp)

Displays a rotating rainbow pattern on Encoder Wheel's LED ring.


### Clock
[clock/clock.cpp](clock/clock.cpp)

Displays a 12 hour clock on Encoder Wheel's LED ring, getting time from the system.


## Interactive Examples

### Colour Picker
[colour_picker/colour_picker.cpp](colour_picker/colour_picker.cpp)

Create a colour wheel on the Encoder Wheel's LED ring, and use all functions of the wheel to interact with it.


### Stop Watch
[stop_watch/stop_watch.cpp](stop_watch/stop_watch.cpp)

Display a circular stop-watch on the Encoder Wheel's LED ring.


### Chase Game
[chase_game/chase_game.cpp](chase_game/chase_game.cpp)

A simple alignment game. Use Encoder Wheel's rotary dial to align the coloured band to the white goal. The closer to the goal, the greener your coloured band will be. When you reach the goal, the goal will move to a new random position.


## GPIO Examples

### GPIO PWM
[gpio_pwm/gpio_pwm.cpp](gpio_pwm/gpio_pwm.cpp)

Output a sine wave PWM sequence on the Encoder Wheel's side GPIO pins.
