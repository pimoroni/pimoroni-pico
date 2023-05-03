#include <math.h>
#include <string>
#include "breakout_encoder_wheel.hpp"
#include "time.h"

using namespace pimoroni;
using namespace encoderwheel;

/*
Display a circular stop-watch on the Encoder Wheel's LED ring.

Press the centre button to start the stopwatch, then again to pause and resume.
*/

// Constants
constexpr float BRIGHTNESS = 1.0f;              // The brightness of the LEDs
const uint UPDATES = 50;                        // How many times the LEDs will be updated per second
const uint MINUTE_UPDATES = UPDATES * 60;       // How many times the LEDs will be updated per minute
const uint HOUR_UPDATES = MINUTE_UPDATES * 60;  // How many times the LEDs will be updated per hour
const uint UPDATE_RATE_US = 1000000 / UPDATES;

constexpr float IDLE_PULSE_MIN = 0.2f;          // The brightness (between 0.0 and 1.0) that the idle pulse will go down to
constexpr float IDLE_PULSE_MAX = 0.5f;          // The brightness (between 0.0 and 1.0) that the idle pulse will go up to
constexpr float IDLE_PULSE_TIME = 2.0f;         // The time (in seconds) to perform a complete idle pulse
constexpr uint UPDATES_PER_PULSE = IDLE_PULSE_TIME * UPDATES;

// The state constants used for program flow
enum State {
  IDLE = 0,
  COUNTING,
  PAUSED
};

// Create a new BreakoutEncoderWheel
I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutEncoderWheel wheel(&i2c);

// Variables
State state = IDLE;
uint idle_update = 0;
uint second_update = 0;
uint minute_update = 0;
uint hour_update = 0;
bool last_centre_pressed = false;


int main() {
  stdio_init_all();

  // Attempt to initialise the encoder wheel
  if(wheel.init()) {
    // Record the current time
    absolute_time_t current_time = get_absolute_time();

    // Run the update loop forever
    while(true) {

      // Record the start time of this loop
      absolute_time_t start_time = get_absolute_time();

      // Read whether or not the wheen centre has been pressed
      bool centre_pressed = wheel.pressed(CENTRE);
      if(centre_pressed && centre_pressed != last_centre_pressed) {
        switch(state) {
          case IDLE:        // If we're currently idle, switch to counting
            second_update = 0;
            minute_update = 0;
            hour_update = 0;
            state = COUNTING;
            break;
          case COUNTING:    // If we're counting, switch to paused
            state = PAUSED;
            break;
          case PAUSED:      // If we're paused, switch back to counting
            state = COUNTING;
        }
      }
      last_centre_pressed = centre_pressed;

      switch(state) {
        // If we're idle, perform a pulsing animation to show the stopwatch is ready to go
        case IDLE:
          {
            float percent_along = MIN((float)idle_update / (float)UPDATES_PER_PULSE, 1.0f);
            float brightness = ((cosf(percent_along * M_PI * 2.0f) + 1.0f) / 2.0f) * ((IDLE_PULSE_MAX - IDLE_PULSE_MIN)) + IDLE_PULSE_MIN;
            // Update all the LEDs
            for(int i = 0; i < NUM_LEDS; i++) {
              wheel.set_hsv(i, 0.0, 0.0, brightness);
            }
            wheel.show();

            // Advance to the next update, wrapping around to zero if at the end
            idle_update += 1;
            if(idle_update >= UPDATES_PER_PULSE) {
              idle_update = 0;
            }
          }
          break;

        // If we're counting, perform the stopwatch animation
        case COUNTING:
          {
            // Calculate how many LED channels should light, as a proportion of a second, minute, and hour
            float r_to_light = MIN((float)second_update / UPDATES, 1.0f) * 24.0f;
            float g_to_light = MIN((float)minute_update / MINUTE_UPDATES, 1.0f) * 24.0f;
            float b_to_light = MIN((float)hour_update / HOUR_UPDATES, 1.0f) * 24.0f;

            // Set each LED, such that ones below the current time are fully lit, ones after
            // are off, and the one at the transition is at a percentage of the brightness
            for(int i = 0; i < NUM_LEDS; i++) {
              int r = (int)(CLAMP(r_to_light - i, 0.0f, 1.0f) * BRIGHTNESS * 255.0f);
              int g = (int)(CLAMP(g_to_light - i, 0.0f, 1.0f) * BRIGHTNESS * 255.0f);
              int b = (int)(CLAMP(b_to_light - i, 0.0f, 1.0f) * BRIGHTNESS * 255.0f);
              wheel.set_rgb(i, r, g, b);
            }
            wheel.show();

            // Advance the second updates count, wrapping around to zero if at the end
            second_update += 1;
            if(second_update >= UPDATES) {
              second_update = 0;
            }

            // Advance the minute updates count, wrapping around to zero if at the end
            minute_update += 1;
            if(minute_update >= MINUTE_UPDATES) {
              minute_update = 0;
            }

            // Advance the hour updates count, wrapping around to zero if at the end
            hour_update += 1;
            if(hour_update >= HOUR_UPDATES) {
              hour_update = 0;
            }
          }
          break;

        case PAUSED:
          // Do nothing
          break;
      }

      // Sleep until the next update, accounting for how long the above operations took to perform
      current_time = delayed_by_us(start_time, UPDATE_RATE_US);
      sleep_until(current_time);
    }
  }

  return 0;
}