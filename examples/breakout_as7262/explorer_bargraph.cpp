#include "pico/stdlib.h"
#include "common/pimoroni_common.hpp"

#include "breakout_as7262.hpp"
#include "pico_explorer.hpp"

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

constexpr float INTEGRATION_TIME = 10.0f;

I2C i2c(BOARD::PICO_EXPLORER);
BreakoutAS7262 as7262(&i2c);

ST7789 st7789(PicoExplorer::WIDTH, PicoExplorer::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);


void draw_bar(float scale, uint16_t channel) {
  static uint8_t bar_width = st7789.width / 6;
  static uint8_t bar_height = st7789.height;

  int16_t bar_top = bar_height - (bar_height * scale);
  bar_top = std::max((int16_t)0, bar_top);
  int16_t current_bar_height = bar_height - bar_top;
  graphics.rectangle(Rect(channel * bar_width, bar_top, bar_width, current_bar_height - 1));
}

int main() {
  stdio_init_all();

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

  Pen BLACK = graphics.create_pen(0, 0, 0);
  Pen RED = graphics.create_pen(255, 0, 0);
  Pen ORANGE = graphics.create_pen(255, 128, 0);
  Pen YELLOW = graphics.create_pen(255, 255, 0);
  Pen GREEN = graphics.create_pen(0, 255, 0);
  Pen BLUE = graphics.create_pen(0, 0, 255);
  Pen VIOLET = graphics.create_pen(255, 0, 255);

  while(true) {
    graphics.set_pen(BLACK);
    graphics.clear();

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

    graphics.set_pen(BLACK);
    graphics.clear();

    // Red
    graphics.set_pen(RED);
    draw_bar(reading.red / m, 0);
    
    // Orange
    graphics.set_pen(ORANGE);
    draw_bar(reading.orange / m, 1);

    // Yellow
    graphics.set_pen(YELLOW);
    draw_bar(reading.yellow / m, 2);

    // Green
    graphics.set_pen(GREEN);
    draw_bar(reading.green / m, 3);

    // Blue
    graphics.set_pen(BLUE);
    draw_bar(reading.blue / m, 4);

    // Violet
    graphics.set_pen(VIOLET);
    draw_bar(reading.violet / m, 5);

    st7789.update(&graphics);

    sleep_ms(INTEGRATION_TIME);
  }

  return 0;
}
