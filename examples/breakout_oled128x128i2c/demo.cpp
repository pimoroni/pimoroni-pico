#include "pico/stdlib.h"

#include "breakout_oled128x128i2c.hpp"

using namespace pimoroni;

BreakoutOLED128x128i2c oled;

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  oled.init();   

  // Flash the screen white
  oled.Fill((pimoroni::SH1107_COLOR)1);
  oled.UpdateScreen();
  sleep_ms(500);
  //
  // Back to black
  oled.Fill((pimoroni::SH1107_COLOR)0);
  oled.UpdateScreen();
  sleep_ms(500);

  // Write lines, text, circles and rectangles on the screen
  oled.SetCursor(64, 64);
  oled.WriteString("Hello Mum", Font_7x10, (pimoroni::SH1107_COLOR)1);
  oled.SetCursor(64, 4);
  oled.WriteString("Big", Font_16x26, (pimoroni::SH1107_COLOR)1);
  oled.SetCursor(56, 34);
  oled.WriteString("Medium", Font_11x18, (pimoroni::SH1107_COLOR)1);
  oled.SetCursor(74, 54);
  oled.WriteString("Small", Font_6x8, (pimoroni::SH1107_COLOR)1);
  oled.SetCursor(4, 4);
  oled.WriteChar('A', Font_11x18, (pimoroni::SH1107_COLOR)1);
  oled.Line(1,1,50,1, (pimoroni::SH1107_COLOR)1);
  oled.Line(1,25,50,25, (pimoroni::SH1107_COLOR)1);
  oled.Line(1,50,50,50, (pimoroni::SH1107_COLOR)1);
  oled.Line(50,50,50,75, (pimoroni::SH1107_COLOR)1);
  oled.Line(50,80,50,95, (pimoroni::SH1107_COLOR)1);
  oled.Line(50,100,50,110, (pimoroni::SH1107_COLOR)1);
  oled.DrawCircle(50,100,25,(pimoroni::SH1107_COLOR)1);
  oled.DrawCircle(50,100,20,(pimoroni::SH1107_COLOR)1);
  oled.DrawCircle(50,100,15,(pimoroni::SH1107_COLOR)1);
  oled.DrawCircle(50,100,10,(pimoroni::SH1107_COLOR)1);
  oled.DrawCircle(50,100,5,(pimoroni::SH1107_COLOR)1);
  oled.DrawRectangle(25, 75, 75, 125,(pimoroni::SH1107_COLOR)1);
  oled.UpdateScreen();

  // Now flash the on-board LED to show we've finished
  while(true) {
    gpio_put(PICO_DEFAULT_LED_PIN, true);
    sleep_ms(1000);
    gpio_put(PICO_DEFAULT_LED_PIN, false);
    sleep_ms(1000);
  }

  return 0;
}
