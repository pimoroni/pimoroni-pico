# Pico Explorer Base <!-- omit in toc -->

Pico Explorer Base straps a whole host of physical computing goodies to your Pico - a vibrant 1.14" (240x240) IPS LCD screen, four switches, a piezo buzzer/speaker and a DRV8833 motor driver, as well as handy accessible general purpose inputs and outputs and a built in breadboard.

We've included helper functions to handle every aspect of drawing to the screen and interfacing with the buttons, piezo and motor driver. [See the library reference](#reference) for details.

- [Example Program](#example-program)
- [Reference](#reference)
  - [PicoGraphics](#picographics)
  - [Constants](#constants)
    - [Buttons](#buttons)
    - [ADC Channels](#adc-channels)
    - [GPIO](#gpio)
  - [Functions](#functions)
    - [init](#init)
    - [set_backlight](#set_backlight)
    - [set_motor](#set_motor)
    - [get_adc](#get_adc)
    - [set_audio_pin](#set_audio_pin)
    - [set_tone](#set_tone)
    - [is_pressed](#is_pressed)
    - [update](#update)

## Example Program

The following example sets up Pico Explorer, displays some basic demo text and graphics and will illuminate the RGB LED green if the A button is pressed.

```c++
#include "pico_explorer.hpp"

using namespace pimoroni;

uint16_t buffer[PicoExplorer::WIDTH * PicoExplorer::HEIGHT];
PicoExplorer pico_explorer(buffer);

int main() {
    pico_explorer.init();

    while(true) {

        // now we've done our drawing let's update the screen
        pico_explorer.update();
    }
}
```

## Reference

### PicoGraphics

Pico Explorer uses our Pico Graphics library to draw graphics and text. For more information [read the Pico Graphics function reference.](../pico_graphics/README.md#function-reference).

Please note that the backlight on Pico Explorer is not dimmable (we needed the pins to hook up other functions) so the `set_backlight` function is not available for this board.

### Constants

#### Buttons

The four buttons, A, B, X and Y have corresponding constants set to their respective GPIO pins. For example:

```c++
bool a_is_pressed = pico_explorer.is_pressed(pico_explorer.A);
```

#### ADC Channels

The three ADC channels are defined as `ADC0`, `ADC1` and `ADC2`, and should be used with `get_adc`, eg:

```c++
float adc0_reading = pico_explorer.get_adc(pico_explorer.ADC0);
```

#### GPIO

The 8 general purpose IO pins on the lower Pico Explorer are GP0 through GP7, we've created constants for you to identify them easily. You should use Pico SDK's `gpio_get` to read a pin, eg:

```c++
bool pin_state = gpio_get(pico_explorer.GP0);
```

### Functions

#### init

Sets up Pico Explorer. `init` must be called before any other functions since it configures the required PWM and GPIO:

```c++
pico_explorer.init();
```

#### set_motor

```c++
void PicoExplorer::set_motor(uint8_t channel, uint8_t action, float speed);
```

Motors are driven by PWM via an onboard DRV8833, `set_motor` will set the PWM values for the corresponding channel.

Channel should be one of `MOTOR1` or `MOTOR2`.

Action should be `FORWARD`, `REVERSE` or `STOP`.

Speed should be given as a float between `0.0` and `1.0`, eg:

```c++
pico_explorer.set_motor(pico_explorer.MOTOR1, pico_explorer.FORWARD, 0.5f);
pico_explorer.set_motor(pico_explorer.MOTOR2, pico_explorer.REVERSE, 0.5f);
```

And to stop the motor:

```c++
pico_explorer.set_motor(pico_explorer.MOTOR1, pico_explorer.STOP);
pico_explorer.set_motor(pico_explorer.MOTOR2, pico_explorer.STOP);
```

#### get_adc

```c++
float get_adc(uint8_t channel);
```

Pico Explorer's ADC channels are connected to Pico's ADC-capable pins 26, 27 and 28 which correspond to channels 0, 1 and 2 respectively. eg:

```c++
float reading = pico_explorer.get_adc(pico_explorer.ADC0);
```

Will perform a 12-bit ADC read and return the result as a float scaled from `0.0f` to `1.0f`. This value can be plugged directly into a motor, eg:

```c++
float reading = pico_explorer.get_adc(pico_explorer.ADC0);
pico_explorer.set_motor(pico_explorer.MOTOR1, pico_explorer.FORWARD, reading);
```

#### set_audio_pin

```c++
void set_audio_pin(uint8_t p);
```

`set_audio_pin` configures the PIN that Pico Explorer uses for audio output. It should be one of `GP0` through `GP7`, eg:

```c++
pico_explorer.set_audio_pin(pico_explorer.GP0);
```

Note: You must bridge this pin over to the `AUDIO` pin on the Pico Explorer header in order to drive the onboard Piezo, eg:

#### set_tone

```c++
void set_tone(uint16_t frequency, float duty = 0.2f);
```

`set_tone` will play an audio tone out of your chosen audio pin, if you have bridged that pin to "AUDIO" on the Pico Explorer header then it will sound the onboard Piezo.

```c++
uint16_t frequency = 440;
pico_explorer.set_tone(frequency);
```

#### is_pressed

```c++
bool is_pressed(uint8_t button);
```

Reads the GPIO pin connected to one of Pico Explorer's buttons, returning a `bool` - `true` if it's pressed and `false` if it is released.

```c++
pico_explorer.is_pressed(button);
```

The button vaule should be a `uint8_t` denoting a pin, and constants `A`, `B`, `X` and `Y` are supplied to make it easier. e:

```c++
bool is_a_button_pressed = pico_explorer.is_pressed(PicoDisplay::A)
```

#### update

To display your changes on Pico Explorer's screen you need to call `update`:

```c++
pico_explorer.update();
```