#pragma once

#include "pico/stdlib.h"
#include <iostream>

namespace pimoroni {

  enum LoggingLevel {
    LOG_NONE = 0,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG
  };


  struct logger {
    uint level = LOG_INFO;

    void warn(std::string message) {
      if(level >= LOG_WARN) {
        print(message);
      }
    }

    void info(std::string message) {
      if(level >= LOG_INFO) {
        print(message);
      }
    }

    void debug(std::string message) {
      if(level >= LOG_DEBUG) {
        print(message);
      }
    }
    //stdio_usb_connected()
    void print(std::string message) {
      if(stdio_usb_connected()) {
        std::cout << message;
      }
    }
  };

  extern logger logging;

}
