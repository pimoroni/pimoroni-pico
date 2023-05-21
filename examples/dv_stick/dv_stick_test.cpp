#include <cstdio>
#include <math.h>

#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "drivers/dv_display/dv_display.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

#define FRAME_WIDTH 720
#define FRAME_HEIGHT 480

#define READ_EDID 1
#if READ_EDID
extern "C" { int decode_edid(unsigned char* edid); }
#endif

void on_uart_rx() {
    while (uart_is_readable(uart1)) {
        uint8_t ch = uart_getc(uart1);
        putc(ch, stdout);
    }
}

int main() {
  set_sys_clock_khz(216000, true);

  stdio_init_all();

  // Relay UART RX from the display driver
  gpio_set_function(5, GPIO_FUNC_UART);
  uart_init(uart1, 115200);
  uart_set_hw_flow(uart1, false, false);
  uart_set_format(uart1, 8, 1, UART_PARITY_NONE);
  uart_set_fifo_enabled(uart1, false);
  irq_set_exclusive_handler(UART1_IRQ, on_uart_rx);
  irq_set_enabled(UART1_IRQ, true);
  uart_set_irq_enables(uart1, true, false);

  constexpr uint BUTTON_A = 9;
  gpio_init(BUTTON_A);
  gpio_set_dir(BUTTON_A, GPIO_IN);
  gpio_pull_up(BUTTON_A);

  //sleep_ms(5000);

  DVDisplay display(FRAME_WIDTH, FRAME_HEIGHT);
  display.init();
  //display.test();

#if READ_EDID
  sleep_ms(5000);

  {
    uint8_t edid[128];
    display.get_edid(edid);
    sleep_ms(50);
    decode_edid(edid);
  }
#endif

  PicoGraphics_PenDV_RGB555 graphics(FRAME_WIDTH, FRAME_HEIGHT, display);

  graphics.set_pen(0x001F);
  graphics.clear();
  display.flip();
  sleep_ms(2000);
  graphics.set_pen(0x7C00);
  graphics.clear();
  display.flip();

  printf("Starting\n");
  graphics.set_font("bitmap8");

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

  int frames = 0;
  while (true) {
    //while(gpio_get(BUTTON_A) == 1) {
    //  sleep_ms(10);
    //}
    uint32_t render_start_time = time_us_32();

    graphics.set_pen(0xFFFF);
    graphics.clear();

    for (uint i = 0; i < 128; i++) {
      for (uint j = 0; j < 256; j++) {
        graphics.set_pen((j << 7) | i);
        graphics.pixel(Point(j, i));
      }
    }

#if 0
    for (uint i = 0; i < 128; i++) {
      for (uint j = 0; j < 256; j++) {
        graphics.set_pen((j << 7) | i);
        graphics.pixel(Point(i, j+128));
      }
    }
#endif

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

    char buffer[8];
    sprintf(buffer, "%s %s %s", 
            gpio_get(BUTTON_A) == 0 ? "A" : " ", 
            display.is_button_b_pressed() ? "B" : " ",
            display.is_button_c_pressed() ? "C" : " ");
    graphics.set_pen(0);
    graphics.text(buffer, {500,10}, FRAME_WIDTH - 500, 3);

    uint32_t flip_start_time = time_us_32();
    display.flip();
    uint32_t flip_time = time_us_32() - flip_start_time;
    printf("Render: %.3f, flip: %.3f\n", render_time / 1000.f, flip_time / 1000.f);

    ++frames;
    if (gpio_get(BUTTON_A) == 0) display.set_led_level((uint8_t)frames);
    else display.set_led_heartbeat();
  }
}
