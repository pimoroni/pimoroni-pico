#include <cstdio>
#include <math.h>

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/vreg.h"

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



// helpful type for storing and manipulating colours
struct RGB {
  int r, g, b;

  RGB() : r(0), g(0), b(0) {}
  RGB(int r, int g, int b) : r(r), g(g), b(b) {}

  RGB  operator+ (const RGB& c) const {return RGB(r + c.r, g + c.g, b + c.b);}
  RGB& operator+=(const RGB& c) {r += c.r; g += c.g; b += c.b; return *this;}
  RGB  operator- (const RGB& c) const {return RGB(r - c.r, g - c.g, b - c.b);}
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
};*/

// it turns out that visually things look a lot better if you fully
// saturate the palette, this makes sense in hindsight as otherwise the
// palette entries are all quite muted and relatively close to each other
// perceptively

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

// find a list of candidates by starting with the ideal colours closest
// match and then compounding the error before selecting the next candidate
// this is repeated until we have enough candidates for all elements in our
// dither pattern
void get_candidates(const RGB &col, std::array<uint8_t, 16> &candidates) {
  RGB error;
  for(size_t i = 0; i < std::size(candidates); i++) {
    candidates[i] = closest(col + error);
    error += (col - palette[candidates[i]]);
  }

  // sort by a rough approximation of luminance, this ensures that neighbouring
  // pixels in the dither matrix are at extreme opposites of luminence
  // giving a more balanced output
  std::sort(candidates.begin(), candidates.end(), [](int a, int b) {
    return luminance(palette[a]) > luminance(palette[b]);
  });
}

// runs around 10x faster but at some cost to image fidelity. the rgb colour
// is reduced down to 9-bits (rrrgggbbb) and converted into an integer that
// becomes a key into a cache containing the already found candidates for
// that colour
#define CACHE_CANDIDATES 1

// enables ordered dithering when rendering jpeg images
#define ENABLE_JPEG_DITHER 1

// finds a suitable pallet index for the specified pixel based on the
// supplied target colour by applying an ordered dither (4x4)
int find_colour_dithered(const RGB &col, int x, int y) {
  static uint pattern[16] = // dither pattern
    {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};

  #if CACHE_CANDIDATES == 1

    static std::array<std::array<uint8_t, 16>, 512> candidate_cache;
    static bool cache_built = false;

    // if we haven't already built the cache then do it now
    if(!cache_built) {
      for(uint i = 0; i < 512; i++) {
        RGB cache_col((i & 0x1C0) >> 1, (i & 0x38) << 2, (i & 0x7) << 5);
        get_candidates(cache_col, candidate_cache[i]);
      }
      cache_built = true;
    }

    uint cache_key =
      ((col.r & 0xE0) << 1) | ((col.g & 0xE0) >> 2) | ((col.b & 0xE0) >> 5);

    // find the pattern coordinate offset
    uint pattern_index = (x & 0b11) | ((y & 0b11) << 2);

    // return the selected candidate
    return candidate_cache[cache_key][pattern[pattern_index]];

  #else

    static std::array<uint8_t, 16> candidates;
    get_candidates(col, candidates);

    // find the pattern coordinate offset
    uint pattern_index = (x & 0b11) | ((y & 0b11) << 2);

    // return the selected candidate
    return candidates[pattern[pattern_index]];

  #endif
}

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

      if(xo + sx > 0 && xo + sx < 600 && yo + sy > 0 && yo + sy < 448) {
        RGB c(
          ((*p >> 11) &  0b11111) << 3,
          ((*p >>  5) & 0b111111) << 2,
          ((*p >>  0) &  0b11111) << 3
        );

        #if ENABLE_JPEG_DITHER == 1

          inky.fast_pixel(xo + sx, yo + sy, find_colour_dithered(c, xo + sx, yo + sy));

        #else

          inky.fast_pixel(xo + sx, yo + sy, closest(c));

        #endif
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

  printf("- starting jpeg decode..");
  int start = time();
  jpeg.decode(0, 0, 0);
  printf("done in %d ms!\n", int(time() - start));

  jpeg.close();
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

  vreg_set_voltage(VREG_VOLTAGE_1_20);
  sleep_ms(10);
  set_sys_clock_khz(250000, true);

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

  while(true) {
    bool pressed = false;
    std::string filename = "";
    int offset = 0;
    while(!pressed) {
      if(inky.pressed(InkyFrame::BUTTON_A)) {
        filename = "a.jpg"; pressed = true;
        inky.led(InkyFrame::LED_A, 255);
        offset = 20;
      }
      if(inky.pressed(InkyFrame::BUTTON_B)) {
        filename = "b.jpg"; pressed = true;
        inky.led(InkyFrame::LED_B, 255);
        offset = 40;
      }
      if(inky.pressed(InkyFrame::BUTTON_C)) {
        filename = "c.jpg"; pressed = true;
        inky.led(InkyFrame::LED_C, 255);
        offset = 60;
      }
      if(inky.pressed(InkyFrame::BUTTON_D)) {
        filename = "d.jpg"; pressed = true;
        inky.led(InkyFrame::LED_D, 255);
        offset = 80;
      }
      if(inky.pressed(InkyFrame::BUTTON_E)) {
        filename = "e.jpg"; pressed = true;
        inky.led(InkyFrame::LED_E, 255);
        offset = 100;
      }

      tight_loop_contents();
    }

    draw_jpeg(filename, offset, offset, 400, 300);

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
