#include <stdio.h>
#include "pico/stdlib.h"
#include "common/pimoroni_common.hpp"
#include "hardware/uart.h"

#include <cstring>

#include "pms5003.hpp"

using namespace pimoroni;


PMS5003 pms5003(uart1, 8, 9, 2, 3);
PMS5003::response_data data;

int main() {
  stdio_init_all();

  while(true){
    bool result = pms5003.read(data);
    if(result){
      printf("%04x ", data.pm_1_0);      // PM1.0 ug/m3 (ultrafine particles)
      printf("%04x ", data.pm_2_5);      // PM2.5 ug/m3 (combustion particles, organic compounds, metals)
      printf("%04x ", data.pm_10);       // PM10 ug/m3  (dust, pollen, mould spores)
      printf("%04x ", data.pm_1_0_ao);   // PM 1.0 under atmospheric environment
      printf("%04x ", data.pm_2_5_ao);   // PM 2.5 under atmospheric environment
      printf("%04x ", data.pm_10_ao);    // PM 10  under atmospheric environment
      printf("%04x ", data.pm_0_3_1l);   // PM 0.3 in 0.1L of air
      printf("%04x ", data.pm_0_5_1l);   // PM 0.5 in 0.1L of air
      printf("%04x ", data.pm_1_0_1l);   // PM 1.0 in 0.1L of air
      printf("%04x ", data.pm_2_5_1l);   // PM 2.5 in 0.1L of air
      printf("%04x ", data.pm_5_1l);     // PM 5   in 0.1L of air
      printf("%04x ", data.pm_10_1l);    // PM 10  in 0.1L of air
      printf("\n");
    }
    sleep_ms(100);
  };

  return 0;
}