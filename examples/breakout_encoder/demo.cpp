#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include "common/pimoroni_common.hpp"

#include "breakout_encoder.hpp"

using namespace pimoroni;

static const uint8_t STEPS_PER_REV = 24;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutEncoder enc(&i2c);
bool toggle = false;

// HSV Conversion expects float inputs in the range of 0.00-1.00 for each channel
// Outputs are rgb in the range 0-255 for each channel
void from_hsv(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
  float i = floor(h * 6.0f);
  float f = h * 6.0f - i;
  v *= 255.0f;
  uint8_t p = v * (1.0f - s);
  uint8_t q = v * (1.0f - f * s);
  uint8_t t = v * (1.0f - (1.0f - f) * s);

  switch (int(i) % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  }
}

void count_changed(int16_t count) {
  printf("Count: %d\n", count);
  float h = (count % STEPS_PER_REV) / (float)STEPS_PER_REV;
  uint8_t r = 0, g = 0, b = 0;
  from_hsv(h, 1.0f, 1.0f, r, g, b);
  enc.set_led(r, g, b);
}

int main() {
#ifdef PICO_DEFAULT_LED_PIN
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif

  stdio_init_all();

  int16_t count = 0;
  if(enc.init()) {
    printf("Encoder found...\n");

    //enc.set_direction(BreakoutEncoder::DIRECTION_CCW);    // Uncomment this to flip the direction

    count_changed(count);
    enc.clear_interrupt_flag();

    while(true) {
#ifdef PICO_DEFAULT_LED_PIN
      gpio_put(PICO_DEFAULT_LED_PIN, toggle);
#endif
      toggle = !toggle;

      if(enc.get_interrupt_flag()) {
        count = enc.read();
        enc.clear_interrupt_flag();

        while(count < 0)
          count += STEPS_PER_REV;

        count_changed(count);
      }

      sleep_ms(20);
    }
  }
  else {
    printf("Encoder not found :'(\n");
#ifdef PICO_DEFAULT_LED_PIN
    gpio_put(PICO_DEFAULT_LED_PIN, true);
#endif
  }

  return 0;
}
