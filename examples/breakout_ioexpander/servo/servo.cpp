#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>

#include "breakout_ioexpander.hpp"

using namespace pimoroni;

static const uint8_t IOE_SERVO_PIN = 1;

// Settings to produce a 50Hz output from the 24MHz clock.
// 24,000,000 Hz / 8 = 3,000,000 Hz
// 3,000,000 Hz / 60,000 Period = 50 Hz
static const uint8_t DIVIDER = 8;
static const uint16_t PERIOD = 60000;
static constexpr float CYCLE_TIME = 5.0f;
static constexpr float SERVO_RANGE = 1000.0f;   // Between 1000 and 2000us (1-2ms)

BreakoutIOExpander ioe(0x18);
bool toggle = false;

int main() {
#ifdef PICO_DEFAULT_LED_PIN
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif

  stdio_init_all();

  if(ioe.init()) {
    printf("IOExpander found...\n");

    ioe.set_pwm_period(PERIOD);
    ioe.set_pwm_control(DIVIDER);

    ioe.set_mode(IOE_SERVO_PIN, IOExpander::PIN_PWM);

    while(true) {
#ifdef PICO_DEFAULT_LED_PIN
      gpio_put(PICO_DEFAULT_LED_PIN, toggle);
#endif
      toggle = !toggle;

      absolute_time_t at = get_absolute_time();
      float t = to_us_since_boot(at) / 1000000.0f;
      float s = sinf((t * M_PI * 2.0f) / CYCLE_TIME) / 2.0f;
      float servo_us = 1500.0f + (s * SERVO_RANGE);

      float duty_per_microsecond = (float)PERIOD / (float)(20 * 1000);    // Default is 3 LSB per microsecond

      uint16_t duty_cycle = (uint16_t)(roundf(servo_us * duty_per_microsecond));
      printf("Cycle Time: %.2f, Pulse: %.1fus, Duty Cycle: %d\n", fmodf(t, CYCLE_TIME), servo_us, duty_cycle);
      ioe.output(IOE_SERVO_PIN, duty_cycle);

      sleep_ms(20);
    }
  }
  else {
    printf("IOExpander not found :'(\n");
#ifdef PICO_DEFAULT_LED_PIN
    gpio_put(PICO_DEFAULT_LED_PIN, true);
#endif
  }

  return 0;
}
