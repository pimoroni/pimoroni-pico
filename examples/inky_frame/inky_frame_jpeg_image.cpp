#include <cstdio>
#include <math.h>

#include <stdio.h>
#include "pico/stdlib.h"

#include "JPEGDEC.h"
#include "inky_frame.hpp"

using namespace pimoroni;

FATFS fs;
FRESULT fr;


InkyFrame inky;

void led_on() {
}

void led_off() {

}

uint32_t time() {
  absolute_time_t t = get_absolute_time();
  return to_ms_since_boot(t);
}


struct RGB {
  int r, g, b;

  RGB() : r(0), g(0), b(0) {}
  RGB(int r, int g, int b) : r(r), g(g), b(b) {}

  RGB  operator+(const RGB& c) const {return RGB(r + c.r, g + c.g, b + c.b);}
  RGB& operator+=(const RGB& c) {r += c.r; g += c.g; b += c.b; return *this;}
  RGB  operator-(const RGB& c) const {return RGB(r - c.r, g - c.g, b - c.b);}
};




// taken by photographing the screen in day light with solid colour bars
// then applying a strong gaussian blue to the image and eye dropping each
// colour from it.

/*RGB palette[8] = {
  { 49,  40,  68}, // black
  {185, 179, 183}, // white
  { 95, 127, 104}, // green
  {110,  95, 138}, // blue
  {192,  64,  85}, // red
  {197, 163,  53}, // yellow
  {200, 128,  41}, // orange
  {175, 140, 134}  // clean / taupe?!
};
*/

RGB palette[8] = {
  {  0,   0,   0}, // black
  {255, 255, 255}, // white
  {  0, 255,   0}, // green
  {  0,   0, 255}, // blue
  {255,   0,   0}, // red
  {255, 255,   0}, // yellow
  {255, 128,   0}, // orange
  {220, 180, 200}  // clean / taupe?!
};

// a relatively low cost approximation of how "different" two colours are
// perceived which avoids expensive colour space conversions.
// described in detail at https://www.compuphase.com/cmetric.htm
int distance(const RGB &col1, const RGB &col2) {
  int rmean = (col1.r + col2.r) / 2;
  int r = col1.r - col2.r, g = col1.g - col2.g, b = col1.b - col2.b;
  return abs((int)(
    (((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8)
  ));
}

// a rough approximation of how bright a colour is used to compare the
// relative brightness of two colours
int luminance(const RGB &col) {
  // weights based on https://www.johndcook.com/blog/2009/08/24/algorithms-convert-color-grayscale/
  return col.r * 21 + col.g * 72 + col.b * 7;
}

// returns the index of the closest match colour in the palette
int closest(const RGB &col) {
  int d = INT_MAX, m = -1;
  for(size_t i = 0; i < std::size(palette); i++) {
    int dc = distance(col, palette[i]);
    if(dc < d) {m = i; d = dc;}
  }
  return m;
}

// finds a suitable pallet index for the specified pixel based on the
// supplied target colour by applying an ordered dither (4x4)
int find_colour_dithered(RGB colour, int x, int y) {
  static uint pattern[16] = // dither pattern
    {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};

  // find a list of candidates by starting with the ideal colours closest
  // match and then compounding the error before selecting the next candidate
  // this is repeated until we have enough candidates for all elements in our
  // dither pattern
  static std::array<int, 16> candidates;
  RGB error;
  for(size_t i = 0; i < std::size(candidates); i++) {
    candidates[i] = closest(colour + error);
    error += (colour - palette[candidates[i]]);
  }

  // sort by a rough approximation of luminence
  std::sort(candidates.begin(), candidates.end(), [](int a, int b) {
    return luminance(palette[a]) > luminance(palette[b]);
  });

  // find the pattern coordinate offset
  uint pattern_index = (x & 0b11) | ((y & 0b11) << 2);

  // return the selected candidate
  return candidates[pattern[pattern_index]];
}


void * myOpen(const char *filename, int32_t *size) {
  FIL *pfil = new FIL;
  FRESULT fr = f_open(pfil, filename, FA_READ);

  printf("open %s..", filename);
  if(fr) {
    return nullptr;
  }

  printf("..done!\n");

  *size = f_size(pfil);
  return (void *)pfil;
}


void myClose(void *handle) {
  FIL *pf = (FIL *)handle;
  f_close(pf);
  delete pf;
}

int32_t myRead(JPEGFILE *handle, uint8_t *buffer, int32_t length) {
  FIL *pf = (FIL *)handle->fHandle;
  uint bytes_read;
  f_read(pf, buffer, length, &bytes_read);
  return bytes_read;
}

int32_t mySeek(JPEGFILE *handle, int32_t position) {
  FIL *pf = (FIL *)handle->fHandle;
  FRESULT fr = f_lseek (pf, position);
  return fr == FR_OK ? 1 : 0;
}

int JPEGDraw(JPEGDRAW *pDraw) {
  uint16_t *p = pDraw->pPixels;

  int xo = pDraw->x;
  int yo = pDraw->y;

  for(int y = 0; y < pDraw->iHeight; y++) {
    for(int x = 0; x < pDraw->iWidth; x++) {
      uint16_t rgb = *p;

      RGB c;
      c.r = ((rgb >> 11) &  0b11111) << 3;
      c.g = ((rgb >>  5) & 0b111111) << 2;
      c.b = ((rgb >>  0) &  0b11111) << 3;

      if(x + xo > 0 && x + xo < 600 && y + yo > 0 && y + yo < 448) {
        inky.fast_pixel(x + xo, y + yo, find_colour_dithered(c, x + xo, y + yo));
        //inky.fast_pixel(x + xo, y + yo, closest(c));
      }

      p++;
    }
  }

  return 1; // continue drawing
}



int main() {
  // turn on led to show we're awake
  gpio_set_function(6, GPIO_FUNC_SIO);
  gpio_set_dir(6, GPIO_OUT);
  gpio_put(6, true);

  // keep the pico awake by holding vsys_en high
  gpio_set_function(2, GPIO_FUNC_SIO);
  gpio_set_dir(2, GPIO_OUT);
  gpio_put(2, true);

  // init usb serial for debugging and give our terminal monitor a bit of
  // time to connect
  stdio_init_all();
  sleep_ms(500);

  printf("\n");
  printf("\n");
  printf("\n");

  printf("initialising inky frame.. ");
  inky.init();
  printf("done!\n");

  printf("turning on leds.. ");
  //inky.led(InkyFrame::LED_A, 128);
  //inky.led(InkyFrame::LED_B, 128);
  //inky.led(InkyFrame::LED_CONN, 255);
  printf("done!\n");

  printf("mounting sd card.. ");
  fr = f_mount(&fs, "", 1);
  if (fr != FR_OK) {
    printf("Failed to mount SD card, error: %d\n", fr);
    return 0;
  }
  printf("done!\n");

  printf("listing sd card contents..\n");

  FILINFO file;
  auto dir = new DIR();
  f_opendir(dir, "/");
  while(f_readdir(dir, &file) == FR_OK && file.fname[0]) {
      printf("- %s %lld\n", file.fname, file.fsize);
  }
  f_closedir(dir);
  printf("done!\n");

  JPEGDEC jpeg;
/*
  printf("updating screen.. ");
  jpeg.open("test.jpg", myOpen, myClose, myRead, mySeek, JPEGDraw);
  jpeg.decode(0, 0, 0);
  jpeg.close();
  inky.update();
  printf("done!\n");*/
/*

  for(int i = 0; i < 8; i++) {
    inky.pen(i);
    inky.rectangle(i * 75, 0, 75, 448);
  }
  inky.update();

  while(true) {}
*/

  while(true) {
    bool pressed = false;
    std::string filename = "";
    while(!pressed) {
      if(inky.pressed(InkyFrame::BUTTON_A)) {
        filename = "a.jpg"; pressed = true;
        inky.led(InkyFrame::LED_A, 255);
      }
      if(inky.pressed(InkyFrame::BUTTON_B)) {
        filename = "b.jpg"; pressed = true;
        inky.led(InkyFrame::LED_B, 255);
      }
      if(inky.pressed(InkyFrame::BUTTON_C)) {
        filename = "c.jpg"; pressed = true;
        inky.led(InkyFrame::LED_C, 255);
      }
      if(inky.pressed(InkyFrame::BUTTON_D)) {
        filename = "d.jpg"; pressed = true;
        inky.led(InkyFrame::LED_D, 255);
      }
      if(inky.pressed(InkyFrame::BUTTON_E)) {
        filename = "e.jpg"; pressed = true;
        inky.led(InkyFrame::LED_E, 255);
      }

      tight_loop_contents();
    }


    jpeg.open(filename.c_str(), myOpen, myClose, myRead, mySeek, JPEGDraw);

    printf("- starting jpeg decode..");
    int start = time();
    jpeg.decode(0, 0, 0);
    printf("done in %d ms!\n", int(time() - start));

    jpeg.close();
    inky.update();
    inky.led(InkyFrame::LED_A, 0);
    inky.led(InkyFrame::LED_B, 0);
    inky.led(InkyFrame::LED_C, 0);
    inky.led(InkyFrame::LED_D, 0);
    inky.led(InkyFrame::LED_E, 0);




  }

/*
  for(int i = 0; i <  8; i++) {

    inky.pen(i);
    inky.rectangle(i * 40, 0, 40, 200);
  }*/

  // int open(
  //   const char *szFilename,
  //   JPEG_OPEN_CALLBACK *pfnOpen,
  //   JPEG_CLOSE_CALLBACK *pfnClose,
  //   JPEG_READ_CALLBACK *pfnRead,
  //   JPEG_SEEK_CALLBACK *pfnSeek,
  //   JPEG_DRAW_CALLBACK *pfnDraw
  // );

  //while(true) {
//    bool state = inky.pressed(InkyFrame::BUTTON_C);
  //  printf("%s\n", state ? "pressed": "released");
    //tight_loop_contents();
  //}
  /*
  printf("writing to screen.. ");
  inky.thickness(5);
  inky.text("this is a test", 10, 100, 2.0f);
  printf("done!\n");

  for(int x = 0; x < 50; x++) {
    for(int y = 0; y < 50; y++) {
      inky.fast_pixel(x, y, (x + y) & 0b111);
    }

  }*/


/*


  printf("reading file from sd card.. ");
  uint start = time();

  uint total_bytes_read = 0;
  fr = f_open(&fil, "dishat.jpg", FA_READ);
  if(fr == FR_OK) {
    uint8_t readbuf[1024 * 16];
    while(!f_eof(&fil)) {
      uint bytes_read;
      f_read(&fil, readbuf, 1024 * 16, &bytes_read);
      total_bytes_read += bytes_read;
      printf(".");
    }

    f_close(&fil);
  }
  uint end = time();

  printf("done!\n");
  printf("\n");
  printf("%d bytes read in %d\n", total_bytes_read, end - start);
  printf("\n");
*/

  while(true) {
    tight_loop_contents();
  }



/*
  printf("\n\n\n");
  printf("PCF85063A rtc demo!\n");
  printf("===================\n");
  sleep_ms(500);

  printf("initialise rtc.. ");
  I2C i2c(4, 5);
  PCF85063A rtc(&i2c);
  rtc.init();
  printf("done!\n");
  printf("\n");

  printf("check clock function\n");
  printf("\n");

  printf("- setting date and time\n");
  printf("  > 30/5/2022 08:07:12\n");
  datetime_t now = {
    .year = 2022, .month = 5, .day = 30, .hour = 8, .min = 7, .sec = 12
  };
  rtc.set_datetime(&now);

  printf("\n");
  printf("- sleeping for 3 seconds\n");
  sleep_ms(3000);

  printf("\n");
  printf("- reading date and time\n");
  now = rtc.get_datetime();
  printf("  > %02d/%02d/%04d %02d:%02d:%02d\n",
    now.day, now.month, now.year, now.hour, now.min, now.sec);
  printf("\n");

  printf("check alarm function\n");
  printf("\n");

  printf("- setting alarm for in 6 seconds\n");
  printf("  > 08:07:21\n");
  rtc.set_alarm(21, 7, 8);

  printf("\n");
  printf("- waiting for alarm flag\n");

  while(!rtc.read_alarm_flag()) {
    tight_loop_contents();
  }

  printf("\n");
  printf("- alarm triggered at\n");
  now = rtc.get_datetime();
  printf("  > %02d/%02d/%04d %02d:%02d:%02d\n",
    now.day, now.month, now.year, now.hour, now.min, now.sec);
  printf("\n");

  printf("- clear alarm flag\n");
  rtc.clear_alarm_flag();


  printf("check timer function\n");
  printf("\n");

  printf("- setting timer for in 5 seconds\n");
  rtc.set_timer(5);

  printf("\n");
  printf("- waiting for timer flag\n");

  while(!rtc.read_timer_flag()) {
    tight_loop_contents();
  }

  printf("\n");
  printf("- timer triggered at\n");
  now = rtc.get_datetime();
  printf("  > %02d/%02d/%04d %02d:%02d:%02d\n",
    now.day, now.month, now.year, now.hour, now.min, now.sec);
  printf("\n");

  printf("- clear timer flag\n");
  rtc.clear_alarm_flag();

  printf("\n");
  printf("tests complete\n");
*/
  return 0;
}
