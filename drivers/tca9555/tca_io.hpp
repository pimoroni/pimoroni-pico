#pragma once

#include "pico/stdlib.h"

namespace pimoroni {

  struct TCA {
    uint CHIP;
    uint GPIO;
  };

  class TCAAccessor {
  public:
    virtual bool get_slow_input(TCA gpio) = 0;
    virtual bool get_slow_output(TCA gpio) = 0;
    virtual bool get_slow_config(TCA gpio) = 0;
    virtual bool get_slow_polarity(TCA gpio) = 0;

    virtual void set_slow_output(TCA gpio, bool value) = 0;
    virtual void set_slow_config(TCA gpio, bool output) = 0;
    virtual void set_slow_polarity(TCA gpio, bool polarity) = 0;
  };

  class TCA_IO {
  public:
    TCA_IO(TCA pin, TCAAccessor& accessor);
    TCA_IO(TCA pin, TCAAccessor& accessor, bool out);
    ~TCA_IO();
    bool direction();
    void direction(bool out);
    void to_output(bool val);
    void to_input();
    bool value();
    void value(bool val);
  private:
    TCA pin;
    TCAAccessor& accessor;
  };

}
