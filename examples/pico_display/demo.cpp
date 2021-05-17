#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_display.hpp"

using namespace pimoroni;

extern unsigned char image_tif[];
extern unsigned int image_tif_len;

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay pico_display(buffer);
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
  pico_display.init();
  pico_display.set_backlight(100);

  // uint16_t white = pico_display.create_pen(255, 255, 255);
  // uint16_t black = pico_display.create_pen(0, 0, 0);
  // uint16_t red = pico_display.create_pen(255, 0, 0);
  // uint16_t green = pico_display.create_pen(0, 255, 0);
  // uint16_t dark_grey = pico_display.create_pen(20, 40, 60);
  // uint16_t dark_green = pico_display.create_pen(10, 100, 10);
  // uint16_t blue = pico_display.create_pen(0, 0, 255);

  struct pt {
    float      x;
    float      y;
    uint8_t    r;
    float     dx;
    float     dy;
    uint16_t pen;
  };

  std::vector<pt> shapes;
  for(int i = 0; i < 1000; i++) {
    pt shape;
    shape.x = rand() % 240;
    shape.y = rand() % 135;
    shape.r = (rand() % 10) + 3;
    shape.dx = float(rand() % 255) / 128.0f;
    shape.dy = float(rand() % 255) / 128.0f;
    shape.pen = pico_display.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  uint32_t i = 0;
  while(true) {
    pico_display.set_pen(120, 40, 60);
    pico_display.clear();

    for(auto &shape : shapes) {
      shape.x += shape.dx;
      shape.y += shape.dy;
      if(shape.x < 0) shape.dx *= -1;
      if(shape.x >= pico_display.bounds.w) shape.dx *= -1;
      if(shape.y < 0) shape.dy *= -1;
      if(shape.y >= pico_display.bounds.h) shape.dy *= -1;

      pico_display.set_pen(shape.pen);
      pico_display.circle(Point(shape.x, shape.y), shape.r);
    }

    float led_step = fmod(i / 20.0f, M_PI * 2.0f);
    int r = (sin(led_step) * 25.0f) + 25.0f;
    pico_display.set_led(r, r / 1.2f, r);


    std::vector<Point> poly;
    poly.push_back(Point(30, 30));
    poly.push_back(Point(50, 35));
    poly.push_back(Point(70, 25));
    poly.push_back(Point(80, 65));
    poly.push_back(Point(50, 85));
    poly.push_back(Point(30, 45));

    pico_display.set_pen(255, 255, 0);
    pico_display.polygon(poly);


    pico_display.set_pen(0, 255, 255);
    pico_display.triangle(Point(50, 50), Point(130, 80), Point(80, 110));

    pico_display.set_pen(255, 255, 255);
    pico_display.line(Point(50, 50), Point(120, 80));
    pico_display.line(Point(20, 20), Point(120, 20));
    pico_display.line(Point(20, 20), Point(20, 120));

    for(int r = 0; r < 30; r++) {
      for(int j = 0; j < 10; j++) {
        float rads = ((M_PI * 2) / 30.0f) * float(r);
        rads += (float(i) / 100.0f);
        rads += (float(j) / 100.0f);
        float cx = sin(rads) * 300.0f;
        float cy = cos(rads) * 300.0f;
        pico_display.line(Point(120, 67), Point(cx + 120, cy + 67));
      }
    }
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
    pico_display.update();

    i++;
  }

    return 0;
}
