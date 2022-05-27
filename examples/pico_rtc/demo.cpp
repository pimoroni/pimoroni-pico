#include <cstdio>

#include <stdio.h>
#include "pico/stdlib.h"

#include "pcf85063a.hpp"

using namespace pimoroni;

PCF85063A rtc;

int main() {
  // keep the pico awake by holding vsys_en high
  gpio_set_function(2, GPIO_FUNC_SIO); 
  gpio_set_dir(2, GPIO_OUT); 
  gpio_put(2, true);

  stdio_init_all();

  printf("woken up!\n");

  // turn on led to show we're awake
  gpio_set_function(6, GPIO_FUNC_SIO); 
  gpio_set_dir(6, GPIO_OUT); 
  gpio_put(6, true);


  rtc.init();
  rtc.configure(PCF85063A::CLEAR_ALARM_FLAG);
  // rtc.setup(false);



  // Make sure we have 24-hour time (smaller display!)
//  if(rtc.is_12_hour())
    //rtc.set_24_hour();
  datetime_t now = {
    .year = 2022, .month = 5, .day = 27,
    .hour = 12, .min = 29, .sec = 14
  };
  rtc.set_datetime(&now);

  // stay awake for 1 second
  sleep_ms(1000);

  // shuold wake up 4 seconds after going to sleep
  rtc.set_seconds_timer(3);


 // printf("going to sleep!\n");

  // go to sleep by driving vsys_en low
  //gpio_put(2, false);
  gpio_set_dir(2, GPIO_IN); 

/*
  while(true) {
    datetime_t dt = rtc.get_datetime();
    printf(
      "%04d-%02d-%02d %02d:%02d:%02d\n",
      dt.year, dt.month, dt.day,
      dt.hour, dt.min, dt.sec
    );

    sleep_ms(1000);
  }
*/
  return 0;
}
