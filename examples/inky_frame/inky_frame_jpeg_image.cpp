#include <cstdio>
#include <math.h>

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/vreg.h"

#include "JPEGDEC.h"

#ifdef INKY_FRAME_7
#include "libraries/inky_frame_7/inky_frame_7.hpp"
#else
#include "libraries/inky_frame/inky_frame.hpp"
#endif

using namespace pimoroni;

FATFS fs;
FRESULT fr;

InkyFrame inky;


JPEGDEC jpeg;
struct {
  int x, y, w, h;
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
  uint16_t *p = draw->pPixels;

  int xo = jpeg_decode_options.x;
  int yo = jpeg_decode_options.y;

  for(int y = 0; y < draw->iHeight; y++) {
    for(int x = 0; x < draw->iWidth; x++) {
      int sx = ((draw->x + x + xo) * jpeg_decode_options.w) / jpeg.getWidth();
      int sy = ((draw->y + y + yo) * jpeg_decode_options.h) / jpeg.getHeight();

      if(xo + sx > 0 && xo + sx < inky.bounds.w && yo + sy > 0 && yo + sy < inky.bounds.h) {
        inky.set_pixel_dither({xo + sx, yo + sy}, RGB(RGB565(*p)));
      }

      p++;
    }
  }

  return 1; // continue drawing
}

void draw_jpeg(std::string filename, int x, int y, int w, int h) {

  // TODO: this is a horrible way to do it but we need to pass some parameters
  // into the jpegdec_draw_callback() method somehow and the library isn't
  // setup to allow any sort of user data to be passed around - yuck
  jpeg_decode_options.x = x;
  jpeg_decode_options.y = y;
  jpeg_decode_options.w = w;
  jpeg_decode_options.h = h;

  jpeg.open(
    filename.c_str(),
    jpegdec_open_callback,
    jpegdec_close_callback,
    jpegdec_read_callback,
    jpegdec_seek_callback,
    jpegdec_draw_callback);

  jpeg.setPixelType(RGB565_BIG_ENDIAN);

  printf("- starting jpeg decode..");
  int start = millis();
  jpeg.decode(0, 0, 0);
  printf("done in %d ms!\n", int(millis() - start));

  jpeg.close();
}

int main() {
  inky.init();

  inky.led(InkyFrame::LED_ACTIVITY, 0);

  inky.led(InkyFrame::LED_CONNECTION, 0);

  std::string filename;

  if(inky.get_wake_up_event() == InkyFrame::BUTTON_A_EVENT) {
    filename = "a.jpg";
    inky.led(InkyFrame::LED_A, 255);
  }
  if(inky.get_wake_up_event() == InkyFrame::BUTTON_B_EVENT) {
    filename = "b.jpg";
    inky.led(InkyFrame::LED_B, 255);
  }
  if(inky.get_wake_up_event() == InkyFrame::BUTTON_C_EVENT) {
    filename = "c.jpg";
    inky.led(InkyFrame::LED_C, 255);
  }
  if(inky.get_wake_up_event() == InkyFrame::BUTTON_D_EVENT) {
    filename = "d.jpg";
    inky.led(InkyFrame::LED_D, 255);
  }
  if(inky.get_wake_up_event() == InkyFrame::BUTTON_E_EVENT) {
    filename = "e.jpg";
    inky.led(InkyFrame::LED_E, 255);
  }
  if(inky.get_wake_up_event() == InkyFrame::EXTERNAL_TRIGGER_EVENT) {
    uint8_t i = 128;
    while(true){
      inky.led(InkyFrame::LED_ACTIVITY, i);
      i++;
    }; // Wait for debugger
  }
  
  filename = "butterfly-600x448.jpg";

  //inky.led(InkyFrame::LED_E, 255);
  //sleep_ms(1000);

  if(filename == "") {
    inky.sleep();
  }

  inky.led(InkyFrame::LED_ACTIVITY, 255);
  // turn on led to show we're awake
  

  // init usb serial for debugging and give our terminal monitor a bit of
  // time to connect
  stdio_init_all();
  printf("Woke with file: %s\n", filename.c_str());

  printf("initialising inky frame.. ");

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
  f_opendir(dir, "/");
  while(f_readdir(dir, &file) == FR_OK && file.fname[0]) {
      printf("- %s %lld\n", file.fname, file.fsize);
  }
  f_closedir(dir);
  printf("done!\n");

  printf("Displaying file: %s\n", filename.c_str());
  draw_jpeg(filename, 0, 0, inky.width, inky.height);
  printf("done!\n");

  inky.update();

  sleep_ms(5000);
  inky.led(InkyFrame::LED_CONNECTION, 0);
  inky.sleep();

  return 0;
}
