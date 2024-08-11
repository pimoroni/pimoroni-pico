#include "psram_display.hpp"

#include <cstdlib>
#include <math.h>
#include <string.h>

namespace pimoroni {

  enum reg {
    WRITE = 0x02,
    READ = 0x03,
    RESET_ENABLE = 0x66,
    RESET = 0x99
  };

  void PSRamDisplay::init() {
    uint baud = spi_init(spi, 31'250'000);
    printf("PSRam connected at %u\n", baud);
    gpio_set_function(CS, GPIO_FUNC_SIO);
    gpio_set_dir(CS, GPIO_OUT);
    gpio_put(CS, 1);

    gpio_set_function(SCK,  GPIO_FUNC_SPI);
    gpio_set_function(MOSI, GPIO_FUNC_SPI);
    gpio_set_function(MISO, GPIO_FUNC_SPI);

    gpio_put(CS, 0);
    uint8_t command_buffer[2] = {RESET_ENABLE, RESET};
    spi_write_blocking(spi, command_buffer, 2);
    gpio_put(CS, 1);
  }
  
  void PSRamDisplay::write(uint32_t address, size_t len, const uint8_t *data)
  {
    gpio_put(CS, 0);
    uint8_t command_buffer[4] = {WRITE, (uint8_t)((address >> 16) & 0xFF), (uint8_t)((address >> 8) & 0xFF), (uint8_t)(address & 0xFF)};
    spi_write_blocking(spi, command_buffer, 4);
    spi_write_blocking(spi, data, len);
    gpio_put(CS, 1);
  }

  void PSRamDisplay::write(uint32_t address, size_t len, const uint8_t byte)
  {
    gpio_put(CS, 0);
    uint8_t command_buffer[4] = {WRITE, (uint8_t)((address >> 16) & 0xFF), (uint8_t)((address >> 8) & 0xFF), (uint8_t)(address & 0xFF)};
    spi_write_blocking(spi, command_buffer, 4);
    SpiSetBlocking(byte, len);
    gpio_put(CS, 1);
  }


  void PSRamDisplay::read(uint32_t address, size_t len, uint8_t *data)
  {
    gpio_put(CS, 0);
    uint8_t command_buffer[4] = {READ, (uint8_t)((address >> 16) & 0xFF), (uint8_t)((address >> 8) & 0xFF), (uint8_t)(address & 0xFF)};
    spi_write_blocking(spi, command_buffer, 4);
    spi_read_blocking(spi, 0, data, len);
    gpio_put(CS, 1);
  }

  void PSRamDisplay::write_pixel(const Point &p, uint8_t colour)
  {
    write(pointToAddress(p), 1, colour);
  }

  void PSRamDisplay::write_pixel_span(const Point &p, uint l, uint8_t colour)
  {
    write(pointToAddress(p), l, colour);
  }

  void PSRamDisplay::read_pixel_span(const Point &p, uint l, uint8_t *data)
  {
    read(pointToAddress(p), l, data);
  }
}
