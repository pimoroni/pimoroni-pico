#include <stdio.h>
#include <math.h>
#include <cstdint>

#include "pico/stdlib.h"

#include "plasma2040.hpp"

#include "common/pimoroni_common.hpp"
#include "breakout_msa301.hpp"
#include "rgbled.hpp"
#include "button.hpp"

/*
A simple balancing game, where you use the MSA301 accelerometer to line up a band with a goal on the strip.
This can either be done using:
- Angle mode: Where position on the strip directly matches the accelerometer's angle
- Velocity mode: Where tilting the accelerometer changes the speed the band moves at
When the goal position is reached, a new position is randomly selected

Press "A" to change the game mode.
Press "B" to start or stop the game mode.
Press "Boot" to invert the direction of the accelerometer tilt
*/

using namespace pimoroni;
using namespace plasma;

// Set how many LEDs you have
const uint N_LEDS = 30;

// How many times the LEDs will be updated per second
const uint UPDATES = 60;

// The sensitivity of the accelerometer input
constexpr float ANGLE_SENSITIVITY = 0.05f;
constexpr float VELOCITY_SENSITIVITY = 0.2f / UPDATES;

// The band colour hues to show in Angle mode
constexpr float ANGLE_MODE_GOAL_HUE = 0.333f;
constexpr float ANGLE_MODE_EDGE_HUE = 0.0f;

// The band colour hues to show in Velocity mode
constexpr float VELOCITY_MODE_GOAL_HUE = 0.667f;
constexpr float VELOCITY_MODE_EDGE_HUE = 1.0f;

// The width and colour settings for the band
constexpr float BAND_PIXEL_WIDTH = 2.0f;
constexpr float BAND_SATURATION = 1.0f;
constexpr float BAND_IN_GOAL_SATURATION = 0.5f;
constexpr float BAND_BRIGHTNESS = 1.0f;

// The width and colour settings for the goal
// Goal should be wider than the band by a small amount
constexpr float GOAL_PIXEL_WIDTH = BAND_PIXEL_WIDTH + 2.0f;
constexpr float GOAL_BRIGHTNESS = 0.1f;

// The percentage of the new angle (between 0.0 and 1.0) to apply to the last angle
// Has the effect of smoothing out the reading, at the cost of making it slower to react
constexpr float SMOOTHING_FACTOR = 0.1f;


// Pick *one* LED type by uncommenting the relevant line below:

// APA102-style LEDs with Data/Clock lines. AKA DotStar
//APA102 led_strip(N_LEDS, pio0, 0, plasma2040::DAT, plasma2040::CLK);

// WS28X-style LEDs with a single signal line. AKA NeoPixel
WS2812 led_strip(N_LEDS, pio0, 0, plasma2040::DAT);


Button user_sw(plasma2040::USER_SW, Polarity::ACTIVE_LOW, 0);
Button button_a(plasma2040::BUTTON_A, Polarity::ACTIVE_LOW, 0);
Button button_b(plasma2040::BUTTON_B, Polarity::ACTIVE_LOW, 0);

RGBLED led(plasma2040::LED_R, plasma2040::LED_G, plasma2040::LED_B);

I2C i2c(BOARD::PICO_EXPLORER);
BreakoutMSA301 msa(&i2c);

enum LEVEL_MODE {
  ANGLE,
  VELOCITY
};

// Maps a value from one range to another
float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Shows a band and goal with the given widths at the positions on the strip
void colour_band(float centre_position, float width, float goal_position, float goal_width, float hue) {
  if((centre_position >= 0.0f) && (width > 0.0) && (goal_width > 0.0)) {
    float band_pixels_start = centre_position - (width / 2);
    float band_pixels_end = centre_position + (width / 2);

    float goal_pixels_start = goal_position - (goal_width / 2);
    float goal_pixels_end = goal_position + (goal_width / 2);

    // Go through each led in the strip
    uint i2;
    float saturation, brightness, sat, val;
    for(uint i = 0; i < led_strip.num_leds; i++) {
      // Set saturation and brightness values for if the led is inside or outside of the goal
      saturation = BAND_SATURATION;
      brightness = 0.0f;
      if((i >= goal_pixels_start) && (i < goal_pixels_end)) {
        saturation = BAND_IN_GOAL_SATURATION;
        brightness = GOAL_BRIGHTNESS;
      }

      i2 = i + 1;
      if(i2 <= band_pixels_end) {
        if(i2 <= band_pixels_start) {
          // Outside of the band
          led_strip.set_hsv(i, hue, 0.0, brightness);
        }
        else if(i <= band_pixels_start) {
          // Transition into the band
          val = map(band_pixels_start, (float)i, (float)i2, BAND_BRIGHTNESS, brightness);
          sat = map(band_pixels_start, (float)i, (float)i2, BAND_SATURATION, saturation);
          led_strip.set_hsv(i, hue, sat, val);
        }
        else {
          // Inside the band
          led_strip.set_hsv(i, hue, 1.0, 1.0);
        }
      }
      else if(i <= band_pixels_end) {
        // Transition out of the band
        val = map(band_pixels_end, (float)i, (float)i2, brightness, BAND_BRIGHTNESS);
        sat = map(band_pixels_end, (float)i, (float)i2, saturation, BAND_SATURATION);
        led_strip.set_hsv(i, hue, sat, val);
      }
      else {
        // Outside of the band
        led_strip.set_hsv(i, hue, 0.0, brightness);
      }
    }
  }
}

int main() {
  stdio_init_all();

  led_strip.start(UPDATES);

  bool accel_detected = msa.init();

  float band_position = 0.0f;
  float goal_position = 0.0f;
  float measured_angle = 0.0f;
  bool invert = false;
  bool game_mode = false;

  LEVEL_MODE mode = LEVEL_MODE::ANGLE;
  while(true) {
    if(accel_detected) {
      // Read the x and y axes of the accelerometer
      float x = msa.get_x_axis();
      float y = msa.get_y_axis();

      // Convert those values to an angle in degrees, and invert if selected
      float new_measured_angle = (atan2(x, -y) * 180.0f) / M_PI;
      if(invert)
        new_measured_angle = -new_measured_angle;
      printf("Angle: %f deg\n", new_measured_angle);

      // Smooth out the measured angle
      measured_angle = ((new_measured_angle - measured_angle) * SMOOTHING_FACTOR) + measured_angle;

      float hue = 0.0;
      float position_diff;
      switch(mode) {
        case LEVEL_MODE::ANGLE:
          // Apply the measured angle directly to the band position, clamping it between -1 and +1
          band_position = measured_angle * ANGLE_SENSITIVITY;
          band_position = std::min(1.0f, std::max(-1.0f, band_position));

          // Convert the difference between the band and goal positions into a colour hue
          position_diff = std::min(abs(band_position - goal_position), 1.0f);
          hue = map(position_diff, 0.0f, 1.0f, ANGLE_MODE_GOAL_HUE, ANGLE_MODE_EDGE_HUE);
          break;

        case LEVEL_MODE::VELOCITY:
          // Apply the measured angle as a velocity to the band position, clamping it between -1 and +1
          band_position += measured_angle * VELOCITY_SENSITIVITY;
          band_position = std::min(1.0f, std::max(-1.0f, band_position));

          // Convert the difference between the band and goal positions into a colour hue
          position_diff = std::min(abs(band_position - goal_position), 1.0f);
          hue = map(position_diff, 0.0f, 1.0f, VELOCITY_MODE_GOAL_HUE, VELOCITY_MODE_EDGE_HUE);
          break;
      }

      // Convert the band and goal positions to positions on the LED strip
      float strip_band_position = map(band_position, -1.0f, 1.0f, 0.0f, float(led_strip.num_leds));
      float strip_goal_position = map(goal_position, -1.0f, 1.0f, 0.0f, float(led_strip.num_leds));

      // Draw the band and goal
      colour_band(strip_band_position, BAND_PIXEL_WIDTH, strip_goal_position, GOAL_PIXEL_WIDTH, hue);

      bool sw_pressed = user_sw.read();
      bool a_pressed = button_a.read();
      bool b_pressed = button_b.read();

      if(b_pressed)
        game_mode = !game_mode;

      if(sw_pressed)
        invert = !invert;

      switch(mode) {
        case ANGLE:
          if(game_mode)
            led.set_rgb(255, 255, 0);
          else
            led.set_rgb(0, 255, 0);
          if(a_pressed)
            mode = VELOCITY;
          break;

        case VELOCITY:
          if(game_mode)
            led.set_rgb(255, 0, 255);
          else
            led.set_rgb(0, 0, 255);
          if(a_pressed)
            mode = ANGLE;
          break;
      }

      if(game_mode) {
        // Check if the band is within the goal, and if so, set a new goal
        bool above_lower = strip_band_position >= strip_goal_position - (GOAL_PIXEL_WIDTH - BAND_PIXEL_WIDTH) / 2;
        bool below_upper = strip_band_position <= strip_goal_position + (GOAL_PIXEL_WIDTH - BAND_PIXEL_WIDTH) / 2;
        if(above_lower && below_upper)
          goal_position = map((float)rand(), 0.0f, (float)RAND_MAX, -1.0f, 1.0f);
      }
    }
    // Sleep time controls the rate at which the LED buffer is updated
    // but *not* the actual framerate at which the buffer is sent to the LEDs
    sleep_ms(1000 / UPDATES);
  }
}
