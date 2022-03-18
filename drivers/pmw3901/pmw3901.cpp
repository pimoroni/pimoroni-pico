#include "pmw3901.hpp"

#include <cstdlib>
#include <math.h>
#include <cstdio>
#include <algorithm>
#include <cstring>

namespace pimoroni {

  enum reg : uint8_t {
    ID                  = 0x00,
    REVISION            = 0x01,
    DATA_READY          = 0x02,
    MOTION_BURST        = 0x16,
    POWER_UP_RESET      = 0x3a,
    ORIENTATION         = 0x5b,
    RESOLUTION          = 0x4e,  // PAA5100 only
    RAWDATA_GRAB        = 0x58,
    RAWDATA_GRAB_STATUS = 0x59,
  };

  bool PMW3901::init() {
    // configure spi interface and pins
    spi_init(spi, spi_baud);

    gpio_set_function(cs, GPIO_FUNC_SIO);
    gpio_set_dir(cs, GPIO_OUT);

    gpio_set_function(sck,  GPIO_FUNC_SPI);
    gpio_set_function(mosi, GPIO_FUNC_SPI);
    gpio_set_function(miso, GPIO_FUNC_SPI);

    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_pull_up(interrupt);
    }

    cs_select();
    sleep_ms(50);
    cs_deselect();

    write_register(reg::POWER_UP_RESET, 0x5a);
    sleep_ms(20);
    for(uint8_t offset = 0; offset < 5; offset++) {
      uint8_t data = 0;
      read_registers(reg::DATA_READY + offset, &data, 1);
    }

    secret_sauce();

    uint8_t product_id = get_id();
    uint8_t revision = get_revision();

    if(product_id != 0x49 || revision != 0x00) {
      return false;
    }

    return true;
  }

  spi_inst_t* PMW3901::get_spi() const {
    return spi;
  }

  int PMW3901::get_cs() const {
    return cs;
  }

  int PMW3901::get_sck() const {
    return sck;
  }

  int PMW3901::get_mosi() const {
    return mosi;
  }

  int PMW3901::get_miso() const {
    return miso;
  }

  int PMW3901::get_int() const {
    return interrupt;
  }

  uint8_t PMW3901::get_id() {
    uint8_t data = 0;
    read_registers(reg::ID, &data, 1);
    return data;
  }

  uint8_t PMW3901::get_revision() {
    uint8_t data = 0;
    read_registers(reg::REVISION, &data, 1);
    return data;
  }

  void PMW3901::set_rotation(Degrees degrees) {
    switch(degrees) {
      default:
      case DEGREES_0:
        set_orientation(true, true, true);
        break;
      
      case DEGREES_90:
        set_orientation(false, true, false);
        break;

      case DEGREES_180:
        set_orientation(false, false, true);
        break;

      case DEGREES_270:
        set_orientation(true, false, false);
        break;
    }
  }

  void PMW3901::set_orientation(bool invert_x, bool invert_y, bool swap_xy) {
    uint8_t value = 0;
    if(swap_xy)
      value |= 0b10000000;
    if(invert_y)
      value |= 0b01000000;
    if(invert_x)
      value |= 0b00100000;
    write_register(reg::ORIENTATION, value);
  }

  bool PMW3901::get_motion(int16_t& x_out, int16_t& y_out, uint16_t timeout_ms) {
    uint32_t start_time = millis();
    while(millis() - start_time < timeout_ms) {
      uint8_t buf[12];
      read_registers(reg::MOTION_BURST, buf, 12);
      uint8_t dr = buf[0];
      //uint8_t obs = buf[1];
      x_out = (int16_t)((int32_t)buf[3] << 8 | buf[2]);
      y_out = (int16_t)((int32_t)buf[5] << 8 | buf[4]);
      uint8_t quality = buf[6];
      //uint8_t raw_sum = buf[7];
      //uint8_t raw_max = buf[8];
      //uint8_t raw_min = buf[9];
      uint8_t shutter_upper = buf[10];
      //uint8_t shutter_lower = buf[11];
      if((dr & 0b10000000) && !((quality < 0x19) && (shutter_upper == 0x1f)))
        return true;

      sleep_ms(1);
    }

    return false;
  }

  bool PMW3901::get_motion_slow(int16_t& x_out, int16_t& y_out, uint16_t timeout_ms) {
    uint32_t start_time = millis();
    while(millis() - start_time < timeout_ms) {
      uint8_t buf[5];
      read_registers(reg::DATA_READY, buf, 5);
      uint8_t dr = buf[0];
      x_out = (int16_t)((int32_t)buf[2] << 8 | buf[1]);
      y_out = (int16_t)((int32_t)buf[4] << 8 | buf[3]);
      if(dr & 0b10000000)
        return true;

      sleep_ms(1);
    }

    return false;
  }

  bool PMW3901::frame_capture(uint8_t (&data_out)[FRAME_BYTES], uint16_t& data_size_out, uint16_t timeout_ms) {
    bool success = false;

    data_size_out = 0;

    uint8_t buf[] = {
      0x7f, 0x07,
      0x4c, 0x00,
      0x7f, 0x08,
      0x6a, 0x38,
      0x7f, 0x00,
      0x55, 0x04,
      0x40, 0x80,
      0x4d, 0x11,

      WAIT, 0x0a,

      0x7f, 0x00,
      0x58, 0xff
    };
    write_buffer(buf, sizeof(buf));

    uint8_t status = 0;
    uint32_t start_time = millis();
    while(millis() - start_time < timeout_ms) {
      status = read_register(reg::RAWDATA_GRAB_STATUS);
      if(status & 0b11000000) {
        success = true;
        break;
      }
    }

    if(success) {
      write_register(reg::RAWDATA_GRAB, 0x00);
      memset(data_out, 0, FRAME_BYTES * sizeof(uint8_t));
      
      uint8_t data = 0;
      uint16_t x = 0;

      success = false;
      start_time = millis();
      while(millis() - start_time < timeout_ms) {
        data = read_register(reg::RAWDATA_GRAB);
        if((data & 0b11000000) == 0b01000000) {     // Upper 6-bits
          data_out[x] &= ~0b11111100;
          data_out[x] |= (data & 0b00111111) << 2;  // Held in 5:0
        }
        if((data & 0b11000000) == 0b10000000) {     // Lower 2-bits
          data_out[x] &= ~0b00000011;
          data_out[x] |= (data & 0b00001100) >> 2;  // Held in 3:2
          x++;
        }
        if(x == FRAME_BYTES) {
          success = true;
          break;
        }
      }

      data_size_out = x;
    }

    return success;
  }

  void PMW3901::cs_select() {
    gpio_put(cs, false);  // Active low
  }

  void PMW3901::cs_deselect() {
    sleep_us(1);
    gpio_put(cs, true);
    sleep_us(1);
  }

  void PMW3901::write_register(uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg | 0x80;
    buf[1] = data;
    cs_select();
    spi_write_blocking(spi, buf, 2);
    cs_deselect();
  }

  void PMW3901::write_buffer(uint8_t *buf, uint16_t len) {
    for(uint8_t i = 0; i < len; i += 2) {
      if(buf[i] == WAIT)
        sleep_ms(buf[i + 1]);
      else
        write_register(buf[i], buf[i + 1]);
    }
  }

  void PMW3901::read_registers(uint8_t reg, uint8_t *buf, uint16_t len) {
    cs_select();
    spi_write_blocking(spi, &reg, 1);
    spi_read_blocking(spi, 0, buf, len);
    cs_deselect();
  }

  uint8_t PMW3901::read_register(uint8_t reg) {
    uint8_t data = 0;
    cs_select();
    spi_write_blocking(spi, &reg, 1);
    spi_read_blocking(spi, 0, &data, 1);
    cs_deselect();
    return data;
  }

  void PMW3901::secret_sauce() {
    uint8_t buf[] = {
      0x7f, 0x00,
      0x55, 0x01,
      0x50, 0x07,
  
      0x7f, 0x0e,
      0x43, 0x10
    };
    write_buffer(buf, sizeof(buf));

    if(read_register(0x67) & 0b10000000)
        write_register(0x48, 0x04);
    else
        write_register(0x48, 0x02);

    uint8_t buf2[] = {
      0x7f, 0x00,
      0x51, 0x7b,

      0x50, 0x00,
      0x55, 0x00,
      0x7f, 0x0E
    };
    write_buffer(buf2, sizeof(buf2));

    if(read_register(0x73) == 0x00) {
      uint8_t c1 = read_register(0x70);
      uint8_t c2 = read_register(0x71);
      if(c1 <= 28)
        c1 += 14;
      if(c1 > 28)
        c1 += 11;
      c1 = std::max((uint8_t)0, std::min((uint8_t)0x3F, c1));
      c2 = (c2 * 45) / 100;

      uint8_t buf3[] = {
        0x7f, 0x00,
        0x61, 0xad,
        0x51, 0x70,
        0x7f, 0x0e
      };
      write_buffer(buf3, sizeof(buf3));

      write_register(0x70, c1);
      write_register(0x71, c2);
    }

    uint8_t buf4[] = {
      0x7f, 0x00,
      0x61, 0xad,
      0x7f, 0x03,
      0x40, 0x00,
      0x7f, 0x05,

      0x41, 0xb3,
      0x43, 0xf1,
      0x45, 0x14,
      0x5b, 0x32,
      0x5f, 0x34,
      0x7b, 0x08,
      0x7f, 0x06,
      0x44, 0x1b,
      0x40, 0xbf,
      0x4e, 0x3f,
      0x7f, 0x08,
      0x65, 0x20,
      0x6a, 0x18,

      0x7f, 0x09,
      0x4f, 0xaf,
      0x5f, 0x40,
      0x48, 0x80,
      0x49, 0x80,

      0x57, 0x77,
      0x60, 0x78,
      0x61, 0x78,
      0x62, 0x08,
      0x63, 0x50,
      0x7f, 0x0a,
      0x45, 0x60,
      0x7f, 0x00,
      0x4d, 0x11,

      0x55, 0x80,
      0x74, 0x21,
      0x75, 0x1f,
      0x4a, 0x78,
      0x4b, 0x78,

      0x44, 0x08,
      0x45, 0x50,
      0x64, 0xff,
      0x65, 0x1f,
      0x7f, 0x14,
      0x65, 0x67,
      0x66, 0x08,
      0x63, 0x70,
      0x7f, 0x15,
      0x48, 0x48,
      0x7f, 0x07,
      0x41, 0x0d,
      0x43, 0x14,

      0x4b, 0x0e,
      0x45, 0x0f,
      0x44, 0x42,
      0x4c, 0x80,
      0x7f, 0x10,

      0x5b, 0x02,
      0x7f, 0x07,
      0x40, 0x41,
      0x70, 0x00,
      WAIT, 0x0A,  // Sleep for 10ms

      0x32, 0x44,
      0x7f, 0x07,
      0x40, 0x40,
      0x7f, 0x06,
      0x62, 0xf0,
      0x63, 0x00,
      0x7f, 0x0d,
      0x48, 0xc0,
      0x6f, 0xd5,
      0x7f, 0x00,

      0x5b, 0xa0,
      0x4e, 0xa8,
      0x5a, 0x50,
      0x40, 0x80,
      WAIT, 0xF0,

      0x7f, 0x14,  // Enable LED_N pulsing
      0x6f, 0x1c,
      0x7f, 0x00
    };
    write_buffer(buf4, sizeof(buf4));
  }

  uint32_t PMW3901::millis() {
    return to_ms_since_boot(get_absolute_time());
  }

  void PAA5100::secret_sauce() {
    uint8_t buf[] = {
      0x7f, 0x00,
      0x55, 0x01,
      0x50, 0x07,
  
      0x7f, 0x0e,
      0x43, 0x10
    };
    write_buffer(buf, sizeof(buf));

    if(read_register(0x67) & 0b10000000)
        write_register(0x48, 0x04);
    else
        write_register(0x48, 0x02);

    uint8_t buf2[] = {
      0x7f, 0x00,
      0x51, 0x7b,

      0x50, 0x00,
      0x55, 0x00,
      0x7f, 0x0E
    };
    write_buffer(buf2, sizeof(buf2));

    if(read_register(0x73) == 0x00) {
      uint8_t c1 = read_register(0x70);
      uint8_t c2 = read_register(0x71);
      if(c1 <= 28)
        c1 += 14;
      if(c1 > 28)
        c1 += 11;
      c1 = std::max((uint8_t)0, std::min((uint8_t)0x3F, c1));
      c2 = (c2 * 45) / 100;

      uint8_t buf3[] = {
        0x7f, 0x00,
        0x61, 0xad,
        0x51, 0x70,
        0x7f, 0x0e
      };
      write_buffer(buf3, sizeof(buf3));

      write_register(0x70, c1);
      write_register(0x71, c2);
    }

    uint8_t buf4[] = {
      0x7f, 0x00,
      0x61, 0xad,

      0x7f, 0x03,
      0x40, 0x00,

      0x7f, 0x05,
      0x41, 0xb3,
      0x43, 0xf1,
      0x45, 0x14,

      0x5f, 0x34,
      0x7b, 0x08,
      0x5e, 0x34,
      0x5b, 0x11,
      0x6d, 0x11,
      0x45, 0x17,
      0x70, 0xe5,
      0x71, 0xe5,

      0x7f, 0x06,
      0x44, 0x1b,
      0x40, 0xbf,
      0x4e, 0x3f,

      0x7f, 0x08,
      0x66, 0x44,
      0x65, 0x20,
      0x6a, 0x3a,
      0x61, 0x05,
      0x62, 0x05,

      0x7f, 0x09,
      0x4f, 0xaf,
      0x5f, 0x40,
      0x48, 0x80,
      0x49, 0x80,
      0x57, 0x77,
      0x60, 0x78,
      0x61, 0x78,
      0x62, 0x08,
      0x63, 0x50,

      0x7f, 0x0a,
      0x45, 0x60,

      0x7f, 0x00,
      0x4d, 0x11,
      0x55, 0x80,
      0x74, 0x21,
      0x75, 0x1f,
      0x4a, 0x78,
      0x4b, 0x78,
      0x44, 0x08,

      0x45, 0x50,
      0x64, 0xff,
      0x65, 0x1f,

      0x7f, 0x14,
      0x65, 0x67,
      0x66, 0x08,
      0x63, 0x70,
      0x6f, 0x1c,

      0x7f, 0x15,
      0x48, 0x48,

      0x7f, 0x07,
      0x41, 0x0d,
      0x43, 0x14,
      0x4b, 0x0e,
      0x45, 0x0f,
      0x44, 0x42,
      0x4c, 0x80,

      0x7f, 0x10,
      0x5b, 0x02,

      0x7f, 0x07,
      0x40, 0x41,

      WAIT, 0x0a,  // Sleep for 10ms

      0x7f, 0x00,
      0x32, 0x00,

      0x7f, 0x07,
      0x40, 0x40,

      0x7f, 0x06,
      0x68, 0xf0,
      0x69, 0x00,

      0x7f, 0x0d,
      0x48, 0xc0,
      0x6f, 0xd5,

      0x7f, 0x00,
      0x5b, 0xa0,
      0x4e, 0xa8,
      0x5a, 0x90,
      0x40, 0x80,
      0x73, 0x1f,

      WAIT, 0x0a,  // Sleep for 10ms

      0x73, 0x00
    };
    write_buffer(buf4, sizeof(buf4));
  }
}