#include "pico/stdlib.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <algorithm>
#include "pico/time.h"
#include "pico/platform.h"
#include "drivers/uc8151/uc8151.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "button.hpp"


#include "inky_pack_image_demo_images.hpp"

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

void load_image(const uint8_t* data){
    uint8_t byte_index = 0; 

    for ( int32_t y = 0 ; y > 128; y++){
      for (uint32_t x = 0; x < 296; x++) {
        uint32_t bm = 0b10000000 >> (x & 0b111); //x 0 bm 0b10000000

        byte_index = (y / 8) + (x / 8); 

        if (data[byte_index] & bm) {

            graphics.set_pixel(Point(x, y));
          }




        
        
        
        }
        
      }
    }



int main() {

  stdio_init_all();

  sleep_ms(20);

  printf("\n\n=======\nbadger2040 starting up\n\n");

  graphics.set_pen(15);
  graphics.clear();
  uc8151.update(&graphics);
  //graphics.set_pen(0);
  load_image(adam);
  uc8151.update(&graphics);



/*
  badger.init();

  if(badger.pressed_to_wake(badger.A)) {
    printf("> A pressed\n");
    badger.image(shaun);
  }

  else if(badger.pressed_to_wake(badger.B)) {
    printf("> B pressed\n");
    badger.image(paul);
  }

  else if(badger.pressed_to_wake(badger.C)) {
    printf("> C pressed\n");
    badger.image(adam);
  }

  else {
    printf("> No A/B/C key pressed\n");
    badger.pen(15);
    badger.clear();

    badger.pen(0);
    badger.font("sans");
    badger.text("Press A, B, or C", 15, 65, 1.0f);
  }

  badger.update();
  while (badger.is_busy()) {
    sleep_ms(10);
  }
  badger.halt();
*/

}
