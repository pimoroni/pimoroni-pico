#include <cstdio>
#include <math.h>

#include <stdio.h>
#include "hardware/gpio.h"
#include "drivers/dv_display/dv_display.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480

int main() {
  set_sys_clock_khz(200000, true);

  stdio_init_all();

  constexpr uint BUTTON_A = 9;
  gpio_init(BUTTON_A);
  gpio_set_dir(BUTTON_A, GPIO_IN);
  gpio_pull_up(BUTTON_A);

  sleep_ms(5000);

  DVDisplay display(FRAME_WIDTH, FRAME_HEIGHT);
  display.init();
  //display.test();

  PicoGraphics_PenDV_RGB555 graphics(FRAME_WIDTH, FRAME_HEIGHT, display);

  graphics.set_pen(0x001F);
  graphics.clear();
  display.flip();
  sleep_ms(2000);
  graphics.set_pen(0x7C00);
  graphics.clear();
  display.flip();

  printf("Starting\n");

  constexpr int NUM_CIRCLES = 50;
  struct Circle {
    uint16_t x, y, size, grow;
  } circles[NUM_CIRCLES];

  for(int i =0 ; i < 50 ; i++)
  {
    circles[i].size = (rand() % 50) + 1;
    circles[i].grow = std::max(0, (rand() % 50) - 25);
    circles[i].x = rand() % graphics.bounds.w;
    circles[i].y = rand() % graphics.bounds.h;
  }

  while (true) {
    //while(gpio_get(BUTTON_A) == 1) {
    //  sleep_ms(10);
    //}
    uint32_t render_start_time = time_us_32();

    graphics.set_pen(0xFFFF);
    graphics.clear();

    for(int i =0 ; i < NUM_CIRCLES ; i++)
    {
      graphics.set_pen(0);
      graphics.circle(Point(circles[i].x, circles[i].y), circles[i].size);

      graphics.set_pen(RGB::from_hsv(i * 0.02f, 1.0f, 1.0f).to_rgb555());
      graphics.circle(Point(circles[i].x, circles[i].y), circles[i].size-2);
      if (circles[i].grow) {
        circles[i].size++;
        circles[i].grow--;
      } else {
        circles[i].size--;
        if (circles[i].size == 0) {
          circles[i].size = 1;
          circles[i].grow = rand() % 75;
          circles[i].x = rand() % graphics.bounds.w;
          circles[i].y = rand() % graphics.bounds.h;
        }
      }
    }

#if 0
    uint x = 260; //rand() % graphics.bounds.w;
    uint y = 468; //rand() % graphics.bounds.h;
    printf("Circle at (%d, %d)\n", x, y);
    graphics.set_pen(0);
    graphics.circle(Point(x, y), 25);
  #endif

    uint32_t render_time = time_us_32() - render_start_time;

    uint32_t flip_start_time = time_us_32();
    display.flip();
    uint32_t flip_time = time_us_32() - flip_start_time;
    printf("Render: %.3f, flip: %.3f\n", render_time / 1000.f, flip_time / 1000.f);
  }
}
