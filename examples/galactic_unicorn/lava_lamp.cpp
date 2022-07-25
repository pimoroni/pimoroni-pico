#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "galactic_unicorn.hpp"
#include "okcolor.hpp"

using namespace pimoroni;

PicoGraphics_PenRGB565 graphics(53, 11, nullptr);
GalacticUnicorn galactic_unicorn;

// HSV Conversion expects float inputs in the range of 0.00-1.00 for each channel
// Outputs are rgb in the range 0-255 for each channel
void from_hsv(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
  float i = floor(h * 6.0f);
  float f = h * 6.0f - i;
  v *= 255.0f;
  uint8_t p = v * (1.0f - s);
  uint8_t q = v * (1.0f - f * s);
  uint8_t t = v * (1.0f - (1.0f - f) * s);

  switch (int(i) % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  }
}

struct blob_t {
  float x, y;
  float r;
  float dx, dy;
};

constexpr int blob_count = 20;

int main() {

  stdio_init_all();

  galactic_unicorn.init();

  // randomise blob start positions, directions, and size
  std::array<blob_t, blob_count> blobs;
  for(auto &blob : blobs) {
    blob.x = rand() % 11;
    blob.y = rand() % 53;
    blob.r = ((rand() % 40) / 10.0f) + 5.0f;
    blob.dx = ((rand() % 2) / 10.0f) - 0.05f;
    blob.dy = ((rand() % 3) / 10.0f) - 0.1f;
  }

  float hue = 0.0f;

  while(true) {
    // allow user to adjust brightness
    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_BRIGHTNESS_UP)) {
      galactic_unicorn.adjust_brightness(+0.01);
    }
    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_BRIGHTNESS_DOWN)) {
      galactic_unicorn.adjust_brightness(-0.01);
    }
    
    uint start_ms = to_ms_since_boot(get_absolute_time());

    // calculate the influence of each blob on the liquid based
    // on their distance to each pixel. this causes blobs to 
    // "merge" into each other when we use fixed thresholds to
    // determine which colour to draw with
    float liquid[11][53] = {0.0f};
    for(auto &blob : blobs) {
      float r_sq = blob.r * blob.r;
      for(int y = 0; y < 53; y++) {
        for(int x = 0; x < 11; x++) {
          float d_sq = (x - blob.x) * (x - blob.x) + (y - blob.y) * (y - blob.y);
          if(d_sq <= r_sq) {
            // add this blobs influence to this pixel
            liquid[x][y] += 1.0f - (d_sq / r_sq);
          }
        }
      }
    }

    // update the blob positions
    for(auto &blob : blobs) {
      blob.x += blob.dx;
      blob.y += blob.dy;

      // if we hit the edge then bounce!
      if(blob.x < 0.0f || blob.x >= 11.0f) {
        blob.dx *= -1.0f;
      }    
      if(blob.y < 0.0f || blob.y >= 53.0f) {
        blob.dy *= -1.0f;
      }    
    }

    // rotate the hue
    hue += 0.001f;

    // calculate dark, medium, and bright shades for rendering the 
    // lava
    uint8_t dark_r, dark_g, dark_b;
    from_hsv(hue, 1.0f, 0.3f, dark_r, dark_g, dark_b);
    uint8_t mid_r, mid_g, mid_b;
    from_hsv(hue, 1.0f, 0.6f, mid_r, mid_g, mid_b);
    uint8_t bright_r, bright_g, bright_b;
    from_hsv(hue, 1.0f, 1.0f, bright_r, bright_g, bright_b);

    // clear the canvas
    graphics.set_pen(0, 0, 0);
    graphics.clear();

    // render the lava
    for(int y = 0; y < 53; y++) {
      for(int x = 0; x < 11; x++) {
        float v = liquid[x][y];

        // select a colour for this pixel based on how much
        // "blobfluence" there is at this position in the liquid
        if(v >= 1.5f) {
          graphics.set_pen(bright_r, bright_g, bright_b);
          graphics.pixel(Point(y, x));
        }else if(v >= 1.25f) {
          graphics.set_pen(mid_r, mid_g, mid_b);
          graphics.pixel(Point(y, x));
        }else if(v >= 1.0f) {
          graphics.set_pen(dark_r, dark_g, dark_b);
          graphics.pixel(Point(y, x));
        }
      }
    }

    uint end_ms = to_ms_since_boot(get_absolute_time());

    printf("rendering took %dms\n", end_ms - start_ms);

    galactic_unicorn.update(graphics);
  }

  return 0;
}
