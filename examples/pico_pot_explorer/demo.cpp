#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include <sstream>
#include <iomanip>

#include "pico_explorer.hpp"
#include "breakout_potentiometer.hpp"

using namespace pimoroni;

uint16_t buffer[PicoExplorer::WIDTH * PicoExplorer::HEIGHT];
PicoExplorer pico_explorer(buffer);

BreakoutPotentiometer pot;
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

  pico_explorer.init();
  pico_explorer.update();

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

      pico_explorer.set_pen(0, 0, 0);
      pico_explorer.clear();

      {
        pico_explorer.set_pen(255, 0, 0);
        std::ostringstream ss;
        ss << "R = ";
        ss << (int)r;
        std::string s(ss.str());
        pico_explorer.text(s, Point(10, 10), 220, 6);
      }

      {
        pico_explorer.set_pen(0, 255, 0);
        std::ostringstream ss;
        ss << "G = ";
        ss << (int)g;
        std::string s(ss.str());
        pico_explorer.text(s, Point(10, 70), 220, 6);
      }

      {
        pico_explorer.set_pen(0, 0, 255);
        std::ostringstream ss;
        ss << "B = ";
        ss << (int)b;
        std::string s(ss.str());
        pico_explorer.text(s, Point(10, 130), 220, 6);
      }

      {
        pico_explorer.set_pen(r, g, b);
        std::ostringstream ss;
        ss << "#";
        ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (int)r;
        ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (int)g;
        ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (int)b;
        std::string s(ss.str());
        pico_explorer.text(s, Point(10, 190), 220, 5);
      }
      pico_explorer.update();
    }
  }
  else {
    printf("Encoder Potentiometer found :'(\n");
    gpio_put(PICO_DEFAULT_LED_PIN, true);
  }

  return 0;
}
