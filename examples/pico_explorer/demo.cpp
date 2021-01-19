#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_explorer.hpp"
#include "msa301.hpp"

using namespace pimoroni;

extern unsigned char _binary_fox_tga_start[];

PicoExplorer pico_explorer;
MSA301 msa301;

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
  msa301.init();

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
    pico_explorer.clear();

    for(auto &shape : shapes) {
      shape.x += shape.dx;
      shape.y += shape.dy;
      if(shape.x < 0) shape.dx *= -1;
      if(shape.x >= pico_explorer.bounds.w) shape.dx *= -1;
      if(shape.y < 0) shape.dy *= -1;
      if(shape.y >= pico_explorer.bounds.h) shape.dy *= -1;

      pico_explorer.set_pen(shape.pen);
      pico_explorer.circle(point(shape.x, shape.y), shape.r);
    }

    float rv = pico_explorer.get_adc(pico_explorer.ADC0);
    pico_explorer.set_pen(255, 255, 255);
    pico_explorer.circle(point(rv * 140 + 50, 110), 20);
    pico_explorer.set_pen(rv * 255, 0, 0);
    pico_explorer.circle(point(rv * 140 + 50, 110), 15);

    float gv = pico_explorer.get_adc(pico_explorer.ADC1);
    pico_explorer.set_pen(255, 255, 255);
    pico_explorer.circle(point(gv * 140 + 50, 160), 20);
    pico_explorer.set_pen(0, gv * 255, 0);
    pico_explorer.circle(point(gv * 140 + 50, 160), 15);

    float bv = pico_explorer.get_adc(pico_explorer.ADC2);
    pico_explorer.set_pen(255, 255, 255);
    pico_explorer.circle(point(bv * 140 + 50, 210), 20);
    pico_explorer.set_pen(0, 0, bv * 255);
    pico_explorer.circle(point(bv * 140 + 50, 210), 15);

    pico_explorer.set_motor(pico_explorer.MOTOR1, pico_explorer.FORWARD, bv);
    pico_explorer.set_motor(pico_explorer.MOTOR2, pico_explorer.FORWARD, rv);

    pico_explorer.set_tone(440, 0.5);

    if(pico_explorer.is_pressed(pico_explorer.A)) {
      pico_explorer.set_pen(255, 255, 255);
      pico_explorer.character('A', point(120, 180), 5);
    }

    if(pico_explorer.is_pressed(pico_explorer.B)) {
      pico_explorer.set_pen(255, 255, 255);
      pico_explorer.character('B', point(120, 180), 5);
    }

    if(pico_explorer.is_pressed(pico_explorer.X)) {
      pico_explorer.set_pen(255, 255, 255);
      pico_explorer.character('X', point(120, 180), 5);
    }

    if(pico_explorer.is_pressed(pico_explorer.Y)) {
      pico_explorer.set_pen(255, 255, 255);
      pico_explorer.character('Y', point(120, 180), 5);
    }

    float tyoff = cos(i / 20.0f) * 50.0f - 50.0f;
    rect text_box(10, 10, 150, 150);
    pico_explorer.set_pen(55, 65, 75);
    pico_explorer.rectangle(text_box);
    text_box.deflate(10);
    pico_explorer.set_clip(text_box);
    pico_explorer.set_pen(255, 255, 255);
    pico_explorer.text("This is a test of some text data that should wrap nicely onto multiple lines which is dead useful like.", point(text_box.x, text_box.y + tyoff), 100);

    float xoff = sin(i / 20.0f) * 50.0f;
    xoff += 120 - (81 / 2);
    float yoff = cos(i / 20.0f) * 50.0f;
    yoff += 120 - (68 / 2);
    for(int y = 0; y < 68; y++) {
      uint16_t *dest = pico_explorer.frame_buffer + (y * 240);
      uint8_t *src = _binary_fox_tga_start + 18 + (y * 81 * 3);
      for(int x = 0; x < 81; x++) {
        uint8_t r = *src++;
        uint8_t g = *src++;
        uint8_t b = *src++;

        pico_explorer.set_pen(r, g, b);
        pico_explorer.pixel(point(x + xoff, 68 - y + yoff));
      }
    }

    pico_explorer.remove_clip();

    pico_explorer.set_pen(255, 255, 255);
    pico_explorer.text("x: " + std::to_string(int(msa301.get_axis(msa301.X, 16) * 100)), point(10, 190), 100);
    pico_explorer.text("y: " + std::to_string(int(msa301.get_axis(msa301.Y, 16) * 100)), point(10, 205), 100);
    pico_explorer.text("z: " + std::to_string(int(msa301.get_axis(msa301.Z, 16) * 100)), point(10, 220), 100);

    uint16_t xpos = (msa301.get_axis(msa301.X, 16) * 120) + 120;
    uint16_t ypos = (msa301.get_axis(msa301.Z, 16) * 120) + 120;
    pico_explorer.set_pen(255, 255, 255);
    pico_explorer.circle(point(xpos, ypos), 20);
    pico_explorer.set_pen(255, 0, 255);
    pico_explorer.circle(point(xpos, ypos), 15);


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
