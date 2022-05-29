#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include <sstream>
#include <iomanip>

#include "common/pimoroni_i2c.hpp"
#include "pico_explorer.hpp"
#include "breakout_potentiometer.hpp"
#include "picographics_st7789.hpp"

using namespace pimoroni;

PicoGraphicsST7789 display(
  PicoExplorer::WIDTH,
  PicoExplorer::HEIGHT,
  ROTATE_0,  // Rotation
  false,     // Is it round!?
  nullptr,   // Buffer
  get_spi_pins(BG_SPI_FRONT)
);

Pen BLACK = display.create_pen(0, 0, 0);
Pen RED = display.create_pen(255, 0, 0);
Pen GREEN = display.create_pen(0, 255, 0);
Pen BLUE = display.create_pen(0, 0, 255);

I2C i2c(PICO_EXPLORER);
BreakoutPotentiometer pot(&i2c);
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

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  stdio_init_all();

  printf("Starting...\n");

  if(pot.init()) {
    printf("Potentiometer found...\n");

    while(true) {
      gpio_put(PICO_DEFAULT_LED_PIN, toggle);
      toggle = !toggle;

      float percent = pot.read();

      printf("Percent: %d\n", (int)(percent * 100));
      uint8_t r = 0, g = 0, b = 0;
      from_hsv(percent, 1.0f, 1.0f, r, g, b);
      pot.set_led(r, g, b);

      display.set_pen(BLACK);
      display.clear();

      {
        display.set_pen(RED);
        std::ostringstream ss;
        ss << "R = ";
        ss << (int)r;
        std::string s(ss.str());
        display.text(s, Point(10, 10), 220, 6);
      }

      {
        display.set_pen(GREEN);
        std::ostringstream ss;
        ss << "G = ";
        ss << (int)g;
        std::string s(ss.str());
        display.text(s, Point(10, 70), 220, 6);
      }

      {
        display.set_pen(BLUE);
        std::ostringstream ss;
        ss << "B = ";
        ss << (int)b;
        std::string s(ss.str());
        display.text(s, Point(10, 130), 220, 6);
      }

      {
        // Shouldn't really use create_pen in-line.
        // In default (RGB332) palette mode this will lookup the nearest 8-bit colour
        display.set_pen(display.create_pen(r, g, b));
        std::ostringstream ss;
        ss << "#";
        ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (int)r;
        ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (int)g;
        ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (int)b;
        std::string s(ss.str());
        display.text(s, Point(10, 190), 220, 5);
      }
      display.update();
    }
  }
  else {
    printf("No Potentiometer found :'(\n");
    gpio_put(PICO_DEFAULT_LED_PIN, true);
  }

  return 0;
}
