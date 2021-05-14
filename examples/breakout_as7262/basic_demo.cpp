#include "pico/stdlib.h"
#include "common/pimoroni_common.hpp"

#include "breakout_as7262.hpp"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutAS7262 as7262(&i2c);

int main() {
  stdio_init_all();

  as7262.init();

  uint8_t dev_type = as7262.device_type();
  uint8_t hw_version = as7262.hardware_version();

  uint8_t major, minor, sub;
  as7262.firmware_version(major, minor, sub);
  printf("Device: %d, HW: %d, FW: %d.%d.%d\n", dev_type, hw_version, major, minor, sub);

  as7262.set_gain(AS7262::gain::X64);
  as7262.set_integration_time(17.857);
  as7262.set_measurement_mode(AS7262::measurement_mode::cont_roygbr);
  as7262.set_illumination_current(AS7262::illumination_current::ma12);
  as7262.set_indicator_current(AS7262::indicator_current::ma4);
  as7262.set_leds(true, true);

  while(true) {

    AS7262::reading reading = as7262.read();
    printf("R: %f O: %f Y: %f G: %f B: %f V: %f \n",
      reading.red,
      reading.orange,
      reading.yellow,
      reading.green,
      reading.blue,
      reading.violet
    );

    sleep_ms(1000);
  }

  return 0;
}
