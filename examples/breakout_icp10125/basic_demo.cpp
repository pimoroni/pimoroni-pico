#include "pico/stdlib.h"
#include "common/pimoroni_common.hpp"

#include "icp10125.hpp"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
ICP10125 icp10125(&i2c);

int main() {
  stdio_init_all();

  icp10125.init();
  printf("init()\n");

  while(true) {
    auto result = icp10125.measure(ICP10125::NORMAL);
    printf("%fc %fPa %d\n", result.temperature, result.pressure, result.status);
    sleep_ms(500);
  }

  return 0;
}
