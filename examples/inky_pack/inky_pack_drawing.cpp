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

  int ball_qty = 20;
  sleep_ms(500);

  printf("\n\n=======\nInky pack going to do some drawing\n\n");

  uc8151.set_update_speed(1); //Change EInk Update speed  0=Default, 1=Medium, 2=Fast, 3=Trubo


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

    // update screen
    uc8151.update(&graphics);
    sleep_ms(1000 / 1);
    i++;
    if (button_a.read()){
      pt shape;
      shape.x = rand() % 240;
      shape.y = rand() % 135;
      shape.r = (rand() % 10) + 3;
      shape.dx = float(rand() % 255) / 32.0f;
      shape.dy = float(rand() % 255) / 32.0f;
      shape.pen = (rand() % 14);
      shapes.push_back(shape);
      
    }
    
    
    if (button_b.read()){
      shapes.pop_back();

    }
    
  }

    return 0;
}

