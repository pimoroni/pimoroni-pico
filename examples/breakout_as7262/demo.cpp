#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "as7262.hpp"

using namespace pimoroni;

#define I2C_BUS i2c0
#define I2C_SDA 20
#define I2C_SCL 21

AS7262 as72622(I2C_BUS, I2C_SDA, I2C_SCL, 22);

int main() {
  setup_default_uart();

  printf("Hello World\n");

  as72622.init();
  printf("init Done\n");

  int16_t hw_version = as72622.i2c_reg_read_uint16(0x00);
  int16_t fw_version = as72622.i2c_reg_read_uint16(0x02);
  printf("%04x %04x \n", hw_version, fw_version);

  as72622.set_gain(AS7262::gain::X16);
  as72622.set_measurement_mode(AS7262::measurement_mode::cont_roygbr);
  as72622.set_illumination_current(AS7262::illumination_current::ma12);
  as72622.set_indicator_current(AS7262::indicator_current::ma4);
  as72622.set_leds(true, true);

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
