#pragma once

#include "hardware/i2c.h"
#include "../../common/pimoroni_common.hpp"

namespace pimoroni {

  class PCF85063A {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint DEFAULT_SDA_PIN          = 4;
    static const uint DEFAULT_SCL_PIN          = 5;
    static const uint8_t I2C_ADDRESS           = 0x51;

    static const uint8_t COMMAND_RESET         = 0b01011000;

    static const uint8_t ALARM_INTERRUPT       = 0b10000000;
    static const uint8_t CLEAR_ALARM_FLAG      = 0b01000000;
    static const uint8_t MINUTE_INTERRUPT      = 0b00100000;
    static const uint8_t HALF_MINUTE_INTERRUPT = 0b00010000;
    static const uint8_t TIMER_FLAG            = 0b00001000;

    static const uint8_t CLOCK_OUT_32768HZ     = 0b00000000;
    static const uint8_t CLOCK_OUT_16384HZ     = 0b00000001;
    static const uint8_t CLOCK_OUT_8192HZ      = 0b00000010;
    static const uint8_t CLOCK_OUT_4096HZ      = 0b00000011;
    static const uint8_t CLOCK_OUT_2048HZ      = 0b00000100;
    static const uint8_t CLOCK_OUT_1024HZ      = 0b00000101;
    static const uint8_t CLOCK_OUT_1HZ         = 0b00000110;
    static const uint8_t CLOCK_OUT_OFF         = 0b00000111;

    static const uint8_t RTC_STOP              = 0b00100000;
    static const uint8_t MODE_12_HOUR          = 0b00000010;
    static const uint8_t MODE_24_HOUR          = 0b00000000;
    static const uint8_t CAP_7PF               = 0b00000000;
    static const uint8_t CAP_12_5PF            = 0b00000001;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    // ????

  public:
    // ????

  private:
    i2c_inst_t *i2c     = i2c0;

    // interface pins with our standard defaults where appropriate
    uint sda            = DEFAULT_SDA_PIN;
    uint scl            = DEFAULT_SCL_PIN;
    uint interrupt      = PIN_UNUSED;

    uint32_t i2c_baud    = 400000;

    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    PCF85063A() {}

    PCF85063A(i2c_inst_t *i2c, uint sda, uint scl, uint interrupt = PIN_UNUSED) : 
        i2c(i2c), sda(sda), scl(scl), interrupt(interrupt) {}

    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void init();
    void reset();

    bool set_datetime(datetime_t *t);
    datetime_t get_datetime();

    void set_second_alarm(uint sec);
    void configure(uint8_t flags);

    void reset_timer();
    void set_seconds_timer(uint8_t sec);

    i2c_inst_t* get_i2c() const;
    int get_sda() const;
    int get_scl() const;
    int get_interrupt() const;    
  };

}