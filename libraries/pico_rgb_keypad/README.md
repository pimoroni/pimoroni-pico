# Pico RGB Keypad Pack <!-- omit in toc -->

Our Pico Keypad Pack offers a 4x4, squishy, RGB illuminated keypad for the Raspberry Pi Pico. The buttons are read via an i2c IO expander and the LEDs are APA102s driven by SPI.

We've included helper functions to handle every aspect of setting beautiful RGB colours and interfacing with the squishy buttons. [See the library reference](#reference) for details.

- [Example Program](#example-program)
- [Reference](#reference)
  - [Constants](#constants)
    - [Dimensions](#dimensions)
  - [Functions](#functions)
    - [set_brightness](#set_brightness)
    - [illuminate](#illuminate)
    - [clear](#clear)
    - [get_button_states](#get_button_states)
    - [update](#update)

## Example Program

The following example sets up Pico RGB Keypad and toggles the key LEDs on/off when they are pressed.

```c++
#include "pico_rgb_keypad.hpp"

using namespace pimoroni;

PicoRGBKeypad pico_rgb_keypad;

uint16_t last_buttons = 0;
uint16_t led_states = 0;

int main() {
    pico_rgb_keypad.init(); // Set up GPIO
    pico_rgb_keypad.set_brightness(0.1f); // It's bright, trust us!

    while(true) {
        current_buttons = get_button_states();
        changed_buttons = current_buttons ^ last_buttons;
        pressed_buttons = current_buttons & changed_buttons;
        released_buttons =  last_buttons & changed_buttons;

        // Toggle the LED states when a button is pressed
        led_states ^= pressed_buttons;

        // Grab each bit from led_states and update the
        // corresponding LED
        for(auto i = 0u; i < pico_rgb_keypad.NUM_KEYS; i++) {
            bool state = led_states & (1 << i);
            if(state) {
                pico_rgb_keypad.illuminate(i, 0xaa, 0x33, 0xff);
            } else {
                pico_rgb_keypad.illuminate(i, 0x00, 0x00, 0x00);
            }
        }

        last_buttons = current_buttons;

        pico_rgb_keypad.update();
        sleep_ms(100);
    }
}
```

## Reference

### Constants

#### Dimensions

Pico RGB Keypad includes the constants `WIDTH`, `HEIGHT` and `NUM_PAD` (width * height) so you can avoid these creeping in as magic numbers in your code.

For example, you might iterate through all pads like so:

```c++
for(auto y = 0u; y < pico_rgb_keypad.HEIGHT; y++) {
    for(auto x = 0u; x < pico_rgb_keypad.WIDTH; x++) {
        pico_rgb_keypad.illuminate(x, y, 255, 0, 255);
    }
}
```

### Functions

#### set_brightness

```c++
void set_brightness(float brightness);
```

Sets the 5-bit global brightness value for each LED.

#### illuminate

```c++
void illuminate(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
void illuminate(uint8_t i, uint8_t r, uint8_t g, uint8_t b);
```

You can specify a single keypad key by its `x,y` coordinates, eg:

```c++
pico_rgb_keypad.illuminate(x, y, 255, 0, 255);
```

Or its index, eg:

```c++
pico_rgb_keypad.illuminate(index, 255, 0, 255);
```

#### clear

```c++
void clear();
```

Clears the illumination states of all keys, setting them to `0,0,0`, this is equivilent to:

```c++
for(auto i = 0u; i < pico_rgb_keypad.NUM_PADS; i++) {
    pico_rgb_keypad.illuminate(i, 0, 0, 0);
}
```

#### get_button_states

```c++
uint16_t get_button_states();
```

Reads a 16-bit integer from the i2c IO expander connected to the buttons.

The button states are not debounced, you can handle button changes by introducing a small delay between button reads, keeping the last button state and using bitwise operations to get a mask of changed buttons and, subsequently, figured out which have been pressed and/or released, eg:

```c++
current_buttons = get_button_states();

// XOR the previous and current button states. 
// This gives us a mask of changes, since:
// 0 ^ 0 = 0  -- was 0, now 0, no change
// 0 ^ 1 = 1  -- was 0, now 1, changed!
// 1 ^ 0 = 1  -- was 1, now 0, changed!
// 1 ^ 1 = 0  -- was 1, now 1, no change
changed_buttons = current_buttons ^ last_buttons;

// AND the current button states with the change mask.
// This gives us all the pressed buttons, since:
// 0 & 0 = 0  -- is 0, unchanged, not pressed
// 0 & 1 = 0  -- is 0, has changed, must be released
// 1 & 0 = 0  -- is 1, unchanged, held
// 1 & 1 = 1  -- is 1, changed, pressed!
pressed_buttons = current_buttons & changed;

// AND the last button states with the change mask.
// This gives us all the released buttons, since:
// 0 & 0 = 0  -- was 0, unchanged, not released
// 0 & 1 = 0  -- was 0, changed, must be pressed
// 1 & 0 = 0  -- was 1, unchanged, held
// 1 & 1 = 1  -- was 1, changed, must be released!
released_buttons =  last_buttons & changed;

last_buttons = current_buttons;
```

#### update

```c++
void update();
```

Update the RGB keypad lights with your changes.