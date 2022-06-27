#pragma once

#include "hardware/i2c.h"
#include "hardware/rtc.h"

#include "common/pimoroni_common.hpp"
#include "common/pimoroni_i2c.hpp"

namespace pimoroni {

  class PCF85063A {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint DEFAULT_I2C_ADDRESS      = 0x51;
    static const int PARAM_UNUSED              = -1;


    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum ClockOut : uint8_t {
      CLOCK_OUT_32768HZ = 0,
      CLOCK_OUT_16384HZ = 1,
      CLOCK_OUT_8192HZ  = 2,
      CLOCK_OUT_4096HZ  = 3,
      CLOCK_OUT_2048HZ  = 4,
      CLOCK_OUT_1024HZ  = 5,
      CLOCK_OUT_1HZ     = 6,
      CLOCK_OUT_OFF     = 7
    };

    enum DayOfWeek : int8_t {
      SUNDAY    =  0,
      MONDAY    =  1,
      TUESDAY   =  2,
      WEDNESDAY =  3,
      THURSDAY  =  4,
      FRIDAY    =  5,
      SATURDAY  =  6,
      NONE      = PARAM_UNUSED
    };

    enum TimerTickPeriod : int8_t {
      TIMER_TICK_4096HZ       = 0b00,
      TIMER_TICK_64HZ         = 0b01,
      TIMER_TICK_1HZ          = 0b10,
      TIMER_TICK_1_OVER_60HZ  = 0b11
    };

  private:
    enum Registers : uint8_t {
      CONTROL_1         = 0x00,
      CONTROL_2         = 0x01,
      OFFSET            = 0x02,
      RAM_BYTE          = 0x03,
      OSCILLATOR_STATUS = 0x04, // flag embedded in seconds register (see below)
      SECONDS           = 0x04, // contains oscillator status flag   (see above)
      MINUTES           = 0x05,
      HOURS             = 0x06,
      DAYS              = 0x07,
      WEEKDAYS          = 0x08,
      MONTHS            = 0x09,
      YEARS             = 0x0A,
      SECOND_ALARM      = 0x0B,
      MINUTE_ALARM      = 0x0C,
      HOUR_ALARM        = 0x0D,
      DAY_ALARM         = 0x0E,
      WEEKDAY_ALARM     = 0x0F,
      TIMER_VALUE       = 0x10,
      TIMER_MODE        = 0x11
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    I2C *i2c;

    // Interface pins with our standard defaults where appropriate
    uint address        = DEFAULT_I2C_ADDRESS;
    uint interrupt      = PIN_UNUSED;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    PCF85063A() {}

    PCF85063A(I2C *i2c, uint interrupt = PIN_UNUSED) :
        i2c(i2c), interrupt(interrupt) {}

    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void init();
    void reset();

    // I2C instance details access methods
    i2c_inst_t* get_i2c() const;
    int get_address() const;
    int get_sda() const;
    int get_scl() const;
    int get_int() const;

    // Set and get the date and time
    // Uses datetime_t from pico sdk (hardware/rtc) for compatibility
    datetime_t get_datetime();
    void set_datetime(datetime_t *t);

    // Alarm manipulation methods
    void set_alarm(int second = PARAM_UNUSED, int minute = PARAM_UNUSED,
      int hour = PARAM_UNUSED, int day = PARAM_UNUSED);
    void set_weekday_alarm(int second = PARAM_UNUSED, int minute = PARAM_UNUSED,
      int hour = PARAM_UNUSED, DayOfWeek dotw = DayOfWeek::NONE);
    void enable_alarm_interrupt(bool enable);
    bool read_alarm_flag();
    void clear_alarm_flag();
    void unset_alarm();

    // Timer manipulation methods
    void set_timer(uint8_t ticks,
      TimerTickPeriod ttp = TimerTickPeriod::TIMER_TICK_1HZ);
    void enable_timer_interrupt(bool enable, bool flag_only = false);
    bool read_timer_flag();
    void clear_timer_flag();
    void unset_timer();

    // Clock output
    void set_clock_output(ClockOut co);

    // FREE byte of RAM!!
    void set_byte(uint8_t v);
    uint8_t get_byte();
  };

}