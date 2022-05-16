#include "pico/stdlib.h"

#include "motor2040.hpp"
#include "button.hpp"

/*
Displays a rotating rainbow pattern on the Motor 2040's onboard LED.

Press "Boot" to exit the program.
*/

using namespace plasma;
using namespace motor;

// The speed that the LED will cycle at
const uint SPEED = 5;

// The brightness of the LED
constexpr float BRIGHTNESS = 0.4f;

// How many times the LED will be updated per second
const uint UPDATES = 50;


// Create the LED, using PIO 1 and State Machine 0
WS2812 led(motor2040::NUM_LEDS, pio1, 0, motor2040::LED_DATA);

// Create the user button
Button user_sw(motor2040::USER_SW);


int main() {
  stdio_init_all();

  // Start updating the LED
  led.start();

  float hue = 0.0f;

  // Make rainbows until the user button is pressed
  while(!user_sw.raw()) {

    hue += (float)SPEED / 1000.0f;

    // Update the LED
    led.set_hsv(0, hue, 1.0f, BRIGHTNESS);

    sleep_ms(1000 / UPDATES);
  }

  // Turn off the LED
  led.clear();

  // Sleep a short time so the clear takes effect
  sleep_ms(100);
}
