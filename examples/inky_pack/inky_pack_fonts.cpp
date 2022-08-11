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

std::array<std::string, 5> font_names = {
  "sans",  "gothic", "cursive", "serif"
};
int8_t selected_font = 0;

void draw() {
  graphics.set_pen(15);
  graphics.clear();

  graphics.set_font("sans");
  for(int i = 0; i < int(font_names.size()); i++) {
    std::string name = font_names[i];

    if(selected_font == i) {
      graphics.set_pen(0);
      graphics.rectangle(Rect(0, i * 16, 80, 16));
      graphics.set_pen(15);
    }else{
      graphics.set_pen(0);
    }

    graphics.text(name, Point( 2, i * 16 + 7), 296, 0.5f);
  }

  graphics.set_font(font_names[selected_font]);
  //graphics.thickness(2);
  graphics.text("The quick", Point(100, 10), 296, 0.8f, 1.0f);
  graphics.text("brown fox", Point(100, 32), 296, 0.8f);
  graphics.text("jumped over", Point(100, 54), 296, 0.8f);
  graphics.text("the lazy dog.", Point(100, 76), 296, 0.8f);
  graphics.text("0123456789", Point(100, 98), 296, 0.8f);
  graphics.text("!\"£$%^&*()", Point(100, 120), 296, 0.8f);
  //graphics.thickness(1);

  uc8151.update(&graphics);
}

int main() {

  stdio_init_all();

  sleep_ms(500);

  printf("\n\n=======\nInky Pack starting up\n\n");

  
  uc8151.set_update_speed(1);
  bool update_now = true;
  while(true) {
    
    if (update_now){
      printf("> drawing..");
      draw();
      update_now = false;
      printf("done!\n");
    }

    if(button_a.read()) {
      printf("> A pressed\n");
      selected_font--;
      update_now = true;
    }

    if(button_b.read()) {
      printf("> B pressed\n");
      selected_font++;
      update_now = true;
    }

    if(button_c.read()) {
      printf("> C pressed\n");
      selected_font = 0;
      update_now = true;
    }
    
    selected_font = selected_font < 0 ? int(font_names.size()) - 1 : selected_font;
    selected_font = selected_font >= int(font_names.size()) ? 0 : selected_font;
    if (update_now){
      printf("> newly selected font is %s (%d)\n", font_names[selected_font].c_str(), selected_font);
    }    
  }
}
