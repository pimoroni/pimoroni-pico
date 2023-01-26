#include <math.h>

#include "breakout_dotmatrix.hpp"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutDotMatrix display(&i2c);
bool led_toggle = false;

  //        Left Image       Padding    Right Image      Padding
uint8_t image[] = { 0, 0, 0, 0, 0,   0, 0, 0,   0, 1, 0, 1, 0,   0, 0, 0,
                    0, 1, 0, 1, 0,   0, 0, 0,   1, 0, 1, 0, 1,   0, 0, 0,
                    0, 1, 0, 1, 0,   0, 0, 0,   0, 1, 0, 1, 0,   0, 0, 0,
                    0, 0, 0, 0, 0,   0, 0, 0,   1, 0, 1, 0, 1,   0, 0, 0,
                    1, 0, 0, 0, 1,   0, 0, 0,   0, 1, 0, 1, 0,   0, 0, 0,
                    0, 1, 1, 1, 0,   0, 0, 0,   1, 0, 1, 0, 1,   0, 0, 0,
                    0, 0, 0, 0, 0,   0, 0, 0,   0, 1, 0, 1, 0,   0, 0, 0 };

static const uint8_t IMAGE_HEIGHT = 7;
static const uint8_t IMAGE_PADDING = 3; // 3 columns of padding
static const uint8_t IMAGE_WIDTH = floor(sizeof(image) / IMAGE_HEIGHT);
static const uint8_t ON_LEVEL = 1; //The value above which an image's pixel will be considered as on

uint8_t offset = 0;



int main() {
#ifdef PICO_DEFAULT_LED_PIN
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif

  display.init();

  while(true) {
    display.set_image(image, IMAGE_WIDTH, IMAGE_HEIGHT, offset, 0, true, false, ON_LEVEL, IMAGE_PADDING);
    display.show();

#ifdef PICO_DEFAULT_LED_PIN
    gpio_put(PICO_DEFAULT_LED_PIN, led_toggle);
#endif
    led_toggle = !led_toggle;

    sleep_ms(500);

    offset = (offset + 1) % IMAGE_WIDTH;
  }

  return 0;
}
