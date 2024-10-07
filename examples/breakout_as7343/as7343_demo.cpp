#include "pico/stdlib.h"
#include "common/pimoroni_common.hpp"

#include "as7343.hpp"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
AS7343 as7343(&i2c);

int main() {
  stdio_init_all();

  printf("AS7343 Demo\n");
  as7343.init();
  printf("Init done...\n");

  uint8_t aux_id;
  uint8_t revision_id;
  uint8_t hardware_id;

  as7343.get_version(aux_id, revision_id, hardware_id);

  printf("Aux: %d, Rev: %d, HW: %d\n", aux_id, revision_id, hardware_id);

  printf("set_channels\n");
  as7343.set_channels(AS7343::channel_count::EIGHTEEN_CHANNEL);

  printf("set_gain\n");
  as7343.set_gain(1024);

  printf("set_measurement_time\n");
  as7343.set_measurement_time(500);

  printf("set_integration_time\n");
  as7343.set_integration_time(27800);

  printf("set_illumination_current\n");
  as7343.set_illumination_current(4);

  printf("set_illumination_led\n");
  as7343.set_illumination_led(true);

  printf("start...\n");
  while(true) {

    AS7343::reading reading = as7343.read();
    printf("FZ: %d FY: %d FXL: %d NIR: %d F2 %d F3: %d F4: %d F5: %d F1: %d F5: %d F7: %d F8: %d \n",
      reading.FZ,
      reading.FY,
      reading.FXL,
      reading.NIR,

      reading.F2,
      reading.F3,
      reading.F4,
      reading.F6,

      reading.F1,
      reading.F5,
      reading.F7,
      reading.F8
    );

    printf("Gain: C1: %fx C2: %fx C3: %fx\n", reading.gain(1), reading.gain(2), reading.gain(3));

    sleep_ms(1000);
  }

  return 0;
}