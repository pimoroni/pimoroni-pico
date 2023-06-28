#include <cstdio>
#include <math.h>

#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "drivers/dv_display/dv_display.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

#include "JPEGDEC.h"
#include "drivers/fatfs/ff.h"

using namespace pimoroni;

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 720

FATFS fs;
FRESULT fr;

DVDisplay display;
PicoGraphics_PenDV_RGB888 graphics(FRAME_WIDTH, FRAME_HEIGHT, display);

JPEGDEC jpeg;
struct {
  int x, y;
} jpeg_decode_options;

void *jpegdec_open_callback(const char *filename, int32_t *size) {
  FIL *fil = new FIL;
  if(f_open(fil, filename, FA_READ)) {return nullptr;}
  *size = f_size(fil);
  return (void *)fil;
}

void jpegdec_close_callback(void *handle) {
  f_close((FIL *)handle);
  delete (FIL *)handle;
}

int32_t jpegdec_read_callback(JPEGFILE *jpeg, uint8_t *p, int32_t c) {
  uint br; f_read((FIL *)jpeg->fHandle, p, c, &br);
  return br;
}

int32_t jpegdec_seek_callback(JPEGFILE *jpeg, int32_t p) {
  return f_lseek((FIL *)jpeg->fHandle, p) == FR_OK ? 1 : 0;
}

int jpegdec_draw_callback(JPEGDRAW *draw) {
  uint8_t *p = (uint8_t*)draw->pPixels;

  int xo = jpeg_decode_options.x;
  int yo = jpeg_decode_options.y;

  for(int y = 0; y < draw->iHeight; y++) {
    for(int x = 0; x < draw->iWidth; x+=2) {
      int sx = ((draw->x + x) >> 1) + xo;
      int sy = draw->y + y + yo;

      if(sx >= 0 && sx < graphics.bounds.w && x < draw->iWidthUsed &&
         sy >= 0 && sy < graphics.bounds.h) {
        RGB888 col = ((((uint32_t)p[0] + (uint32_t)p[3]) << 15) & 0xff0000) | 
                     ((((uint32_t)p[1] + (uint32_t)p[4]) << 7) & 0xff00) | 
                      (((uint32_t)p[2] + (uint32_t)p[5]) >> 1);
        graphics.set_pen(col);
        graphics.pixel({sx, sy});
      }

      p += 6;
    }
  }

  return 1; // continue drawing
}

void draw_jpeg(std::string filename) {
  jpeg.open(
    filename.c_str(),
    jpegdec_open_callback,
    jpegdec_close_callback,
    jpegdec_read_callback,
    jpegdec_seek_callback,
    jpegdec_draw_callback
  );

  jpeg_decode_options.x = (FRAME_WIDTH - (jpeg.getWidth() >> 1)) >> 1;
  jpeg_decode_options.y = (FRAME_HEIGHT - jpeg.getHeight()) >> 1;

  jpeg.setPixelType(RGB888_LITTLE_ENDIAN);

  printf("- starting jpeg decode..");
  int start = millis();
  jpeg.decode(0, 0, 0);
  printf("done in %d ms!\n", int(millis() - start));

  jpeg.close();
}

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

  sleep_ms(5000);

  display.init(FRAME_WIDTH, FRAME_HEIGHT, DVDisplay::MODE_RGB888);
  //display.test();

  graphics.set_pen(0);
  graphics.clear();
  display.flip();
  graphics.set_pen(0);
  graphics.clear();

  printf("mounting sd card.. ");
  fr = f_mount(&fs, "", 1);
  if (fr != FR_OK) {
    printf("Failed to mount SD card, error: %d\n", fr);
    return 0;
  }
  printf("done!\n");

  printf("Listing sd card contents..\n");
  FILINFO file;
  auto dir = new DIR();
  bool first = true;
  constexpr int ms_per_image = 5000;
  const TCHAR* dirname = "/images720/";
  while (1) {
    f_opendir(dir, dirname);
    while(f_readdir(dir, &file) == FR_OK && file.fname[0]) {
        printf("- %s %lld\n", file.fname, file.fsize);

        draw_jpeg(std::string(dirname) + file.fname);

        if (!first) {
          for (int w = 0; w < ms_per_image && gpio_get(BUTTON_A) == 1; w += 10) {
            sleep_ms(10);
          }
        }
        first = false;
        display.flip();
        graphics.set_pen(0);
        graphics.clear();
    }
    f_closedir(dir);
    printf("done!\n");
  }
}
