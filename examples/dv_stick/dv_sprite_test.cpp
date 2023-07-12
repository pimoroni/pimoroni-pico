#include <cstdio>
#include <math.h>

#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "drivers/dv_display/dv_display.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

#define DISPLAY_WIDTH 720
#define DISPLAY_HEIGHT 480

#define FRAME_WIDTH 720
#define FRAME_HEIGHT 480

#define SPRITE_WIDTH 8
#define SPRITE_HEIGHT 8

#define USE_PALETTE 0

#if USE_PALETTE
static uint8_t sprite_data_pal[] = {
  0x00, 0x00, 0x00, 0x05, 0x05, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x05, 0x05, 0x05, 0x05, 0x00, 0x00, 
  0x00, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x00, 
  0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 
  0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 
  0x00, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x00, 
  0x00, 0x00, 0x05, 0x05, 0x05, 0x05, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x05, 0x05, 0x00, 0x00, 0x00, 
};
#else
static uint16_t sprite_data[] = {
  0x0000, 0x0000, 0x0000, 0xF000, 0xF000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0xF000, 0xF000, 0xF000, 0xF000, 0x0000, 0x0000, 
  0x0000, 0xF000, 0xF000, 0xF000, 0xF000, 0xF000, 0xF000, 0x0000, 
  0xF000, 0xF000, 0xF000, 0xF000, 0xF000, 0xF000, 0xF000, 0xF000, 
  0xF000, 0xF000, 0xF000, 0xF000, 0xF000, 0xF000, 0xF000, 0xF000, 
  0x0000, 0xF000, 0xF000, 0xF000, 0xF000, 0xF000, 0xF000, 0x0000, 
  0x0000, 0x0000, 0xF000, 0xF000, 0xF000, 0xF000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0xF000, 0xF000, 0x0000, 0x0000, 0x0000, 
};
#endif

void on_uart_rx() {
    while (uart_is_readable(uart1)) {
        uint8_t ch = uart_getc(uart1);
        putc(ch, stdout);
    }
}

int main() {
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
#if USE_PALETTE
  display.init(DISPLAY_WIDTH, DISPLAY_HEIGHT, DVDisplay::MODE_PALETTE, FRAME_WIDTH, FRAME_HEIGHT);
  PicoGraphics_PenDV_P5 graphics(FRAME_WIDTH, FRAME_HEIGHT, display);

  display.define_palette_sprite(0, SPRITE_WIDTH, SPRITE_HEIGHT, sprite_data_pal);
  graphics.create_pen(0, 0, 0x20);
  graphics.create_pen(0xF0, 0, 0);
  graphics.set_pen(0);
#else
  display.init(DISPLAY_WIDTH, DISPLAY_HEIGHT, DVDisplay::MODE_RGB555, FRAME_WIDTH, FRAME_HEIGHT);
  PicoGraphics_PenDV_RGB555 graphics(FRAME_WIDTH, FRAME_HEIGHT, display);

  display.define_sprite(0, SPRITE_WIDTH, SPRITE_HEIGHT, sprite_data);
  graphics.set_pen(0x000F);
#endif
  graphics.clear();
  display.flip();

  constexpr int NUM_SPRITES = 32;
  Point sprite_pos[NUM_SPRITES];
  Point sprite_dir[NUM_SPRITES];

  for (int i = 0; i < 32; ++i) {
    sprite_pos[i].x = rand() % DISPLAY_WIDTH;
    sprite_pos[i].y = rand() % DISPLAY_HEIGHT;
    sprite_dir[i].x = (rand() & 1) ? 1 : -1;
    sprite_dir[i].y = (rand() & 1) ? 1 : -1;
  }

  while (true)
  {
    for (int i = 0; i < 32; ++i) {
      display.set_sprite(i, 0, sprite_pos[i], (i < 16) ? DVDisplay::BLEND_BLEND : DVDisplay::BLEND_DEPTH);

      sprite_pos[i] += sprite_dir[i];
      if (sprite_pos[i].x >= DISPLAY_WIDTH || sprite_pos[i].x <= 0) {
        sprite_dir[i].x = -sprite_dir[i].x;
        sprite_pos[i].x += sprite_dir[i].x;
      }
      if (sprite_pos[i].y >= DISPLAY_HEIGHT || sprite_pos[i].y <= 0) {
        sprite_dir[i].y = -sprite_dir[i].y;
        sprite_pos[i].y += sprite_dir[i].y;
      }
    }

    sleep_ms(16);
  }
}
