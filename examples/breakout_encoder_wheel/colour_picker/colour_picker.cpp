#include <math.h>
#include <string>
#include "pimoroni_i2c.hpp"
#include "breakout_encoder_wheel.hpp"
#include "time.h"

using namespace pimoroni;
using namespace encoderwheel;

/*
Create a colour wheel on the Encoder Wheel's LED ring, and use all functions of the wheel to interact with it.

Rotate the wheel to select a Hue
Press the up direction to increase Brightness
Press the down direction to decrease Brightness
Press the left direction to decrease Saturation
Press the right direction to increase Saturation
Press the centre to hide the selection marker
*/

// Constants
constexpr float BRIGHTNESS_STEP = 0.02f;    // How much to increase or decrease the brightness each update
constexpr float SATURATION_STEP = 0.02f;    // How much to increase or decrease the saturation each update
const uint UPDATES = 50;                    // How many times to update the LEDs per second
const uint UPDATE_RATE_US = 1000000 / UPDATES;

// Create a new BreakoutEncoderWheel
I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutEncoderWheel wheel(&i2c);

// Variables
float brightness = 1.0f;
float saturation = 1.0f;
int position = 0;
bool changed = true;
bool last_centre_pressed = false;

// Struct for storing RGB values
struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    Pixel() : r(0), g(0), b(0) {};
    Pixel(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {};
};

// Basic function to convert Hue, Saturation and Value to an RGB colour
Pixel hsv_to_rgb(float h, float s, float v) {
  if(h < 0.0f) {
    h = 1.0f + fmodf(h, 1.0f);
  }

  int i = int(h * 6);
  float f = h * 6 - i;

  v = v * 255.0f;

  float sv = s * v;
  float fsv = f * sv;

  auto p = uint8_t(-sv + v);
  auto q = uint8_t(-fsv + v);
  auto t = uint8_t(fsv - sv + v);

  uint8_t bv = uint8_t(v);

  switch (i % 6) {
    default:
    case 0: return Pixel(bv, t, p);
    case 1: return Pixel(q, bv, p);
    case 2: return Pixel(p, bv, t);
    case 3: return Pixel(p, q, bv);
    case 4: return Pixel(t, p, bv);
    case 5: return Pixel(bv, p, q);
  }
}

// Simple function to clamp a value between 0.0 and 1.0
float clamp01(float value) {
  return MAX(MIN(value, 1.0f), 0.0f);
}


int main() {
  stdio_init_all();

  // Attempt to initialise the encoder wheel
  if(wheel.init()) {
    // Loop forever
    while(true) {
      // Record the start time of this loop
      absolute_time_t start_time = get_absolute_time();

      // If up is pressed, increase the brightness
      if(wheel.pressed(UP)) {
        brightness += BRIGHTNESS_STEP;
        changed = true;   // Trigger a change
      }

      // If down is pressed, decrease the brightness
      if(wheel.pressed(DOWN)) {
        brightness -= BRIGHTNESS_STEP;
        changed = true;   // Trigger a change
      }

      // If right is pressed, increase the saturation
      if(wheel.pressed(RIGHT)) {
        saturation += SATURATION_STEP;
        changed = true;   // Trigger a change
      }

      // If left is pressed, decrease the saturation
      if(wheel.pressed(LEFT)) {
        saturation -= SATURATION_STEP;
        changed = true;   // Trigger a change
      }

      // Limit the brightness and saturation between 0.0 and 1.0
      brightness = clamp01(brightness);
      saturation = clamp01(saturation);

      // Check if the encoder has been turned
      if(wheel.delta() != 0) {
        // Update the position based on the count change
        position = wheel.step();
        changed = true;   // Trigger a change
      }

      // If centre is pressed, trigger a change
      bool centre_pressed = wheel.pressed(CENTRE);
      if(centre_pressed != last_centre_pressed) {
        changed = true;
      }
      last_centre_pressed = centre_pressed;

      // Was a change triggered?
      if(changed) {
        // Print the colour at the current hue, saturation, and brightness
        Pixel pixel = hsv_to_rgb((float)position / NUM_LEDS, saturation, brightness);
        printf("Colour Code = #%02x%02x%02x\n", pixel.r, pixel.g, pixel.b);

        // Set the LED at the current position to either the actual colour,
        // or an inverted version to show a "selection marker"
        if(centre_pressed)
            wheel.set_rgb(position, pixel.r, pixel.g, pixel.b);
        else
            wheel.set_rgb(position, 255 - pixel.r, 255 - pixel.g, 255 - pixel.b);

        // Set the LEDs below the current position
        for(int i = 0; i < position; i++) {
          wheel.set_hsv(i, (float)i / NUM_LEDS, saturation, brightness);
        }

        // Set the LEDs after the current position
        for(int i = position + 1; i < NUM_LEDS; i++) {
          wheel.set_hsv(i, (float)i / NUM_LEDS, saturation, brightness);
        }
        wheel.show();
        changed = false;
      }

      // Sleep until the next update, accounting for how long the above operations took to perform
      sleep_until(delayed_by_us(start_time, UPDATE_RATE_US));
    }
  }

  return 0;
}