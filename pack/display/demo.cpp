#include <string.h>
#include <math.h>

#include "hardware/pwm.h"
#include "../../drivers/st7789/st7789.hpp"

using namespace pimoroni;

extern unsigned char image_tif[];
extern unsigned int image_tif_len;

enum pin {
  LED_R     =  6,
  LED_G     =  7,
  LED_B     =  8,
  BUTTON_A  = 12,
  BUTTON_B  = 13,
  BUTTON_X  = 14,
  BUTTON_Y  = 15
};


ST7789 screen;

void flip() {
  // write to screen
}


void pixel(int x, int y, uint16_t c) {
  x *= 2;
  y *= 2;
  screen.buffer[x + y * 240] = c;
  screen.buffer[x + 1 + y * 240] = c;
  screen.buffer[x + 1 + (y + 1) * 240] = c;
  screen.buffer[x + (y + 1) * 240] = c;
}


void rect(int x, int y, int w, int h, uint16_t c) {
  for(int rx = x; rx < x + w; rx++) {
    for(int ry = y; ry < y + h; ry++) {
      pixel(rx, ry, c);
    }
  }
}

uint8_t arrow[] = {
    0b00010000,
    0b00110000,
    0b01110000,
    0b11111111,
    0b11111111,
    0b01110000,
    0b00110000,
    0b00010000
  };

uint8_t tick[] = {
    0b00000000,
    0b00000010,
    0b00000111,
    0b01001110,
    0b11111100,
    0b00111000,
    0b00010000,
    0b00000000,
  };


void sprite(uint8_t *p, int x, int y, bool flip, uint16_t c) {
  for(int ay = 0; ay < 8; ay++) {
    uint8_t sl = p[ay];
    for(int ax = 0; ax < 8; ax++) {
      if(flip) {
        if((0b10000000 >> ax) & sl) {
          pixel(ax + x, ay + y, c);
        }
      }else{
        if((0b1 << ax) & sl) {
          pixel(ax + x, ay + y, c);
        }
      }
    }
  }
}

int main() {

  //screen.buffer = (uint16_t*)(image_tif + 292);
  screen.init_240x135();

  screen.set_backlight(100);

  // rgb led to a soft white
  pwm_config cfg1 = pwm_get_default_config();
  pwm_config_set_wrap(&cfg1, 255);
  pwm_config_set_output_polarity(&cfg1, true, true);
  pwm_init(pwm_gpio_to_slice_num(pin::LED_R), &cfg1, true);
  gpio_set_function(pin::LED_R, GPIO_FUNC_PWM);
  pwm_set_gpio_level(pin::LED_R, 50);

  pwm_config cfg2 = pwm_get_default_config();
  pwm_config_set_wrap(&cfg2, 255);
  pwm_config_set_output_polarity(&cfg2, true, true);
  pwm_init(pwm_gpio_to_slice_num(pin::LED_G), &cfg2, true);
  gpio_set_function(pin::LED_G, GPIO_FUNC_PWM);
  pwm_set_gpio_level(pin::LED_G, 50);

  pwm_config cfg3 = pwm_get_default_config();
  pwm_config_set_wrap(&cfg3, 255);
  pwm_config_set_output_polarity(&cfg3, true, true);
  pwm_init(pwm_gpio_to_slice_num(pin::LED_B), &cfg3, true);
  gpio_set_function(pin::LED_B, GPIO_FUNC_PWM);
  pwm_set_gpio_level(pin::LED_B, 50);

  // setup button inputs
  gpio_set_function(pin::BUTTON_A, GPIO_FUNC_SIO); gpio_set_dir(pin::BUTTON_A, GPIO_IN); gpio_pull_up(pin::BUTTON_A);
  gpio_set_function(pin::BUTTON_B, GPIO_FUNC_SIO); gpio_set_dir(pin::BUTTON_B, GPIO_IN); gpio_pull_up(pin::BUTTON_B);
  gpio_set_function(pin::BUTTON_X, GPIO_FUNC_SIO); gpio_set_dir(pin::BUTTON_X, GPIO_IN); gpio_pull_up(pin::BUTTON_X);
  gpio_set_function(pin::BUTTON_Y, GPIO_FUNC_SIO); gpio_set_dir(pin::BUTTON_Y, GPIO_IN); gpio_pull_up(pin::BUTTON_Y);

  uint16_t white = screen.pen(255, 255, 255);
  uint16_t black = screen.pen(0, 0, 0);
  uint16_t red = screen.pen(255, 0, 0);
  uint16_t green = screen.pen(0, 255, 0);
  uint16_t dark_grey = screen.pen(20, 40, 60);
  uint16_t dark_green = screen.pen(10, 100, 10);
  uint16_t blue = screen.pen(0, 0, 255);

  bool a_pressed = false;
  bool b_pressed = false;
  bool x_pressed = false;
  bool y_pressed = false;

  uint32_t i = 0;
  while(true) {
    //rect(0, 0, 120, 67, dark_grey);
    //memset(framebuffer, 0x05, 240 * 135 * 2);

    // we need to be careful to clamp the input to
    // sin here because otherwise the fast float
    // library will choke
    float sin_step = fmod(i / 5.0f, M_PI * 2.0f);
    int bounce = sin(sin_step) * 4.0f;


    float led_step = fmod(i / 20.0f, M_PI * 2.0f);
    int r = (sin(led_step) * 25.0f) + 25.0f;
    // int g = (sin(led_step + 2.09f) * 25.0f) + 25.0f;
    // int b = (sin(led_step + 4.18f) * 25.0f) + 25.0f;
    // pwm_set_gpio_level(pin::LED_R, r);
    // pwm_set_gpio_level(pin::LED_G, g);
    // pwm_set_gpio_level(pin::LED_B, b);

    pwm_set_gpio_level(pin::LED_R, r);
    pwm_set_gpio_level(pin::LED_G, r / 1.2f);
    pwm_set_gpio_level(pin::LED_B, r);

    if(!gpio_get(pin::BUTTON_A)) { a_pressed = true; }
    if(!gpio_get(pin::BUTTON_B)) { b_pressed = true; }
    if(!gpio_get(pin::BUTTON_X)) { x_pressed = true; }
    if(!gpio_get(pin::BUTTON_Y)) { y_pressed = true; }

    if(a_pressed) {
      rect(0, 0, 18, 18, dark_green);
      sprite(tick, 5, 5, true, green);
    }else{
      sprite(arrow, 10 + bounce, 10, true, white);
    }
    if(b_pressed) {
      rect(0, 49, 18, 18, dark_green);
      sprite(tick, 5, 54, true, green);
    }else{
      sprite(arrow, 10 - bounce, 50, true, white);
    }


    if(x_pressed) {
      rect(102, 0, 18, 18, dark_green);
      sprite(tick, 107, 5, true, green);
    }else{
      sprite(arrow, 102 - bounce, 10, false, white);
    }

    if(y_pressed) {
      rect(102, 49, 18, 18, dark_green);
      sprite(tick, 107, 54, true, green);
    }else{
      sprite(arrow, 102 + bounce, 50, false, white);
    }

    // update screen
    // switch st7789 into data mode
    screen.update();

    i++;
  }

    return 0;
}
