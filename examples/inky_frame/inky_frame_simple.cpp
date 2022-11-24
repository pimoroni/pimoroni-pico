#include <cstdio>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "drivers/uc8159/uc8159.hpp"

using namespace pimoroni;

    enum Pin {
      HOLD_VSYS_EN  =  2,
      I2C_INT       =  3,
      I2C_SDA       =  4,
      I2C_SCL       =  5,
      SR_CLOCK      =  8,
      SR_LATCH      =  9,
      SR_OUT        = 10,
      MISO          = 16,
      EINK_CS       = 17,
      CLK           = 18,
      MOSI          = 19,
      SD_DAT0       = 19,
      SD_DAT1       = 20,
      SD_DAT2       = 21,
      SD_DAT3       = 22,
      SD_CS         = 22,
      ADC0          = 26,
      EINK_RESET    = 27,
      EINK_DC       = 28
    };



  uint width=800;
  uint height=480;

PicoGraphics_PenP4 graphics(width, height, nullptr);
UC8159 inky(width, height, {spi0, EINK_CS, CLK, MOSI, PIN_UNUSED, EINK_DC, PIN_UNUSED}, PIN_UNUSED , EINK_RESET );



int main() {
  inky.reset();
  stdio_init_all();
  sleep_ms(500);

  printf("\n");
  printf("\n");
  printf("\n");

  printf("initialising inky frame.. ");
  printf("done!\n");
  graphics.set_pen(3);
  graphics.line(Point(0,0), Point(800,480));

  inky.update(&graphics);






  printf("done!\n");

 


  return 0;
}
