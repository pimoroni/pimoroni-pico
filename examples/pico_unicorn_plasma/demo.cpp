/*
  Pico Unicorn Plasma Example
  Written by Tim Kerby https://github.com/tkerby

  Hardware: Raspberry Pi Pico with Pimoroni Unicorn Pico Display
  Printf over USB 9600 8N1 (see makefile)

  Based on Adafruit Arduino Example for 32x32 panel
  Originally written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon for Adafruit Industries.
  https://github.com/adafruit/RGB-matrix-Panel/blob/master/examples/plasma_32x32/plasma_32x32.ino

  BSD License
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include <time.h>

#include "pico_unicorn.hpp"

using namespace pimoroni;

PicoUnicorn pico_unicorn;

template<typename T>
T cb(T x){return x*x*x;}

int main() {
  stdio_init_all();
  pico_unicorn.init();
  pico_unicorn.clear();

  sleep_ms(1e3);
  printf("\nHi\n");

  int state=0;

  // const int Tperiod=60000;
  // const int Tperiod=600;
  // const int Tperiod=20000;
  // const int Tperiod=10000;
  // const int Tperiod=8000;
  // const int Tperiod=800;
  // const int Tperiod=80;
  // const float Tperiod=8;
  // const float Tperiod=100;
  const float Tperiod=10;
  // const float Tperiod=1;
  // const float Tperiod=2;
  // const float Tperiod=2;
  // const float Tperiod=1;
  // const float Tperiod=0.5;
  // const int Tperiod=4000;
  // const int Tperiod=2000;
  // const int Tperiod=1000;
  // const int period=128;
  // const int period=64;
  // const int period=32;
  const int period=16;
  // const int period=8;

  uint ms = to_us_since_boot(get_absolute_time());
  float looptime = Tperiod/period;
  uint mycount=0;
  float periodfilter=0.0;
  float alpha=0.1;

  for(;;) {
    unsigned char x, y;

    state = (state + 1) % period;

    uint16_t xx = std::min(state,period-state); // triangle wave from 0 to period/2
    uint16_t ii = xx*xx*xx;

   // printf("t=%d xx=%d ii=%d\n", state, xx, ii);

    uint mytime = to_us_since_boot(get_absolute_time());
    periodfilter += alpha*(mytime-ms-periodfilter);
    if (mycount==10) {
      printf("t=%d xx=%d ii=%d looptime=%f time=%f\n", state, xx, ii,looptime, 1e-6*periodfilter);
      mycount=0;
    }
    mycount++;
    ms=mytime;
    
    for(y=0; y<7; y++) {
      for(x=0; x<16; x++) {
	uint16_t ar = (y+state+x)%16; // triangle wave from 0 to period/2
	// uint8_t ar = (y+(state>>1)+x+1)%16; // triangle wave from 0 to period/2
	uint16_t br = std::min(ar,uint16_t(16-ar)); // triangle wave from 0 to period/2-1
	// uint16_t br = x*7+y;
	// uint16_t br = x+y*16;
        // pico_unicorn.set_pixel_(x, y, uint16_t(std::round(1+cb(12*float(br)/float(period-1)))), uint8_t(0), uint8_t(0));
	// pico_unicorn.set_pixel_(x, y, br, uint8_t(0), uint8_t(0));
	// pico_unicorn.set_pixel_(x, y, std::round(cb(float(br)/(7*16-1))*(1<<10)), uint8_t(0), uint8_t(0));
	// pico_unicorn.set_pixel_(x, y, 1+br, uint8_t(0), uint8_t(0));
        // pico_unicorn.set_pixel_(x, y, uint16_t(std::round(1+cb(10*float(br)/float(period-1)))), uint8_t(0), uint8_t(0));
        // pico_unicorn.set_pixel_(x, y, uint16_t(std::round(1+cb(12*float(br)/float(period-1)))), uint8_t(0), uint8_t(0));
        // pico_unicorn.set_pixel_(x, y, uint16_t(std::round(1+cb(16*float(br)/float(period-1)))), uint8_t(0), uint8_t(0));
        pico_unicorn.set_pixel(x, y, uint8_t(std::round(1+254*float(br)/float(period-1))), uint8_t(0), uint8_t(0));
	
	// pico_unicorn.set_pixel_(x, y, br, uint8_t(0), uint8_t(0));
        // pico_unicorn.set_pixel_(x, y, uint16_t(std::round(cb(10*float(br)/float(period-1)))), uint8_t(0), uint8_t(0));
        // pico_unicorn.set_pixel_(x, y, uint16_t(std::round(cb(12*float(br)/float(period-1)))), uint8_t(0), uint8_t(0));

        // pico_unicorn.set_pixel_(x, y, uint16_t(std::round(1+((1<<10)-1)*cb(float(br)/float(period-1)))), uint8_t(0), uint8_t(0));
	// pico_unicorn.set_pixel_(x, y, uint16_t((1<<8)-1), uint16_t(0), uint16_t(0));
        // pico_unicorn.set_pixel_(x, y, uint16_t((1<<12)-1), uint16_t(0), uint16_t(0));
        // pico_unicorn.set_pixel_(x, y, uint16_t((1<<14)-1), uint16_t(0), uint16_t(0));
        // pico_unicorn.set_pixel_(x, y, uint16_t((1<<16)-1), uint16_t(0), uint16_t(0));
        // pico_unicorn.set_pixel_(x, y, uint16_t(std::round(1+((1<<11)-1.0)*cb(float(br)/float(period/2-1)))), uint16_t(0), uint16_t(0));
        // pico_unicorn.set_pixel_(x, y, uint16_t(std::round(0+1022*cb(float(br)/float(period-1)))), uint16_t(0), uint16_t(0));
        // pico_unicorn.set_pixel_(x, y, uint8_t(std::round(1+254*cb(float(br)/float(period-1)))), uint8_t(0), uint8_t(0));
        // pico_unicorn.set_pixel_(x, y, uint8_t(255), uint8_t(0), uint8_t(0));
	
	// uint16_t ag = (7-y+31-state+x+1)%16; // triangle wave from 0 to period/2
	// uint16_t bg = 1+std::min(ag,uint16_t(16-ag)); // triangle wave from 0 to period/2
        // pico_unicorn.set_pixel(x, y, ii, uint16_t(0), uint16_t(0));

        // pico_unicorn.set_pixel(x, y, uint8_t(1+((1<<2)-1)*br*br*br/((period-1)*(period-1)*(period-1))  ), uint8_t(0), uint8_t(0));
        // pico_unicorn.set_pixel(x, y, uint16_t(1+br*br*br/8), uint16_t(0), uint16_t(0));
        // pico_unicorn.set_pixel(x, y, uint16_t((1<<16)-1), uint16_t(0), uint16_t(0));
        // pico_unicorn.set_pixel_(x, y, uint8_t((1<<16)-1), uint8_t(0), uint8_t(0));
        // pico_unicorn.set_pixel(x, y, br, bg, uint16_t(0));
        // pico_unicorn.set_pixel(x, y, uint16_t(br*br*br/4), uint16_t(bg*bg*bg/16), uint16_t(0));
        // pico_unicorn.set_pixel(x, y, uint16_t(0), uint16_t(bg*bg*bg/4), uint16_t(0));
        // pico_unicorn.set_pixel(x, y, br,uint16_t(0), uint16_t(0));
        // pico_unicorn.set_pixel(x, y, uint16_t(0), bg, uint16_t(0));
        // pico_unicorn.set_pixel(x, y, uint16_t(x), uint16_t(0), uint16_t(0));
        // pico_unicorn.set_pixel(x, y, uint16_t(xx+y+1), uint16_t(0), uint16_t(0));
        // pico_unicorn.set_pixel(x, y, uint16_t(x), uint16_t(y), uint16_t(0));
        // pico_unicorn.set_pixel(x, y, uint16_t(x), uint16_t(y), uint16_t(0));
        // pico_unicorn.set_pixel(x, y, uint16_t(x), uint16_t(0), uint16_t(y));
        // pico_unicorn.set_pixel(x, y, uint8_t(xx), uint8_t(0), uint8_t(0));
      }
    }
    sleep_ms(1000*looptime);
  }

  return 0;
}
