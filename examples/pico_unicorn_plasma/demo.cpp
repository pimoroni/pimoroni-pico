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

PicoUnicorn<14,1,0, uint16_t, pimoroni::GAMMA_14BIT> pico_unicorn;

// Sine table to speed up execution
static const int8_t sinetab[256] = {
     0,   2,   5,   8,  11,  15,  18,  21,
    24,  27,  30,  33,  36,  39,  42,  45,
    48,  51,  54,  56,  59,  62,  65,  67,
    70,  72,  75,  77,  80,  82,  85,  87,
    89,  91,  93,  96,  98, 100, 101, 103,
   105, 107, 108, 110, 111, 113, 114, 116,
   117, 118, 119, 120, 121, 122, 123, 123,
   124, 125, 125, 126, 126, 126, 126, 126,
   127, 126, 126, 126, 126, 126, 125, 125,
   124, 123, 123, 122, 121, 120, 119, 118,
   117, 116, 114, 113, 111, 110, 108, 107,
   105, 103, 101, 100,  98,  96,  93,  91,
    89,  87,  85,  82,  80,  77,  75,  72,
    70,  67,  65,  62,  59,  56,  54,  51,
    48,  45,  42,  39,  36,  33,  30,  27,
    24,  21,  18,  15,  11,   8,   5,   2,
     0,  -3,  -6,  -9, -12, -16, -19, -22,
   -25, -28, -31, -34, -37, -40, -43, -46,
   -49, -52, -55, -57, -60, -63, -66, -68,
   -71, -73, -76, -78, -81, -83, -86, -88,
   -90, -92, -94, -97, -99,-101,-102,-104,
  -106,-108,-109,-111,-112,-114,-115,-117,
  -118,-119,-120,-121,-122,-123,-124,-124,
  -125,-126,-126,-127,-127,-127,-127,-127,
  -128,-127,-127,-127,-127,-127,-126,-126,
  -125,-124,-124,-123,-122,-121,-120,-119,
  -118,-117,-115,-114,-112,-111,-109,-108,
  -106,-104,-102,-101, -99, -97, -94, -92,
   -90, -88, -86, -83, -81, -78, -76, -73,
   -71, -68, -66, -63, -60, -57, -55, -52,
   -49, -46, -43, -40, -37, -34, -31, -28,
   -25, -22, -19, -16, -12,  -9,  -6,  -3
};


// We have four objects with radius and centre points as configured for the 16x7 display
const float radius1 =8.2, radius2 =11.5, radius3 =20.4, radius4 =22.1,
            centerx1=8.0, centerx2=5.8, centerx3=12.7, centerx4= 2.0,
            centery1= 2.1, centery2= 1.5, centery3=3.2, centery4=-0.5;
float       angle1  = 0.0, angle2  = 0.0, angle3  = 0.0, angle4  = 0.0;
long        hueShift= 0;

#define FPS 30         // Maximum frames-per-second


// HSV Conversion expects float inputs in the range of 0.00-1.00 for each channel
// Outputs are rgb in the range 0-255 for each channel
void from_hsv(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
  float i = floor(h * 6.0f);
  float f = h * 6.0f - i;
  v *= 255.0f;
  uint8_t p = v * (1.0f - s);
  uint8_t q = v * (1.0f - f * s);
  uint8_t t = v * (1.0f - (1.0f - f) * s);

  switch (int(i) % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  }
}



int main() {
  stdio_init_all();
  // pico_unicorn.init();
  pico_unicorn.clear();


  while(true) {
    int           x1, x2, x3, x4, y1, y2, y3, y4, sx1, sx2, sx3, sx4;
    unsigned char x, y;
    int8_t        value;
    uint8_t       r = 0, g = 0, b = 0;
    uint8_t       j, k, l, m;

    // Setup a delay to slow the framerate. 
    // Would be better to read from a timer as some math operations take variable time
    sleep_ms(1000 / FPS);

    sx1 = (int)(cos(angle1) * radius1 + centerx1);
    sx2 = (int)(cos(angle2) * radius2 + centerx2);
    sx3 = (int)(cos(angle3) * radius3 + centerx3);
    sx4 = (int)(cos(angle4) * radius4 + centerx4);
    y1  = (int)(sin(angle1) * radius1 + centery1);
    y2  = (int)(sin(angle2) * radius2 + centery2);
    y3  = (int)(sin(angle3) * radius3 + centery3);
    y4  = (int)(sin(angle4) * radius4 + centery4);

    for(y=0; y<7; y++) {
      x1 = sx1; x2 = sx2; x3 = sx3; x4 = sx4;
      for(x=0; x<16; x++) {
        j = (x1 * x1 + y1 * y1) >> 2;
        k = (x2 * x2 + y2 * y2) >> 2;
        l = (x3 * x3 + y3 * y3) >> 3;
        m = (x4 * x4 + y4 * y4) >> 3;
        //printf("X: %i, Y: %i, \t%i, %i, %i, %i\n",x,y,j,k,l,m);
        value = (int8_t)(hueShift
          + (int8_t)*(sinetab + j)
          + (int8_t)*(sinetab + k)
          + (int8_t)*(sinetab + l)
          + (int8_t)*(sinetab + m));
        //printf("X: %i, Y: %i, H: %i\n",x,y,value);
        from_hsv(((value+128.0f)/256.0f), 1, 1, r, g, b);
        pico_unicorn.set_pixel(x, y, r, g, b);
        x1--; x2--; x3--; x4--;
      }
      y1--; y2--; y3--; y4--;
    }

    angle1 += 0.03;
    angle2 -= 0.07;
    angle3 += 0.13;
    angle4 -= 0.15;
    hueShift += 2;
  }
  

  return 0;
}
