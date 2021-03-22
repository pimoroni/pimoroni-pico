#include "pico/stdlib.h"

#include "breakout_oled128x128i2c.hpp"
#include "breakout_oled128x128spi.hpp"

using namespace pimoroni;

uint8_t oled_buffer[128 * 128 / 8];

// Pimoroni 1.12" OLED I2C
BreakoutOLED128x128i2c oled(128, 128, oled_buffer, i2c0, 0x3c, 4, 5, -1);

// Pimoroni 1.12" OLED SPI
BreakoutOLED128x128spi oled_spi(128, 128, oled_buffer, spi0, 22, 16, 18, 19, -1);

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  oled.init(); // Init with default invert/flip values
  oled_spi.init(); // Init with default invert/flip values

  // Flash the screen white
  oled.Fill(SH1107_SET);
  oled.UpdateScreen();
  oled_spi.UpdateScreen();
  sleep_ms(500);
  //
  // Back to black
  oled.Fill(SH1107_CLEAR);
  oled.UpdateScreen();
  oled_spi.UpdateScreen();
  sleep_ms(500);

  // Write lines, text, circles and rectangles on the screen
  oled.SetCursor(64, 64);
  oled.WriteString("Hello Mum", Font_7x10, SH1107_SET);
  oled.SetCursor(64, 4);
  oled.WriteString("Big", Font_16x26, SH1107_SET);
  oled.SetCursor(56, 34);
  oled.WriteString("Medium", Font_11x18, SH1107_SET);
  oled.SetCursor(74, 54);
  oled.WriteString("Small", Font_6x8, SH1107_SET);
  oled.SetCursor(4, 4);
  oled.WriteChar('A', Font_11x18, SH1107_SET);
  oled.Line(1, 1, 50, 1, SH1107_SET);
  oled.Line(1, 25, 50, 25, SH1107_SET);
  oled.Line(1, 50, 50, 50, SH1107_SET);
  oled.Line(50, 50, 50, 75, SH1107_SET);
  oled.Line(50, 80, 50, 95, SH1107_SET);
  oled.Line(50, 100, 50, 110, SH1107_SET);
  oled.DrawCircle(50, 100, 25, SH1107_SET);
  oled.DrawCircle(50, 100, 20, SH1107_SET);
  oled.DrawCircle(50, 100, 15, SH1107_SET);
  oled.DrawCircle(50, 100, 10, SH1107_SET);
  oled.DrawCircle(50, 100, 5, SH1107_SET);
  oled.DrawRectangle(25, 75, 75, 125, SH1107_SET);
  oled.UpdateScreen();
  oled_spi.UpdateScreen();

  // Now flash the on-board LED to show we've finished
  while(true) {
    gpio_put(PICO_DEFAULT_LED_PIN, true);
    sleep_ms(1000);
    gpio_put(PICO_DEFAULT_LED_PIN, false);
    sleep_ms(1000);
  }

  return 0;
}
