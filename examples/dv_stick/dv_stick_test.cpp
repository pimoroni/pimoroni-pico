#include <cstdio>
#include <math.h>

#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "drivers/dv_display/dv_display.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

#define DISPLAY_WIDTH 360
#define DISPLAY_HEIGHT 480

#define FRAME_WIDTH 1000
#define FRAME_HEIGHT 480

#define READ_EDID 0
#if READ_EDID
extern "C" { int decode_edid(unsigned char* edid); }
#endif

#define USE_PALETTE 0

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

  DVDisplay display;
  display.init(DISPLAY_WIDTH, DISPLAY_HEIGHT, DVDisplay::MODE_RGB888, FRAME_WIDTH, FRAME_HEIGHT);
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

#if USE_PALETTE
  display.set_mode(DVDisplay::MODE_PALETTE);
  PicoGraphics_PenDV_P5 graphics(FRAME_WIDTH, FRAME_HEIGHT, display);
#else
  //display.set_mode(DVDisplay::MODE_RGB888);
  PicoGraphics_PenDV_RGB888 graphics(FRAME_WIDTH, FRAME_HEIGHT, display);
#endif

  graphics.create_pen(0, 0, 0);
  graphics.create_pen(0xFF, 0xFF, 0xFF);

#if USE_PALETTE
  for (int i = 0; i < 25; ++i) {
    graphics.create_pen_hsv(i * 0.04f, 1.0f, 1.0f);
  }
  for (int i = 0; i < 5; ++i) {
    graphics.create_pen((i+3) * (255/8), 255, 255);
  }
#endif

  graphics.set_pen(0xFF, 0, 0);
  printf(".\n");
  graphics.clear();
  printf("..\n");
  display.flip();
  printf("...\n");
  sleep_ms(2000);
  graphics.set_pen(0, 0, 0xFF);
  graphics.clear();
  display.flip();

  printf("Starting\n");
  graphics.set_font("bitmap8");

  Point scroll = {0, 0};
  int scroll_dir = 1;

  constexpr int NUM_CIRCLES = 50;
  struct Circle {
    uint16_t x, y, size, grow;
    uint32_t pen;
  } circles[NUM_CIRCLES];

  for(int i =0 ; i < 50 ; i++)
  {
    circles[i].size = (rand() % 50) + 1;
    circles[i].grow = std::max(0, (rand() % 50) - 25);
    circles[i].x = rand() % graphics.bounds.w;
    circles[i].y = rand() % graphics.bounds.h;
#if USE_PALETTE
    circles[i].pen = 2 + (i >> 1);
#else
    circles[i].pen = graphics.create_pen_hsv(i * 0.02f, 1.0f, 1.0f);
#endif
  }

  int frames = 0;
  while (true) {
    //while(gpio_get(BUTTON_A) == 1) {
    //  sleep_ms(10);
    //}
    uint32_t render_start_time = time_us_32();

#if 1
    for (int j = 0; j < FRAME_HEIGHT; ++j) {
      graphics.set_pen(j & 0xFF, 0xFF, 0xFF);
      graphics.pixel_span({0,j}, FRAME_WIDTH);
    }
#else
    graphics.set_pen(0xFF, 0xFF, 0xFF);
    graphics.clear();
#endif

#if 0
    for (uint i = 0; i < 128; i++) {
      for (uint j = 0; j < 256; j++) {
        RGB555 col = (j << 7) | i;
        graphics.set_pen((col << 3) & 0xF8, (col >> 2) & 0xF8, (col >> 7) & 0xF8);
        graphics.pixel(Point(j, i));
      }
    }

    for (uint i = 0; i < 128; i++) {
      for (uint j = 0; j < 256; j++) {
        graphics.set_pen((j << 7) | i);
        graphics.pixel(Point(i, j+128));
      }
    }
#endif

    for(int i =0 ; i < NUM_CIRCLES ; i++)
    {
      graphics.set_pen(0, 0, 0);
      graphics.circle(Point(circles[i].x, circles[i].y), circles[i].size);

      //RGB col = RGB::from_hsv(i * 0.02f, 1.0f, 1.0f);
      //graphics.set_pen(col.r, col.g, col.b);
      graphics.set_pen(circles[i].pen);
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
    graphics.set_pen(0, 0, 0);
    graphics.text(buffer, {500,10}, FRAME_WIDTH - 500, 3);

    uint32_t flip_start_time = time_us_32();
    display.flip();
    uint32_t flip_time = time_us_32() - flip_start_time;
    if (false) printf("Render: %.3f, flip: %.3f\n", render_time / 1000.f, flip_time / 1000.f);

    //printf("%02x %02x\n", display.get_gpio(), display.get_gpio_hi());

    display.set_display_offset(scroll);
    #if 1
    scroll.x += scroll_dir;
    if (scroll.x + DISPLAY_WIDTH > FRAME_WIDTH || scroll.x < 0) {
      scroll_dir = -scroll_dir;
      scroll.x += scroll_dir;
    }
    #else
    scroll.y += scroll_dir;
    if (scroll.y + DISPLAY_HEIGHT > FRAME_HEIGHT || scroll.y < 0) {
      scroll_dir = -scroll_dir;
      scroll.y += scroll_dir;
    }
    #endif

    ++frames;
    display.set_gpio_hi_pull_up_all(frames & 0x3F);
    display.set_gpio_hi_pull_down_all(~(frames & 0x3F));
    if (gpio_get(BUTTON_A) == 0) display.set_led_level((uint8_t)frames);
    else display.set_led_heartbeat();
  }
}
