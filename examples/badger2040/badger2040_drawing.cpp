#include "pico/stdlib.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <algorithm>
#include "pico/time.h"
#include "pico/platform.h"

#include "common/pimoroni_common.hpp"
#include "badger2040.hpp"

using namespace pimoroni;

Badger2040 badger;

uint32_t time() {
  absolute_time_t t = get_absolute_time();
  return to_ms_since_boot(t);
}

int main() {

  stdio_init_all();

  sleep_ms(500);

  printf("\n\n=======\nbadger2040 starting up\n\n");

  badger.init();

  while(true) {
    // TODO: draw something interesting...
  }
}
