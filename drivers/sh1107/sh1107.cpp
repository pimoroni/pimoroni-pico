#include "sh1107.hpp"

#include <cstdlib>
#include <math.h>
#include <string.h>

namespace pimoroni {
  
  void SH1107::update(PicoGraphics *graphics) {
    if(graphics->pen_type == PicoGraphics::PEN_1BIT) { // Display buffer is screen native

      uint8_t *p = (uint8_t *)graphics->frame_buffer;
      uint framebuffer_size = PicoGraphics_Pen1Bit::buffer_size(width, height);
      uint page_size = framebuffer_size / 16;

      uint8_t temp[framebuffer_size] = {0};
      uint8_t *ptemp = temp;

      for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
          uint bo = 7 - (x & 0b111);
          uint8_t color = p[(x / 8) + (y * width / 8)] & (1U << bo);
          if(color) {
            temp[x + (y / 8 ) * width] |= 1 << (y % 8);
          }else{
            temp[x + (y / 8 ) * width] &= ~(1 << (y % 8));
          }
        }
      }

      uint8_t buf[page_size + 1];

      for(int i = 0; i < 16; i++) {
        i2c.reg_write_uint8(i2c_address, 0, 0xb0 + i);
        i2c.reg_write_uint8(i2c_address, 0, 0x00);
        i2c.reg_write_uint8(i2c_address, 0, 0x10);

        memcpy(buf + 1, ptemp, page_size);
        buf[0] = 0x40;

        i2c.write_blocking(i2c_address, buf, page_size + 1, false);

        ptemp += page_size;
      }
      
    }
  }

}
