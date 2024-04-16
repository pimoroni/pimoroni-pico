#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "vl53l8cx.hpp"
#include "src/vl53l8cx_firmware.h"

#include "common/pimoroni_i2c.hpp"

using namespace pimoroni;

I2C i2c(4, 5);
VL53L8CX vl53l8cx(&i2c, (uint8_t *)&vl53l8cx_firmware_bin);

int main() {
  stdio_init_all();

  bool result = vl53l8cx.init();
  if(!result) {
      printf("Error initializing...\n");
  }
  vl53l8cx.set_ranging_mode(VL53L8CX::RANGING_MODE_AUTONOMOUS);
  vl53l8cx.set_resolution(VL53L8CX::RESOLUTION_4X4);
  vl53l8cx.start_ranging();

  while(true) {
    if(vl53l8cx.data_ready()) {
        VL53L8CX::ResultsData result;
        if(vl53l8cx.get_data(&result)) {
            printf("Distance: %dmm\n", result.distance_mm[0]);
        }
    }
    sleep_ms(20);
  }

  return 0;
}