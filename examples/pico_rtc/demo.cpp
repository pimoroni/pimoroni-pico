#include <cstdio>

#include <stdio.h>
#include "pico/stdlib.h"

#include "pcf85063a.hpp"

using namespace pimoroni;



void led_on() {
}

void led_off() {

}

int main() {
  // turn on led to show we're awake
  gpio_set_function(6, GPIO_FUNC_SIO);
  gpio_set_dir(6, GPIO_OUT);
  gpio_put(6, true);

  // keep the pico awake by holding vsys_en high
  gpio_set_function(2, GPIO_FUNC_SIO);
  gpio_set_dir(2, GPIO_OUT);
  gpio_put(2, true);

  // init usb serial for debugging and give our terminal monitor a bit of
  // time to connect
  stdio_init_all();
  sleep_ms(500);

  printf("\n\n\n");
  printf("PCF85063A rtc demo!\n");
  printf("===================\n");
  sleep_ms(500);

  printf("initialise rtc.. ");
  I2C i2c(4, 5);
  PCF85063A rtc(&i2c);
  rtc.init();
  printf("done!\n");
  printf("\n");

  printf("check clock function\n");
  printf("\n");

  printf("- setting date and time\n");
  printf("  > 30/5/2022 08:07:12\n");
  datetime_t now = {
    .year = 2022, .month = 5, .day = 30, .hour = 8, .min = 7, .sec = 12
  };
  rtc.set_datetime(&now);

  printf("\n");
  printf("- sleeping for 3 seconds\n");
  sleep_ms(3000);

  printf("\n");
  printf("- reading date and time\n");
  now = rtc.get_datetime();
  printf("  > %02d/%02d/%04d %02d:%02d:%02d\n",
    now.day, now.month, now.year, now.hour, now.min, now.sec);
  printf("\n");

  printf("check alarm function\n");
  printf("\n");

  printf("- setting alarm for in 6 seconds\n");
  printf("  > 08:07:21\n");
  rtc.set_alarm(21, 7, 8);

  printf("\n");
  printf("- waiting for alarm flag\n");

  while(!rtc.read_alarm_flag()) {
    tight_loop_contents();
  }

  printf("\n");
  printf("- alarm triggered at\n");
  now = rtc.get_datetime();
  printf("  > %02d/%02d/%04d %02d:%02d:%02d\n",
    now.day, now.month, now.year, now.hour, now.min, now.sec);
  printf("\n");

  printf("- clear alarm flag\n");
  rtc.clear_alarm_flag();


  printf("check timer function\n");
  printf("\n");

  printf("- setting timer for in 3 seconds\n");
  rtc.set_timer(3);

  printf("\n");
  printf("- waiting for timer flag\n");

  while(!rtc.read_timer_flag()) {
    tight_loop_contents();
  }

  printf("\n");
  printf("- timer triggered at\n");
  now = rtc.get_datetime();
  printf("  > %02d/%02d/%04d %02d:%02d:%02d\n",
    now.day, now.month, now.year, now.hour, now.min, now.sec);
  printf("\n");

  printf("- clear timer flag\n");
  rtc.clear_alarm_flag();

  printf("\n");
  printf("tests complete\n");

  return 0;
}
