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
    - [Motors](#motors)
  - [Functions](#functions)
    - [Motors](#motors-1)
    - [Analog](#analog)
    - [Buzzer](#buzzer)
    - [Buttons](#buttons-1)
  - [ST7789](#st7789)


## Example Program

The following example shows how to set up all of Pico Explorers functions:

```c++
#include "pico_explorer.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "button.hpp"
#include "motor.hpp"
#include "analog.hpp"

using namespace pimoroni;

// Display driver
ST7789 st7789(PicoExplorer::WIDTH, PicoExplorer::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));

// Graphics library - in RGB332 mode you get 256 colours and optional dithering for ~56K RAM.
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

// Buttons
Button button_a(PicoExplorer::A);
Button button_b(PicoExplorer::B);
Button button_x(PicoExplorer::X);
Button button_y(PicoExplorer::Y);

// Motors
Motor motor1(PicoExplorer::MOTOR1_PINS);
Motor motor2(PicoExplorer::MOTOR2_PINS);

// Analog
Analog adc0(PicoExplorer::ADC0_PIN);
Analog adc1(PicoExplorer::ADC1_PIN);
Analog adc2(PicoExplorer::ADC2_PIN);


int main() {
  motor1.init();
  motor2.init();

  while(true) {
    // update screen
    st7789.update(&graphics);
  }
}
```

## Reference

### PicoGraphics

Pico Explorer uses our Pico Graphics library to draw graphics and text. For more information [read the Pico Graphics function reference.](../pico_graphics/README.md#function-reference).

Please note that the backlight on Pico Explorer is not dimmable (we needed the pins to hook up other functions) so the `set_backlight` function is not available for this board.

### Constants

#### Buttons

The four buttons, A, B, X and Y have corresponding constants set to their respective GPIO pins.

```c++
PicoExplorer::A = 12;
PicoExplorer::B = 13;
PicoExplorer::X = 14;
PicoExplorer::Y = 15;
```

#### ADC Channels

The three ADC channels are defined as `ADC0_PIN`, `ADC1_PIN` and `ADC2_PIN`, and should be used with an instance of the `Analog` class:

```c++
PicoExplorer::ADC0_PIN = 26;
PicoExplorer::ADC1_PIN = 27;
PicoExplorer::ADC2_PIN = 28;
```

#### GPIO

The 8 general purpose IO pins on the lower Pico Explorer are GP0 through GP7, we've created constants for you to identify them easily.

```c++
PicoExplorer::GP0 = 0;
PicoExplorer::GP1 = 1;
PicoExplorer::GP2 = 2;
PicoExplorer::GP3 = 3;
PicoExplorer::GP4 = 4;
PicoExplorer::GP5 = 5;
PicoExplorer::GP6 = 6;
PicoExplorer::GP7 = 7;
```

You should use Pico SDK's `gpio_get` to read a pin, eg:

```c++
bool pin_state = gpio_get(pico_explorer.GP0);
```

#### Motors

The two motor channels are defined as pin pairs, and should be used with an instance of the `Motor` class:

```c++
  PicoExplorer::MOTOR1_PINS = {9, 8};
  PicoExplorer::MOTOR2_PINS = {11, 10};
```

### Functions

#### Motors

Pico Explorer uses our `Motor` library to drive motors. Motors are driven by PWM via an onboard DRV8833. Constants are provided for both motors, so setup is as easy as:

```c++
#include "motor.hpp"

using namespace motor;

Motor motor1(PicoExplorer::MOTOR1_PINS);
Motor motor2(PicoExplorer::MOTOR2_PINS);
```

You should also init your motors to set up the required GPIO/PWM on their pins:

```c++
motor1.init();
motor2.init();
```

Speed should be given as a float between `-1.0` and `1.0`, eg:

```c++
motor1.speed(1.0)  // Full-speed forward
motor1.speed(-1.0) // Full-speed backward
```

And to stop the motor:

```c++
motor1.stop()
```

#### Analog

```c++
float get_adc(uint8_t channel);
```

Pico Explorer's ADC channels are connected to Pico's ADC-capable pins 26, 27 and 28 which correspond to channels 0, 1 and 2 respectively.

Constants are supplied for these in the PicoExplorer library, so you can create an Analog instance for each:

```c++
#include "analog.hpp"
Analog adc0(PicoExplorer::ADC0_PIN);
Analog adc1(PicoExplorer::ADC1_PIN);
Analog adc2(PicoExplorer::ADC2_PIN);
```

And read a voltage with:

```c++
adc0.read_voltage();
```

#### Buzzer

Note: You must bridge the pin you use over to the `AUDIO` pin on the Pico Explorer header in order to drive the onboard Piezo.

TODO document buzzer

#### Buttons

```c++
#include "button.hpp"
Button button_a(PicoExplorer::A);
Button button_b(PicoExplorer::B);
Button button_x(PicoExplorer::X);
Button button_y(PicoExplorer::Y);
```

### ST7789

Pico Explorer uses the ST7789 display driver to handle the LCD. For more information [read the ST7789 README.](../../drivers/st7789/README.md).