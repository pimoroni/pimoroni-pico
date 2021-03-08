#include "pico/stdlib.h"
#include "math.h"
#include "breakout_colourlcd160x80.hpp"

using namespace pimoroni;

uint16_t buffer[BreakoutColourLCD160x80::WIDTH * BreakoutColourLCD160x80::HEIGHT];
BreakoutColourLCD160x80 lcd(buffer);

 int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  lcd.init();
  lcd.set_backlight(255);

  Pen my_pen = lcd.create_pen(255, 0, 0);


  uint32_t t = 0;

  while(true) {
    uint16_t x = (sinf(float(t) / 100.0f) * 30) + 30;
    lcd.set_pen(0, 255, 0);
    lcd.clear();
    lcd.set_pen(255, 255, 0);
    lcd.rectangle(Rect(10, 10, 160-20, 80-20));

    lcd.set_pen(0, 0, 255);
    lcd.rectangle(Rect(10, 10, 30, 30));
    lcd.set_pen(255, 0, 0);
    lcd.rectangle(Rect(x, x, 20, 20));
    lcd.update();
    gpio_put(PICO_DEFAULT_LED_PIN, true);
    sleep_ms(8);
    gpio_put(PICO_DEFAULT_LED_PIN, false);
    sleep_ms(8);
    t++;
  }

  return 0;
}
