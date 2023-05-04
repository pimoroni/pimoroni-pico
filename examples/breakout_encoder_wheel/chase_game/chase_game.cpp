#include <math.h>
#include <string>
#include "pimoroni_i2c.hpp"
#include "breakout_encoder_wheel.hpp"
#include "time.h"

using namespace pimoroni;
using namespace encoderwheel;

/*
A simple alignment game. Use Encoder Wheel's rotary dial to align the coloured band
to the white goal. The closer to the goal, the greener your coloured band will be.
When you reach the goal, the goal will move to a new random position.
*/

// The band colour hues to show in Angle mode
constexpr float GOAL_HUE = 0.333f;
constexpr float FAR_HUE = 0.0f;

// The width and colour settings for the band
constexpr float BAND_WIDTH = 5.0f;
constexpr float BAND_SATURATION = 1.0f;
constexpr float BAND_IN_GOAL_SATURATION = 0.5f;
constexpr float BAND_BRIGHTNESS = 1.0f;

// The width and colour settings for the goal
// Goal should be wider than the band by a small amount
constexpr float GOAL_MARGIN = 1.0f;
constexpr float GOAL_WIDTH = BAND_WIDTH + (2.0f * GOAL_MARGIN);
constexpr float GOAL_BRIGHTNESS = 0.4f;

// Create a new BreakoutEncoderWheel
I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutEncoderWheel wheel(&i2c);

// Variables
float goal_position = 0.0f;
int16_t band_position = 0;


// Maps a value from one range to another
float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Shows a band and goal with the given widths at the positions on the strip
void colour_band(float centre_position, float width, float goal_position, float goal_width, float hue) {
  if(centre_position >= 0.0f && width > 0.0f && goal_width > 0.0) {
    float band_start = centre_position - (width / 2);
    float band_end = centre_position + (width / 2);
    float band_centre = centre_position;

    float goal_start = goal_position - (goal_width / 2);
    float goal_end = goal_position + (goal_width / 2);

    // Go through each led in the strip
    for(int i = 0; i < NUM_LEDS; i++) {
      // Set saturation and brightness values for if the led is inside or outside of the goal
      float saturation = BAND_SATURATION;
      float brightness = 0.0f;

      if(i >= goal_start && i < goal_end) {
        saturation = BAND_IN_GOAL_SATURATION;
        brightness = GOAL_BRIGHTNESS;
      }
      if(goal_end >= NUM_LEDS && i + NUM_LEDS < goal_end) {
        saturation = BAND_IN_GOAL_SATURATION;
        brightness = GOAL_BRIGHTNESS;
      }
      if(goal_start < 0 && i - NUM_LEDS >= goal_start) {
        saturation = BAND_IN_GOAL_SATURATION;
        brightness = GOAL_BRIGHTNESS;
      }

      float val = brightness;
      float sat = 0.0f;
      if(i >= band_start && i < band_end) {
        // Inside the band
        if(i < band_centre) {
          // Transition into the band
          val = map(i, band_centre, band_start, BAND_BRIGHTNESS, brightness);
          sat = map(i, band_centre, band_start, BAND_SATURATION, saturation);
        }
        else {
          val = map(i, band_centre, band_end, BAND_BRIGHTNESS, brightness);
          sat = map(i, band_centre, band_end, BAND_SATURATION, saturation);
        }
      }
      else if(band_end >= NUM_LEDS && i + NUM_LEDS < band_end && i < band_centre) {
        val = map(i + NUM_LEDS, band_centre, band_end, BAND_BRIGHTNESS, brightness);
        sat = map(i + NUM_LEDS, band_centre, band_end, BAND_SATURATION, saturation);
      }
      else if(band_start < 0 && i - NUM_LEDS >= band_start && i >= band_centre) {
        val = map(i - NUM_LEDS, band_centre, band_start, BAND_BRIGHTNESS, brightness);
        sat = map(i - NUM_LEDS, band_centre, band_start, BAND_SATURATION, saturation);
      }
      //else {
      // Outside of the band
      //}
      wheel.set_hsv(i, hue, sat, val);
    }
    wheel.show();
  }
}


int main() {
  stdio_init_all();

  // Attempt to initialise the encoder wheel
  if(wheel.init()) {
    // Loop forever
    while(true) {
      band_position = wheel.step();

      // Convert the difference between the band and goal positions into a colour hue
      float diff1, diff2;
      if(band_position > goal_position) {
        diff1 = band_position - goal_position;
        diff2 = (goal_position + NUM_LEDS) - band_position;
      }
      else {
        diff1 = goal_position - band_position;
        diff2 = (band_position + NUM_LEDS) - goal_position;
      }

      float position_diff = MIN(diff1, diff2);
      float hue = map(position_diff, 0, NUM_LEDS / 2.0f, GOAL_HUE, FAR_HUE);

      // Convert the band and goal positions to positions on the LED strip
      float strip_band_position = map(band_position, 0, NUM_LEDS, 0.0f, (float)NUM_LEDS);
      float strip_goal_position = map(goal_position, 0, NUM_LEDS, 0.0f, (float)NUM_LEDS);

      // Draw the band and goal
      colour_band(strip_band_position, BAND_WIDTH, strip_goal_position, GOAL_WIDTH, hue);

      // Check if the band is within the goal, and if so, set a new goal
      if(band_position >= goal_position - GOAL_MARGIN && band_position <= goal_position + GOAL_MARGIN)
        goal_position = rand() % NUM_LEDS;
      if(band_position >= NUM_LEDS && band_position + NUM_LEDS < goal_position + GOAL_MARGIN)
        goal_position = rand() % NUM_LEDS;
      if(goal_position - GOAL_MARGIN < 0 && band_position - NUM_LEDS >= goal_position + GOAL_MARGIN)
        goal_position = rand() % NUM_LEDS;
    }
  }

  return 0;
}