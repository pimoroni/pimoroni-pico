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
#include "hardware/rtc.h"
#include "pico/util/datetime.h"


#define CLOCK_RADIUS_PX 64
#define CLOCK_RIM 5
#define MIN_LENGTH 45
#define HR_LENGTH 30
#define SEC_LEGNTH 60
#define CLOCK_LOC_X 146
#define CLOCK_LOC_Y 64

// Helpful for converting between Deg and Rad
#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)

typedef struct PointCords {
  uint32_t x;
  uint32_t y;
}PointCords;

PointCords circle_positions(uint16_t angle, uint8_t radius, uint8_t center_x, uint8_t center_y){

  PointCords position;
  position.x = (sin(degToRad(360 - angle)) * radius) + center_x;
  position.y = (cos(degToRad(360 - angle)) * radius) + center_y;
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

// RTC setup 
// Set you time here
    datetime_t t = {
        .year  = 2020,
        .month = 01,
        .day   = 13,
        .dotw  = 3, // 0 is Sunday, so 3 is Wednesday
        .hour  = 11,
        .min   = 20,
        .sec   = 00
    };

void draw_number(const std::string &t, uint16_t angle, uint8_t radius, uint8_t center_x, uint8_t center_y){
  PointCords circPositions;
  circPositions =  circle_positions(angle, radius, center_x, center_y);
  graphics.text(t, Point(circPositions.x, circPositions.y), false);  
}

void draw_radial_marks(uint16_t angle, uint8_t radius, uint8_t length, uint8_t center_x, uint8_t center_y){
  PointCords startPos;
  PointCords endPos;
  startPos = circle_positions(angle, radius, center_x, center_y);
  endPos = circle_positions(angle, radius + length, center_x, center_y);
  graphics.line(Point(startPos.x, startPos.y), Point(endPos.x, endPos.y));
}

void draw_hands(uint8_t hr, uint8_t min, uint8_t sec){
  draw_radial_marks(30*hr + 180 , 0, HR_LENGTH, CLOCK_LOC_X, CLOCK_LOC_Y);
  draw_radial_marks(6*min + 180 , 0, MIN_LENGTH, CLOCK_LOC_X, CLOCK_LOC_Y);
  draw_radial_marks(6*sec + 180 , 0, MIN_LENGTH, CLOCK_LOC_X, CLOCK_LOC_Y);
}

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

  // Draw numbers
  graphics.set_pen(0);
  draw_number("12", 175, 60, CLOCK_LOC_X, CLOCK_LOC_Y);
  draw_number("3", 265, 50, CLOCK_LOC_X, CLOCK_LOC_Y);
  draw_number("6", 5, 50, CLOCK_LOC_X, CLOCK_LOC_Y);
  draw_number("9", 95, 60, CLOCK_LOC_X, CLOCK_LOC_Y);


  draw_radial_marks(30, 50, 10, CLOCK_LOC_X, CLOCK_LOC_Y);
  draw_radial_marks(60, 50, 10, CLOCK_LOC_X, CLOCK_LOC_Y);
  draw_radial_marks(120, 50, 10, CLOCK_LOC_X, CLOCK_LOC_Y);
  draw_radial_marks(150, 50, 10, CLOCK_LOC_X, CLOCK_LOC_Y);
  draw_radial_marks(210, 50, 10, CLOCK_LOC_X, CLOCK_LOC_Y);
  draw_radial_marks(240, 50, 10, CLOCK_LOC_X, CLOCK_LOC_Y);
  draw_radial_marks(300, 50, 10, CLOCK_LOC_X, CLOCK_LOC_Y);
  draw_radial_marks(330, 50, 10, CLOCK_LOC_X, CLOCK_LOC_Y);
  

}


PointCords pos1;


int main() {

  stdio_init_all();
  uc8151.set_update_speed(3); //set turbo update speed

// RTC setup 
  datetime_t t = {
      .year  = 2020,
      .month = 01,
      .day   = 13,
      .dotw  = 3, // 0 is Sunday, so 3 is Wednesday
      .hour  = 02,
      .min   = 10,
      .sec   = 00
  };

  // Start the RTC
  rtc_init();
  rtc_set_datetime(&t);

  sleep_ms(2000);
  draw_analogue_clock();
  
  bool time_running = true;
  
  while (1){

    // Draw Clockface 
    draw_analogue_clock();
    if (!time_running){
        graphics.text("Set time: B = Hour, C = Seconds", Point(0,0), false);
      }
    if (button_a.read()){
      time_running = !time_running;
      if (!time_running){
        graphics.text("Set time: B = Hour, C = Seconds", Point(0,0), false);
      }
      uc8151.update(&graphics);
      
    }
    
    if( time_running){
    // Draw hands
    rtc_get_datetime(&t);
    printf( "the time is %d : %d : %d \n", t.hour, t.min, t.sec);
    draw_hands(t.hour, t.min, t.sec);
    uc8151.update(&graphics);
    sleep_ms(2000);
    }
    else{
      // Set time mode
      rtc_get_datetime(&t);
      draw_hands(t.hour, t.min, t.sec);
      if (button_b.read()){
        t.hour++;
        if (t.hour > 23){
          t.hour = 0;
        }
        rtc_set_datetime(&t);
        uc8151.update(&graphics);
      }
      if (button_c.read()){
        t.min++;
        if (t.min > 59){
          t.min = 0;
        }
        rtc_set_datetime(&t);
        uc8151.update(&graphics);
      }
      sleep_ms(1000);
    }

    
  }

  return 0;
}

