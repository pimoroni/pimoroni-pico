#include <stdio.h>
#include "pico/stdlib.h"
#include <string>

using namespace pimoroni;

// Pick *one* sensor type by uncommenting the relevant lines below:

// PMW3901
#include "breakout_pmw3901.hpp"
typedef BreakoutPMW3901 FlowSensor;

// PAA5100
//include "breakout_paa5100.hpp"
//typedef BreakoutPAA5100 FlowSensor;



FlowSensor flo(BG_SPI_FRONT);
FlowSensor::Degrees rotation = FlowSensor::DEGREES_0;
const uint8_t SIZE = FlowSensor::FRAME_SIZE;
uint8_t data[FlowSensor::FRAME_BYTES];

std::string value_to_char(uint8_t value) {
  const std::string charmap = " .:-=+*#%@";
  float val = (float)value / 255.0f;
  val *= charmap.length() - 1;
  std::string chosen_char = charmap.substr((uint8_t)val, 1);
  return chosen_char.append(chosen_char);  // Double chars to - sort of - correct aspect ratio
}

int main() {
  stdio_init_all();

  flo.init();
  flo.set_rotation(rotation);

  uint8_t offset = 0;
  uint8_t value = 0;
  
  while(true) {
    printf("Capturing...\n");
    uint16_t data_size = 0;
    if(flo.frame_capture(data, data_size)) {
      for(uint8_t y = 0; y < SIZE; y++) {
        if(rotation == FlowSensor::DEGREES_180 || rotation == FlowSensor::DEGREES_270)
          y = SIZE - y - 1;

        for(uint8_t x = 0; x < SIZE; x++) {
          if(rotation == FlowSensor::DEGREES_180 || rotation == FlowSensor::DEGREES_90)
            x = SIZE - x - 1;

          if(rotation == FlowSensor::DEGREES_90 || rotation == FlowSensor::DEGREES_270)
            offset = (x * 35) + y;
          else
            offset = (y * 35) + x;

          value = data[offset];
          printf("%s", value_to_char(value).c_str());
        }
        printf("\n");
      }
    }
    else {
      printf("Capture failed. %d bytes received, of %d. Recapturing in ", data_size, FlowSensor::FRAME_BYTES);
    }
    printf("5...\n");
    sleep_ms(1000);
    printf("4...\n");
    sleep_ms(1000);
    printf("3...\n");
    sleep_ms(1000);
    printf("2...\n");
    sleep_ms(1000);
    printf("Get Ready!\n");
    sleep_ms(1000);
  };
  return 0;
}
