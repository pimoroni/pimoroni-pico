#pragma once

#include "pico/stdlib.h"
#include <string>
#include "spi_drv.hpp"

namespace pimoroni {

  class IPAddress {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    union {
      uint8_t bytes[WL_IPV4_LENGTH];
      uint32_t dword;
    } addr;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    IPAddress();
    IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
    IPAddress(uint32_t address);
    IPAddress(const uint8_t *address);


    //--------------------------------------------------
    // Operators
    //--------------------------------------------------
  public:
    bool operator==(const IPAddress &address) const;
    bool operator==(const uint32_t &address) const;
    uint8_t operator[](int index) const;
    uint8_t& operator[](int index);
    IPAddress& operator=(uint32_t address);
    operator uint32_t() const;
    std::string to_string() const;
    const uint8_t* to_bytes() const;
  };

}