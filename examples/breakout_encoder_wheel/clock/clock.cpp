#include <math.h>
#include <string>
#include "pimoroni_i2c.hpp"
#include "breakout_encoder_wheel.hpp"
#include "time.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"

using namespace pimoroni;
using namespace encoderwheel;

/*
Displays a 12 hour clock on Encoder Wheel's LED ring, getting time from the system.
*/

// Datetime Indices
const uint HOUR = 4;
const uint MINUTE = 5;
const uint SECOND = 6;

// Constants
constexpr float BRIGHTNESS = 1.0f;  // The brightness of the LEDs
const uint UPDATES = 50;            // How many times the LEDs will be updated per second
const uint UPDATE_RATE_US = 1000000 / UPDATES;

// Handy values for the number of milliseconds
constexpr float MILLIS_PER_SECOND = 1000;
constexpr float MILLIS_PER_MINUTE = MILLIS_PER_SECOND * 60;
constexpr float MILLIS_PER_HOUR = MILLIS_PER_MINUTE * 60;
constexpr float MILLIS_PER_HALF_DAY = MILLIS_PER_HOUR * 12;

// Create a new BreakoutEncoderWheel
I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutEncoderWheel wheel(&i2c);


// Calculates the brightness of an LED based on its index and a position along the LED ring
int led_brightness_at(int led, float position, float half_width = 1.0f, float span = 1.0f) {
  float brightness = 0.0f;
  float upper = position + half_width;
  float lower = position - half_width;
  if(led > position)
    brightness = CLAMP((upper - led) / span, 0.0f, 1.0f);
  else
    brightness = CLAMP((led - lower) / span, 0.0f, 1.0f);

  // Handle the LEDs being in a circle
  if(upper >= NUM_LEDS)
    brightness = CLAMP(((upper - NUM_LEDS) - led) / span, brightness, 1.0f);
  else if(lower < 0.0f)
    brightness = CLAMP((led - (lower + NUM_LEDS)) / span, brightness, 1.0f);

  return (int)(brightness * BRIGHTNESS * 255);
}


int main() {
  stdio_init_all();

  // Start on Thursday 4th of May 2023 14:20:00
  datetime_t now = {
    .year  = 2023,
    .month = 05,
    .day   = 04,
    .dotw  = 4, // 0 is Sunday, so 4 is Thursday
    .hour  = 14,
    .min   = 20,
    .sec   = 00
  };

  // Start the RTC
  rtc_init();
  rtc_set_datetime(&now);

  // clk_sys is >2000x faster than clk_rtc, so datetime is not updated immediately when rtc_get_datetime() is called.
  // tbe delay is up to 3 RTC clock cycles (which is 64us with the default clock settings)
  sleep_us(64);

  // Attempt to initialise the encoder wheel
  if(wheel.init()) {
    // Loop forever
    while(true) {
      // Record the start time of this loop
      absolute_time_t start_time = get_absolute_time();

      // Get the current system time
      rtc_get_datetime(&now);

      // Convert the seconds, minutes, and hours into milliseconds (this is done to give a smoother animation, particularly for the seconds hand)
      uint sec_as_millis = (now.sec * MILLIS_PER_SECOND);
      uint min_as_millis = (now.min * MILLIS_PER_MINUTE) + sec_as_millis;
      uint hour_as_millis = ((now.hour % 12) * MILLIS_PER_HOUR) + min_as_millis;

      // Calculate the position on the LED ring that the, second, minute, and hour hands should be
      float sec_pos = MIN(sec_as_millis / MILLIS_PER_MINUTE, 1.0f) * NUM_LEDS;
      float min_pos = MIN(min_as_millis / MILLIS_PER_HOUR, 1.0f) * NUM_LEDS;
      float hour_pos = MIN(hour_as_millis / MILLIS_PER_HALF_DAY, 1.0f) * NUM_LEDS;

      for(int i = 0; i < NUM_LEDS; i++) {
        // Turn on the LEDs close to the position of the current second, minute, and hour
        int r = led_brightness_at(i, sec_pos);
        int g = led_brightness_at(i, min_pos);
        int b = led_brightness_at(i, hour_pos);
        wheel.set_rgb(i, r, g, b);
      }
      wheel.show();

      // Sleep until the next update, accounting for how long the above operations took to perform
      sleep_until(delayed_by_us(start_time, UPDATE_RATE_US));
    }
  }

  return 0;
}