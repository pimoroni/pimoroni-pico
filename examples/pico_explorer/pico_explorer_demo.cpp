#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_explorer.hpp"
#include "picographics_st7789.hpp"
#include "button.hpp"
#include "motor.hpp"
#include "msa301.hpp"
#include "analog.hpp"

using namespace pimoroni;
using namespace motor;

extern unsigned char _binary_fox_tga_start[];

PicoGraphicsST7789 display(
  PicoExplorer::WIDTH,
  PicoExplorer::HEIGHT,
  ROTATE_0,  // Rotation
  false,     // Is it round!?
  nullptr,   // Buffer
  get_spi_pins(BG_SPI_FRONT)
);

// Buttons
Button button_a(PicoExplorer::A);
Button button_b(PicoExplorer::B);
Button button_x(PicoExplorer::X);
Button button_y(PicoExplorer::Y);

Motor motor1(PicoExplorer::MOTOR1_PINS);
Motor motor2(PicoExplorer::MOTOR2_PINS);

Analog adc0(PicoExplorer::ADC0_PIN);
Analog adc1(PicoExplorer::ADC1_PIN);
Analog adc2(PicoExplorer::ADC2_PIN);

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
  motor1.init();
  motor2.init();

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
    shape.pen = display.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  Pen BG = display.create_pen(120, 40, 60);
  Pen WHITE = display.create_pen(255, 255, 255);
  Pen BOX = display.create_pen(55, 65, 75);
  Pen PURPLE = display.create_pen(255, 0, 255);

  uint32_t i = 0;
  while(true) {
    display.set_pen(BG);
    display.clear();

    for(auto &shape : shapes) {
      shape.x += shape.dx;
      shape.y += shape.dy;
      if(shape.x < 0) shape.dx *= -1;
      if(shape.x >= display.bounds.w) shape.dx *= -1;
      if(shape.y < 0) shape.dy *= -1;
      if(shape.y >= display.bounds.h) shape.dy *= -1;

      display.set_pen(shape.pen);
      display.circle(Point(shape.x, shape.y), shape.r);
    }

    float rv = adc0.read_voltage() / 3.3f;
    display.set_pen(WHITE);
    display.circle(Point(rv * 140 + 50, 110), 20);
    display.set_pen(display.create_pen(rv * 255, 0, 0));
    display.circle(Point(rv * 140 + 50, 110), 15);

    float gv = adc1.read_voltage() / 3.3f;
    display.set_pen(WHITE);
    display.circle(Point(gv * 140 + 50, 160), 20);
    display.set_pen(display.create_pen(0, gv * 255, 0));
    display.circle(Point(gv * 140 + 50, 160), 15);

    float bv = adc2.read_voltage() / 3.3f;
    display.set_pen(WHITE);
    display.circle(Point(bv * 140 + 50, 210), 20);
    display.set_pen(display.create_pen(0, 0, bv * 255));
    display.circle(Point(bv * 140 + 50, 210), 15);

    motor1.speed(bv);
    motor2.speed(rv);

    // TODO make this work
    // display.set_tone(440, 0.5);

    if(button_a.read()) {
      display.set_pen(WHITE);
      display.character('A', Point(120, 180), 5);
    }

    if(button_b.read()) {
      display.set_pen(WHITE);
      display.character('B', Point(120, 180), 5);
    }

    if(button_x.read()) {
      display.set_pen(WHITE);
      display.character('X', Point(120, 180), 5);
    }

    if(button_y.read()) {
      display.set_pen(WHITE);
      display.character('Y', Point(120, 180), 5);
    }

    float tyoff = cos(i / 20.0f) * 50.0f - 50.0f;
    Rect text_box(10, 10, 150, 150);
    display.set_pen(BOX);
    display.rectangle(text_box);
    text_box.deflate(10);
    display.set_clip(text_box);
    display.set_pen(WHITE);
    display.text("This is a test of some text data that should wrap nicely onto multiple lines which is dead useful like.", Point(text_box.x, text_box.y + tyoff), 100);

    float xoff = sin(i / 20.0f) * 50.0f;
    xoff += 120 - (81 / 2);
    float yoff = cos(i / 20.0f) * 50.0f;
    yoff += 120 - (68 / 2);
    for(int y = 0; y < 68; y++) {
      // uint16_t *dest = display.frame_buffer + (y * 240);
      uint8_t *src = _binary_fox_tga_start + 18 + (y * 81 * 3);
      for(int x = 0; x < 81; x++) {
        uint8_t b = *src++;
        uint8_t g = *src++;
        uint8_t r = *src++;

        display.set_pen(display.create_pen(r, g, b));
        display.pixel(Point(x + xoff, 68 - y + yoff));
      }
    }

    display.remove_clip();

    display.set_pen(WHITE);
    display.text("x: " + std::to_string(int(msa301.get_axis(msa301.X, 16) * 100)), Point(10, 190), 100);
    display.text("y: " + std::to_string(int(msa301.get_axis(msa301.Y, 16) * 100)), Point(10, 205), 100);
    display.text("z: " + std::to_string(int(msa301.get_axis(msa301.Z, 16) * 100)), Point(10, 220), 100);

    uint16_t xpos = (msa301.get_axis(msa301.X, 16) * 120) + 120;
    uint16_t ypos = (msa301.get_axis(msa301.Z, 16) * 120) + 120;
    display.set_pen(WHITE);
    display.circle(Point(xpos, ypos), 20);
    display.set_pen(PURPLE);
    display.circle(Point(xpos, ypos), 15);


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
    display.update();

    i++;
  }

    return 0;
}
