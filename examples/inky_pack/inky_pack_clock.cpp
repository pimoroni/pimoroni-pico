#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include "drivers/uc8151/uc8151.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "button.hpp"


#define CLOCK_RADIUS_PX 64
#define CLOCK_RIM 5
#define MIN_LENGTH 70
#define HR_LENGTH 40
#define SEC_LEGNTH 80
#define CLOCK_LOC_X 146
#define CLOCK_LOC_Y 64

Point point_on_circle;

point_on_circle circle_positions(uint8_t angle, uint8_t radius, uint8_t center_x, uint8_t center_y){

  Point position;
  position.x = sin(angle) * radius + center_x;
  position.y = cos(angle) * radius + center_y;
  return position;

}


using namespace pimoroni;

enum Pin {
    A           = 12,
    B           = 13,
    C           = 14,
    CS          = 17,
    CLK         = 18,
    MOSI        = 19,
    DC          = 20,
    RESET       = 21,
    BUSY        = 26,
};


UC8151 uc8151(296, 128, ROTATE_0); //Initalise the display 
PicoGraphics_Pen1BitY graphics(uc8151.width, uc8151.height, nullptr); //Create a PicoGraphics frame buffer


Button button_a(Pin::A);
Button button_b(Pin::B);
Button button_c(Pin::C);


void draw_analogue_clock(){
  
  // Clear display 
  graphics.set_pen(15);
  graphics.clear();
  graphics.set_pen(1);
  graphics.circle(Point(CLOCK_LOC_X, CLOCK_LOC_Y), CLOCK_RADIUS_PX);
  graphics.set_pen(14);
  graphics.circle(Point(CLOCK_LOC_X, CLOCK_LOC_Y), CLOCK_RADIUS_PX - CLOCK_RIM);
  graphics.set_pen(1);
  graphics.circle(Point(CLOCK_LOC_X, CLOCK_LOC_Y), 2);
  uc8151.update(&graphics);

}


Point pos1 = Point(0,0);


int main() {




  stdio_init_all();
  sleep_ms(1000);
  draw_analogue_clock();
  
  pos1 = circle_positions(90, 40, 50, 50);
  while (1){
    printf("Pos x: %ld Pos y: %ld", pos1.x, pos1.y);
    sleep_ms(1000);
  }



  

  return 0;
}

