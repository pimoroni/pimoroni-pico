#include "pico/stdlib.h"

#include "servo2040.hpp"
#include "button.hpp"

/*
Displays a rotating rainbow pattern on the Servo 2040's onboard LED bar.

Press "Boot" to exit the program.
*/

using namespace plasma;
using namespace servo;

// The speed that the LEDs will cycle at
const uint SPEED = 5;

// The brightness of the LEDs
constexpr float BRIGHTNESS = 0.4f;

// How many times the LEDs will be updated per second
const uint UPDATES = 50;


// Create the LED bar, using PIO 1 and State Machine 0
WS2812 led_bar(servo2040::NUM_LEDS, pio1, 0, servo2040::LED_DATA);

// Create the user button
Button user_sw(servo2040::USER_SW);


int main() {
  stdio_init_all();

  // Start updating the LED bar
  led_bar.start();

  float offset = 0.0f;

  // Make rainbows until the user button is pressed
  while(!user_sw.raw()) {

    offset += (float)SPEED / 1000.0f;

    // Update all the LEDs
    for(auto i = 0u; i < servo2040::NUM_LEDS; i++) {
      float hue = (float)i / (float)servo2040::NUM_LEDS;
      led_bar.set_hsv(i, hue + offset, 1.0f, BRIGHTNESS);
    }

    sleep_ms(1000 / UPDATES);
  }

  // Turn off the LED bar
  led_bar.clear();

  // Sleep a short time so the clear takes effect
  sleep_ms(100);
}
