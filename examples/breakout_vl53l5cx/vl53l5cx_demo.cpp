#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "drivers/vl53l5cx/vl53l5cx.hpp"

#include "common/pimoroni_i2c.hpp"

//using namespace pimoroni;

pimoroni::I2C i2c(4, 5);

VL53L5CX_Configuration configuration {
    .platform = VL53L5CX_Platform{
        .address = 0x29,
        .i2c = i2c0
    },
};

int main() {
  stdio_init_all();

  vl53l5cx_init(&configuration);
  vl53l5cx_set_ranging_mode(&configuration, VL53L5CX_RANGING_MODE_AUTONOMOUS);
  vl53l5cx_set_resolution(&configuration, VL53L5CX_RESOLUTION_4X4);
  vl53l5cx_start_ranging(&configuration);

  while(true) {
      uint8_t is_ready;
    if(vl53l5cx_check_data_ready(&configuration, &is_ready) == VL53L5CX_STATUS_OK) {
        if(is_ready){
            VL53L5CX_ResultsData result;
            if(vl53l5cx_get_ranging_data(&configuration, &result) == VL53L5CX_STATUS_OK) {
                printf("Distance: %dmm\n", result.distance_mm[0]);
            }
        }
    }
    sleep_ms(20);
  }

  return 0;
}
