#include "pico/stdlib.h"

#include "breakout_as7262.hpp"
#include "pico_explorer.hpp"

using namespace pimoroni;

constexpr float INTEGRATION_TIME = 10.0f;

BreakoutAS7262 as7262;
uint16_t buffer[PicoExplorer::WIDTH * PicoExplorer::HEIGHT];
PicoExplorer pico_explorer(buffer);

uint8_t bar_width = PicoExplorer::WIDTH / 6;
uint8_t bar_height = PicoExplorer::HEIGHT;

void draw_bar(float scale, uint16_t channel) {
  int16_t bar_top = bar_height - (bar_height * scale);
  bar_top = std::max((int16_t)0, bar_top);
  int16_t current_bar_height = bar_height - bar_top;
  pico_explorer.rectangle(Rect(channel * bar_width, bar_top, bar_width, current_bar_height - 1));
}

int main() {
  stdio_init_all();

  pico_explorer.init();
  as7262.init();

  uint8_t dev_type = as7262.device_type();
  uint8_t hw_version = as7262.hardware_version();

  uint8_t major, minor, sub;
  as7262.firmware_version(major, minor, sub);
  printf("Device: %d, HW: %d, FW: %d.%d.%d\n", dev_type, hw_version, major, minor, sub);

  as7262.set_gain(AS7262::gain::X64);
  as7262.set_integration_time(INTEGRATION_TIME);
  as7262.set_measurement_mode(AS7262::measurement_mode::cont_roygbr);
  as7262.set_illumination_current(AS7262::illumination_current::ma12);
  as7262.set_indicator_current(AS7262::indicator_current::ma4);
  as7262.set_leds(true, true);

  while(true) {
    pico_explorer.set_pen(0, 0, 0);
    pico_explorer.clear();

    AS7262::reading reading = as7262.read();
    printf("R: %f O: %f Y: %f G: %f B: %f V: %f \n",
      reading.red,
      reading.orange,
      reading.yellow,
      reading.green,
      reading.blue,
      reading.violet
    );

    float m = reading.red;
    if(reading.orange > m) m = reading.orange;
    if(reading.yellow > m) m = reading.yellow;
    if(reading.green > m) m = reading.green;
    if(reading.blue > m) m = reading.blue;
    if(reading.violet > m) m = reading.violet;

    pico_explorer.set_pen(0, 0, 0);
    pico_explorer.clear();

    // Red
    pico_explorer.set_pen(255, 0, 0);
    draw_bar(reading.red / m, 0);
    
    // Orange
    pico_explorer.set_pen(255, 128, 0);
    draw_bar(reading.orange / m, 1);

    // Yellow
    pico_explorer.set_pen(255, 255, 0);
    draw_bar(reading.yellow / m, 2);

    // Green
    pico_explorer.set_pen(0, 255, 0);
    draw_bar(reading.green / m, 3);

    // Blue
    pico_explorer.set_pen(0, 0, 255);
    draw_bar(reading.blue / m, 4);

    // Violet
    pico_explorer.set_pen(255, 0, 255);
    draw_bar(reading.violet / m, 5);

    pico_explorer.update();

    sleep_ms(INTEGRATION_TIME);
  }

  return 0;
}
