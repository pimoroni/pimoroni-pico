#pragma once

#include <string>

namespace pimoroni {

  class OverVoltageError : public std::exception {
  private:
    std::string message;

  public:
    OverVoltageError(std::string msg) : message("OverVoltageError: " + msg) {
    }

    virtual const char* what() const noexcept {
      return message.c_str();
    }
  };

  class UnderVoltageError : public std::exception {
  private:
    std::string message;

  public:
    UnderVoltageError(std::string msg) : message("UnderVoltageError: " + msg) {
    }

    virtual const char* what() const noexcept {
      return message.c_str();
    }
  };

  class OverCurrentError : public std::exception {
  private:
    std::string message;

  public:
    OverCurrentError(std::string msg) : message("OverCurrentError: " + msg) {
    }

    virtual const char* what() const noexcept {
      return message.c_str();
    }
  };

  class OverTemperatureError : public std::exception {
  private:
    std::string message;

  public:
    OverTemperatureError(std::string msg) : message("OverTemperatureError: " + msg) {
    }

    virtual const char* what() const noexcept {
      return message.c_str();
    }
  };

  class FaultError : public std::exception {
  private:
    std::string message;

  public:
    FaultError(std::string msg) : message("FaultError: " + msg) {
    }

    virtual const char* what() const noexcept {
      return message.c_str();
    }
  };

  class VerificationError : public std::exception {
  private:
    std::string message;

  public:
    VerificationError(std::string msg) : message(msg) {
    }

    virtual const char* what() const noexcept {
      return ("VerificationError: " + message).c_str();
    }
  };

}
