#include "ip_address.hpp"

namespace pimoroni {

  IPAddress::IPAddress() {
    addr.dword = 0;
  }

  IPAddress::IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
    addr.bytes[0] = first_octet;
    addr.bytes[1] = second_octet;
    addr.bytes[2] = third_octet;
    addr.bytes[3] = fourth_octet;
  }
    
  IPAddress::IPAddress(uint32_t address) {
    addr.dword = address;
  }

  IPAddress::IPAddress(const uint8_t *address) {
    addr.bytes[0] = address[0];
    addr.bytes[1] = address[1];
    addr.bytes[2] = address[2];
    addr.bytes[3] = address[3];
  }

  bool IPAddress::operator==(const IPAddress &address) const {
    return (addr.dword == address.addr.dword);
  }

  bool IPAddress::operator==(const uint32_t &address) const {
    return (addr.dword == address);
  }

  uint8_t IPAddress::operator[](int index) const {
    return addr.bytes[index];
  }

  uint8_t& IPAddress::operator[](int index) {
    return addr.bytes[index];
  }

  IPAddress& IPAddress::operator=(uint32_t address) {
    addr.dword = address;
    return *this;
  }   

  IPAddress::operator uint32_t() const {
    return addr.dword;
  }

  std::string IPAddress::to_string() const {
    char buf[16] = {0};
    snprintf(buf, 16, "%d.%d.%d.%d", addr.bytes[0], addr.bytes[1], addr.bytes[2], addr.bytes[3]);
    return std::string(buf, 16);
  }

  const uint8_t* IPAddress::to_bytes() const {
    return addr.bytes;
  }

}