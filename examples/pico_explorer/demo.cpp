#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_explorer.hpp"

using namespace pimoroni;

extern unsigned char image_tif[];
extern unsigned int image_tif_len;

PicoExplorer pico_explorer;
/*
void pixel(int x, int y, uint16_t c) {
  x *= 2;
  y *= 2;
  pico_display.frame_buffer[x + y * 240] = c;
  pico_display.frame_buffer[x + 1 + y * 240] = c;
  pico_display.frame_buffer[x + 1 + (y + 1) * 240] = c;
  pico_display.frame_buffer[x + (y + 1) * 240] = c;
}

void rect(int x, int y, int w, int h, uint16_t c) {
  for(int rx = x; rx < x + w; rx++) {
    for(int ry = y; ry < y + h; ry++) {
      pixel(rx, ry, c);
    }
  }
}*/

uint8_t arrow[] = {
    0b00010000,
    0b00110000,
    0b01110000,
    0b11111111,
    0b11111111,
    0b01110000,
    0b00110000,
    0b00010000
  };

uint8_t tick[] = {
    0b00000000,
    0b00000010,
    0b00000111,
    0b01001110,
    0b11111100,
    0b00111000,
    0b00010000,
    0b00000000,
  };

/*
void sprite(uint8_t *p, int x, int y, bool flip, uint16_t c) {
  for(int ay = 0; ay < 8; ay++) {
    uint8_t sl = p[ay];
    for(int ax = 0; ax < 8; ax++) {
      if(flip) {
        if((0b10000000 >> ax) & sl) {
          pixel(ax + x, ay + y, c);
        }
      }else{
        if((0b1 << ax) & sl) {
          pixel(ax + x, ay + y, c);
        }
      }
    }
  }
}*/

int main() {
  pico_explorer.set_backlight(100);

  // uint16_t white = pico_display.create_pen(255, 255, 255);
  // uint16_t black = pico_display.create_pen(0, 0, 0);
  // uint16_t red = pico_display.create_pen(255, 0, 0);
  // uint16_t green = pico_display.create_pen(0, 255, 0);
  // uint16_t dark_grey = pico_display.create_pen(20, 40, 60);
  // uint16_t dark_green = pico_display.create_pen(10, 100, 10);
  // uint16_t blue = pico_display.create_pen(0, 0, 255);

  bool a_pressed = false;
  bool b_pressed = false;
  bool x_pressed = false;
  bool y_pressed = false;

  struct pt {
    float      x;
    float      y;
    uint8_t    r;
    float     dx;
    float     dy;
    uint16_t pen;
  };

  std::vector<pt> shapes;
  for(int i = 0; i < 100; i++) {
    pt shape;
    shape.x = rand() % 240;
    shape.y = rand() % 135;
    shape.r = (rand() % 10) + 3;
    shape.dx = float(rand() % 255) / 128.0f;
    shape.dy = float(rand() % 255) / 128.0f;
    shape.pen = pico_explorer.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  pico_explorer.set_audio_pin(pico_explorer.GP0);
  uint32_t i = 0;
  while(true) {
    pico_explorer.set_pen(120, 40, 60);
    pico_explorer.rectangle(0, 0, pico_explorer.width, pico_explorer.height);

    for(auto &shape : shapes) {
      shape.x += shape.dx;
      shape.y += shape.dy;
      if(shape.x < 0) shape.dx *= -1;
      if(shape.x >= pico_explorer.width) shape.dx *= -1;
      if(shape.y < 0) shape.dy *= -1;
      if(shape.y >= pico_explorer.height) shape.dy *= -1;

      pico_explorer.set_pen(shape.pen);
      pico_explorer.circle(shape.x, shape.y, shape.r);
    }

    pico_explorer.set_pen(255, 255, 255);
    pico_explorer.text("This is a test of some text data that should wrap nicely onto multiple lines which is dead useful like.", 10, 10, 180);

    float rv = pico_explorer.get_adc(pico_explorer.ADC0);
    pico_explorer.set_pen(255, 255, 255);
    pico_explorer.circle(rv * 140 + 50, 110, 20);
    pico_explorer.set_pen(rv * 255, 0, 0);
    pico_explorer.circle(rv * 140 + 50, 110, 15);

    float gv = pico_explorer.get_adc(pico_explorer.ADC1);
    pico_explorer.set_pen(255, 255, 255);
    pico_explorer.circle(gv * 140 + 50, 160, 20);
    pico_explorer.set_pen(0, gv * 255, 0);
    pico_explorer.circle(gv * 140 + 50, 160, 15);

    float bv = pico_explorer.get_adc(pico_explorer.ADC2);
    pico_explorer.set_pen(255, 255, 255);
    pico_explorer.circle(bv * 140 + 50, 210, 20);
    pico_explorer.set_pen(0, 0, bv * 255);
    pico_explorer.circle(bv * 140 + 50, 210, 15);

    pico_explorer.set_motor(pico_explorer.MOTOR1, pico_explorer.FORWARD, bv);

    pico_explorer.set_tone(100 + (bv * 1000), rv);
/*
    if(pico_display.is_pressed(pico_display.A)) {
      pico_display.rectangle(0, 0, 18, 18);
      //sprite(tick, 5, 5, true, green);
    }else{
      //sprite(arrow, 10 + bounce, 10, true, white);
    }

    if(pico_display.is_pressed(pico_display.B)) {
      pico_display.rectangle(0, 49, 18, 18);
      //sprite(tick, 5, 54, true, green);
    }else{
      //sprite(arrow, 10 - bounce, 50, true, white);
    }


    if(pico_display.is_pressed(pico_display.X)) {
      pico_display.rectangle(102, 0, 18, 18);
      //sprite(tick, 107, 5, true, green);
    }else{
      //sprite(arrow, 102 - bounce, 10, false, white);
    }

    if(pico_display.is_pressed(pico_display.Y)) {
      pico_display.rectangle(102, 49, 18, 18);
      //sprite(tick, 107, 54, true, green);
    }else{
      //sprite(arrow, 102 + bounce, 50, false, white);
    }
*/
    // update screen
    pico_explorer.update();

    i++;
  }

    return 0;
}
