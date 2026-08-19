#include "psram_display.hpp"

#include <algorithm>
#include <cstdlib>
#include <math.h>
#include <string.h>

namespace pimoroni {

  enum reg {
    WRITE = 0x02,
    READ = 0x03,
    WRAP_BOUNDARY_TOGGLE = 0xC0,
    RESET_ENABLE = 0x66,
    RESET = 0x99
  };

  static constexpr uint BAUDRATE = 31'250'000;
  static constexpr uint32_t COMMAND_BYTES = 4;
  static constexpr uint32_t WRAP_PROBE_BYTES = 40;
  // straddles a 1K boundary, so both the 32 byte and 1K wrap modes fold the probe
  static constexpr uint32_t SCRATCH_ADDRESS = 0x400000 - (WRAP_PROBE_BYTES / 2);

  void PSRamDisplay::init() {
    BusConfig previous = {spi_get_hw(spi)->cpsr, spi_get_hw(spi)->cr0};

    uint baud = spi_init(spi, BAUDRATE);
    printf("PSRam connected at %u\n", baud);

    bus_cpsr = spi_get_hw(spi)->cpsr;
    bus_cr0 = spi_get_hw(spi)->cr0;

    // tCEM caps CS-low at 8us, which is baud / 1e6 bytes including the command header
    uint32_t tcem_bytes = baud / 1'000'000;
    chunk_size = tcem_bytes > COMMAND_BYTES ? ((tcem_bytes - COMMAND_BYTES) / 8) * 8 : 0;
    chunk_size = std::clamp<uint32_t>(chunk_size, 8, MAX_CHUNK_BYTES);

    gpio_set_function(CS, GPIO_FUNC_SIO);
    gpio_set_dir(CS, GPIO_OUT);
    gpio_put(CS, 1);

    gpio_set_function(SCK,  GPIO_FUNC_SPI);
    gpio_set_function(MOSI, GPIO_FUNC_SPI);
    gpio_set_function(MISO, GPIO_FUNC_SPI);

    reset();

    if(is_wrapped()) {
      command(WRAP_BOUNDARY_TOGGLE);
      if(is_wrapped()) printf("PSRam stuck in wrapped burst mode\n");
    }

    // SSPCPSR reads zero until something configures this instance, so a nonzero
    // value means another driver had the shared bus set up first
    if(previous.cpsr != 0) release_bus(previous);
  }

  PSRamDisplay::BusConfig PSRamDisplay::claim_bus()
  {
    BusConfig previous = {spi_get_hw(spi)->cpsr, spi_get_hw(spi)->cr0};

    if(previous.cpsr != bus_cpsr || previous.cr0 != bus_cr0) {
      spi_get_hw(spi)->cpsr = bus_cpsr;
      spi_get_hw(spi)->cr0 = bus_cr0;
    }

    return previous;
  }

  void PSRamDisplay::release_bus(const BusConfig &previous)
  {
    if(previous.cpsr != bus_cpsr || previous.cr0 != bus_cr0) {
      spi_get_hw(spi)->cpsr = previous.cpsr;
      spi_get_hw(spi)->cr0 = previous.cr0;
    }
  }

  void PSRamDisplay::command(uint8_t value)
  {
    gpio_put(CS, 0);
    spi_write_blocking(spi, &value, 1);
    gpio_put(CS, 1);
  }

  void PSRamDisplay::reset()
  {
    // reset enable and reset must each be framed by their own CS pulse
    command(RESET_ENABLE);
    sleep_us(1);
    command(RESET);
    sleep_us(10);
  }

  bool PSRamDisplay::is_wrapped()
  {
    uint8_t pattern[WRAP_PROBE_BYTES];
    uint8_t readback[WRAP_PROBE_BYTES];

    for(auto i = 0u; i < WRAP_PROBE_BYTES; i++) pattern[i] = (uint8_t)(i * 7 + 1);

    // single bursts, deliberately over both the 32 byte wrap boundary and tCEM
    write_burst(SCRATCH_ADDRESS, WRAP_PROBE_BYTES, pattern);
    read_burst(SCRATCH_ADDRESS, WRAP_PROBE_BYTES, readback);

    return memcmp(pattern, readback, WRAP_PROBE_BYTES) != 0;
  }

  void PSRamDisplay::write_burst(uint32_t address, size_t len, const uint8_t *data)
  {
    uint8_t command_buffer[COMMAND_BYTES] = {WRITE, (uint8_t)((address >> 16) & 0xFF), (uint8_t)((address >> 8) & 0xFF), (uint8_t)(address & 0xFF)};
    gpio_put(CS, 0);
    spi_write_blocking(spi, command_buffer, COMMAND_BYTES);
    spi_write_blocking(spi, data, len);
    gpio_put(CS, 1);
  }

  void PSRamDisplay::fill_burst(uint32_t address, size_t len, const uint8_t byte)
  {
    uint8_t command_buffer[COMMAND_BYTES] = {WRITE, (uint8_t)((address >> 16) & 0xFF), (uint8_t)((address >> 8) & 0xFF), (uint8_t)(address & 0xFF)};
    gpio_put(CS, 0);
    spi_write_blocking(spi, command_buffer, COMMAND_BYTES);
    SpiSetBlocking(byte, len);
    gpio_put(CS, 1);
  }

  void PSRamDisplay::read_burst(uint32_t address, size_t len, uint8_t *data)
  {
    uint8_t command_buffer[COMMAND_BYTES] = {READ, (uint8_t)((address >> 16) & 0xFF), (uint8_t)((address >> 8) & 0xFF), (uint8_t)(address & 0xFF)};
    gpio_put(CS, 0);
    spi_write_blocking(spi, command_buffer, COMMAND_BYTES);
    spi_read_blocking(spi, 0, data, len);
    gpio_put(CS, 1);
  }

  void PSRamDisplay::write(uint32_t address, size_t len, const uint8_t *data)
  {
    flush();

    BusConfig previous = claim_bus();

    while(len > 0) {
      size_t chunk = std::min(len, (size_t)chunk_size);
      write_burst(address, chunk, data);
      address += chunk;
      data += chunk;
      len -= chunk;
    }

    release_bus(previous);
  }

  void PSRamDisplay::write(uint32_t address, size_t len, const uint8_t byte)
  {
    flush();

    BusConfig previous = claim_bus();

    while(len > 0) {
      size_t chunk = std::min(len, (size_t)chunk_size);
      fill_burst(address, chunk, byte);
      address += chunk;
      len -= chunk;
    }

    release_bus(previous);
  }

  void PSRamDisplay::read(uint32_t address, size_t len, uint8_t *data)
  {
    flush();

    BusConfig previous = claim_bus();

    while(len > 0) {
      size_t chunk = std::min(len, (size_t)chunk_size);
      read_burst(address, chunk, data);
      address += chunk;
      data += chunk;
      len -= chunk;
    }

    release_bus(previous);
  }

  void PSRamDisplay::flush()
  {
    if(pending_len == 0) return;

    size_t len = pending_len;
    pending_len = 0;

    BusConfig previous = claim_bus();
    write_burst(pending_address, len, pending_data);
    release_bus(previous);
  }

  // Single pixel writes arrive in address order along each row, so combine them
  // into one burst per chunk rather than one transaction per pixel.
  void PSRamDisplay::write_pixel(const Point &p, uint8_t colour)
  {
    uint32_t address = pointToAddress(p);

    if(pending_len > 0 && (pending_address + pending_len != address || pending_len >= chunk_size)) {
      flush();
    }

    if(pending_len == 0) pending_address = address;

    pending_data[pending_len++] = colour;
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
