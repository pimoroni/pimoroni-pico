#include <stdio.h>
#include <math.h>
#include <cstdint>

#include "pico/stdlib.h"

#include "servo2040.hpp"

#include "common/pimoroni_common.hpp"
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
using namespace servo;

// Set how many LEDs you have
const uint N_LEDS = 6;

// The speed that the LEDs will start cycling at
const uint DEFAULT_SPEED = 10;

// How many times the LEDs will be updated per second
const uint UPDATES = 60;

// WS28X-style LEDs with a single signal line. AKA NeoPixel
WS2812 led_bar(N_LEDS, pio0, 0, servo2040::LED_DAT);


Button user_sw(servo2040::USER_SW, Polarity::ACTIVE_LOW, 0);

uint count = 0;
uint servo_seq = 0;
int main() {
  stdio_init_all();

  led_bar.start(UPDATES);

  sleep_ms(5000);

  MultiPWM pwms(pio1, 0, 0b111111111111111);

  int speed = DEFAULT_SPEED;
  float offset = 0.0f;

  while(true) {
    bool sw_pressed = user_sw.read();

    if(sw_pressed) {
      speed = DEFAULT_SPEED;
    }
    speed = std::min((int)255, std::max((int)1, speed));

    offset += float(speed) / 2000.0f;

    for(auto i = 0u; i < led_bar.num_leds; ++i) {
      float hue = float(i) / led_bar.num_leds;
      led_bar.set_hsv(i, hue + offset, 1.0f, 0.5f);
    }

    count++;
    if(count >= 1000) {
      count = 0;

      pwms.start(servo_seq);
      servo_seq++;
      if(servo_seq >= 4)
        servo_seq = 0;

      pwms.set_servo_duty(1, 1000);
      pwms.apply_servo_duty();
    }

    //pwms.update(true);

    // Sleep time controls the rate at which the LED buffer is updated
    // but *not* the actual framerate at which the buffer is sent to the LEDs
    sleep_ms(1000 / UPDATES);
  }
}
