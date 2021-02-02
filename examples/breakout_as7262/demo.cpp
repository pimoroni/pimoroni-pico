#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "as7262.hpp"

using namespace pimoroni;

AS7262 as72622;

int main() {
  setup_default_uart();

  as72622.init();

  int16_t hw_version = as72622.i2c_reg_read_uint16(0x00);
  int16_t fw_version = as72622.i2c_reg_read_uint16(0x02);
  printf("%04x %04x \n", hw_version, fw_version);

  as72622.set_gain(AS7262::gain::X16);
  as72622.set_measurement_mode(AS7262::measurement_mode::cont_roygbr);
  as72622.set_illumination_current(AS7262::illumination_current::ma12);
  as72622.set_indicator_current(AS7262::indicator_current::ma4);
  //as72622.set_leds(false, false);

  while(true) {

    AS7262::reading reading = as72622.read();
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
