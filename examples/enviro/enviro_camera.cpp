#include <cstdio>
#include <math.h>
#include <time.h>
#include <stdio.h>

#include "tusb.h"

#include "pico/stdlib.h"

#include "enviro_camera.hpp"

using namespace pimoroni;
using namespace enviro;

EnviroCamera camera;


int main() {
  stdio_init_all();
  stdio_set_translate_crlf(&stdio_usb, false);

  camera.init();

  camera.warn_led(EnviroBase::WARN_LED_OFF);

  uint8_t fb[352 * 288 * 2];

  //while(true) {
   /* if(enviro.switch_pressed()) {
      printf("!");
    }else{
      printf(".");
    }*/

    // select dsp register bank
   // uint8_t v = enviro.reg_read(0xC1);
    

    while(!camera.switch_pressed()) {
      sleep_ms(10);
    }

    //uint32_t start = time();
    camera.activity_led(true);
    camera.capture(fb);
    camera.activity_led(false);

//    fwrite(fb, sizeof(uint8_t), sizeof(fb), stdout);

    uint bytes_left = sizeof(fb);
    uint8_t *data = fb;
    while(bytes_left) {
      uint can_write_bytes = tud_cdc_write_available();
      uint will_write_bytes = bytes_left < can_write_bytes ? bytes_left : can_write_bytes;

      if(will_write_bytes) {
        tud_cdc_write(data, will_write_bytes);
        bytes_left -= will_write_bytes;
        data += will_write_bytes;
      }

      float brightness = ((sizeof(fb) - bytes_left) * 100) / sizeof(fb);
      camera.activity_led(brightness);
    }
    tud_cdc_write_flush();

    camera.activity_led(false);
    
    /*
    if (tud_cdc_connected()) {
        if (tud_cdc_available()) {
            return tud_cdc_read(buf, buf_len);
        }
    }

    fwrite(fb, sizeof(uint8_t), sizeof(fb), stdout);*/
   // fflush(stdout);
    //stdio_flush();
/*
    for(int i = 0; i < 352 * 288 * 2; i++) {
      putchar(fb[i]);
    }*/
/*
    for(int i = 0; i < 352 * 288 * 2; i++) {
      putchar(i % 256);
    }
*/
    // request horizontal size value
    //printf("read register");
    /*uint8_t v;
    bool ok = enviro._sccb_read_reg(0x1C, v);

    printf("%s > %d\n", ok ? "OK" : "FAIL", v);
*/
  //}

  return 0;
}
