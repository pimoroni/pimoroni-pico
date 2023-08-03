#pragma once

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
      if(level >= LOG_WARN)
        std::cout << message;
    }

    void info(std::string message) {
      if(level >= LOG_INFO) {
        std::cout << message;
      }
    }

    void debug(std::string message) {
      if(level >= LOG_DEBUG) {
        std::cout << message;
      }
    }
  };

  extern logger logging;

}
