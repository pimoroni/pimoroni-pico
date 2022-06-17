#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_explorer.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "button.hpp"
#include "motor.hpp"
#include "msa301.hpp"
#include "analog.hpp"

using namespace pimoroni;
using namespace motor;

extern unsigned char _binary_fox_tga_start[];

ST7789 st7789(PicoExplorer::WIDTH, PicoExplorer::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

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
    shape.pen = graphics.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  Pen BG = graphics.create_pen(120, 40, 60);
  Pen WHITE = graphics.create_pen(255, 255, 255);
  Pen BOX = graphics.create_pen(55, 65, 75);
  Pen PURPLE = graphics.create_pen(255, 0, 255);

  uint32_t i = 0;
  while(true) {
    graphics.set_pen(BG);
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

    float rv = adc0.read_voltage() / 3.3f;
    graphics.set_pen(WHITE);
    graphics.circle(Point(rv * 140 + 50, 110), 20);
    graphics.set_pen(graphics.create_pen(rv * 255, 0, 0));
    graphics.circle(Point(rv * 140 + 50, 110), 15);

    float gv = adc1.read_voltage() / 3.3f;
    graphics.set_pen(WHITE);
    graphics.circle(Point(gv * 140 + 50, 160), 20);
    graphics.set_pen(graphics.create_pen(0, gv * 255, 0));
    graphics.circle(Point(gv * 140 + 50, 160), 15);

    float bv = adc2.read_voltage() / 3.3f;
    graphics.set_pen(WHITE);
    graphics.circle(Point(bv * 140 + 50, 210), 20);
    graphics.set_pen(graphics.create_pen(0, 0, bv * 255));
    graphics.circle(Point(bv * 140 + 50, 210), 15);

    motor1.speed(bv);
    motor2.speed(rv);

    // TODO make this work
    // display.set_tone(440, 0.5);

    if(button_a.read()) {
      graphics.set_pen(WHITE);
      graphics.character('A', Point(120, 180), 5);
    }

    if(button_b.read()) {
      graphics.set_pen(WHITE);
      graphics.character('B', Point(120, 180), 5);
    }

    if(button_x.read()) {
      graphics.set_pen(WHITE);
      graphics.character('X', Point(120, 180), 5);
    }

    if(button_y.read()) {
      graphics.set_pen(WHITE);
      graphics.character('Y', Point(120, 180), 5);
    }

    float tyoff = cos(i / 20.0f) * 50.0f - 50.0f;
    Rect text_box(10, 10, 150, 150);
    graphics.set_pen(BOX);
    graphics.rectangle(text_box);
    text_box.deflate(10);
    graphics.set_clip(text_box);
    graphics.set_pen(WHITE);
    graphics.text("This is a test of some text data that should wrap nicely onto multiple lines which is dead useful like.", Point(text_box.x, text_box.y + tyoff), 100);

    float xoff = sin(i / 20.0f) * 50.0f;
    xoff += 120 - (81 / 2);
    float yoff = cos(i / 20.0f) * 50.0f;
    yoff += 120 - (68 / 2);
    for(int y = 0; y < 68; y++) {
      uint8_t *src = _binary_fox_tga_start + 18 + (y * 81 * 3);
      for(int x = 0; x < 81; x++) {
        uint8_t b = *src++;
        uint8_t g = *src++;
        uint8_t r = *src++;

        graphics.set_pen(graphics.create_pen(r, g, b));
        graphics.pixel(Point(x + xoff, 68 - y + yoff));
      }
    }

    graphics.remove_clip();

    graphics.set_pen(WHITE);
    graphics.text("x: " + std::to_string(int(msa301.get_axis(msa301.X, 16) * 100)), Point(10, 190), 100);
    graphics.text("y: " + std::to_string(int(msa301.get_axis(msa301.Y, 16) * 100)), Point(10, 205), 100);
    graphics.text("z: " + std::to_string(int(msa301.get_axis(msa301.Z, 16) * 100)), Point(10, 220), 100);

    uint16_t xpos = (msa301.get_axis(msa301.X, 16) * 120) + 120;
    uint16_t ypos = (msa301.get_axis(msa301.Z, 16) * 120) + 120;
    graphics.set_pen(WHITE);
    graphics.circle(Point(xpos, ypos), 20);
    graphics.set_pen(PURPLE);
    graphics.circle(Point(xpos, ypos), 15);

    // update screen
    st7789.update(&graphics);

    i++;
  }

    return 0;
}
