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

//Initiliase the from buffer
Button button_a(Pin::A);
Button button_b(Pin::B);
Button button_c(Pin::C);

int main() {



  stdio_init_all();

  int ball_qty = 5;
  sleep_ms(500);

  printf("\n\n=======\nInky pack going to do some drawing\n\n");

  uc8151.set_update_speed(3); //Change EInk Update speed  0=Default, 1=Medium, 2=Fast, 3=Trubo


  struct pt {
    float      x;
    float      y;
    uint8_t    r;
    float     dx;
    float     dy;
    uint16_t pen;
  };

  std::vector<pt> shapes;
  for(int i = 0; i < ball_qty; i++) {
    pt shape;
    shape.x = rand() % 240;
    shape.y = rand() % 135;
    shape.r = (rand() % 10) + 3;
    shape.dx = float(rand() % 255) / 32.0f;
    shape.dy = float(rand() % 255) / 32.0f;
    shape.pen = (rand() % 14);
    shapes.push_back(shape);
  }
  

 uint32_t i = 0;



  while(true) {

    graphics.set_pen(15);
    graphics.clear();

    for(auto &shape : shapes) {
      shape.x += shape.dx;
      shape.y += shape.dy;
      if(shape.x < 0) shape.dx *= -1;
      if(shape.x >= graphics.bounds.w) shape.dx *= -1;
      if(shape.y < 0) shape.dy *= -1;
      if(shape.y >= graphics.bounds.h) shape.dy *= -1;

      graphics.set_pen(shape.pen);
      graphics.circle(Point(shape.x, shape.y), shape.r);
    }

    /*


    std::vector<Point> poly;
    poly.push_back(Point(30, 30));
    poly.push_back(Point(50, 35));
    poly.push_back(Point(70, 25));
    poly.push_back(Point(80, 65));
    poly.push_back(Point(50, 85));
    poly.push_back(Point(30, 45));

    graphics.set_pen(6);
    //pico_display.pixel(Point(0, 0));
    //graphics.polygon(poly);

    graphics.set_pen(4);
    //graphics.triangle(Point(50, 50), Point(130, 80), Point(80, 110));

    graphics.set_pen(15);
    //graphics.line(Point(50, 50), Point(120, 80));
    //graphics.line(Point(20, 20), Point(120, 20));
    //graphics.line(Point(20, 20), Point(20, 120));

    for(int r = 0; r < 30; r++) {
      for(int j = 0; j < 10; j++) {
        float rads = ((M_PI * 2) / 30.0f) * float(r);
        rads += (float(i) / 100.0f);
        rads += (float(j) / 100.0f);
        float cx = sin(rads) * 300.0f;
        float cy = cos(rads) * 300.0f;
        //graphics.line(Point(120, 67), Point(cx + 120, cy + 67));
      }
    }
    */
    // update screen
    uc8151.update(&graphics);
    sleep_ms(1000 / 5);
    i++;
    if (button_a.read()){
      ball_qty++;
      for(int i = 0; i < ball_qty; i++) {
        pt shape;
        shape.x = rand() % 240;
        shape.y = rand() % 135;
        shape.r = (rand() % 10) + 3;
        shape.dx = float(rand() % 255) / 32.0f;
        shape.dy = float(rand() % 255) / 32.0f;
        shape.pen = (rand() % 14);
        shapes.push_back(shape);
      }
    }
    if (button_b.read()){
      ball_qty--;
      for(int i = 0; i < ball_qty; i++) {
        pt shape;
        shape.x = rand() % 240;
        shape.y = rand() % 135;
        shape.r = (rand() % 10) + 3;
        shape.dx = float(rand() % 255) / 32.0f;
        shape.dy = float(rand() % 255) / 32.0f;
        shape.pen = (rand() % 14);
        shapes.push_back(shape);
      }
    }
  }

    return 0;
}

