#include "pcf85063a.hpp"

#include <chrono>
#include <cstdio>

// binary coded decimal conversion helper functions
uint8_t bcd_encode(uint v) {
  uint v10 = v / 10, v1 = v - (v10 * 10); return v1 | (v10 << 4); }
int8_t bcd_decode(uint v) {
  uint v10 = (v >> 4) & 0x0f, v1 = v & 0x0f; return v1 + (v10 * 10); }

namespace pimoroni {

  void PCF85063A::init() {
    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_set_pulls(interrupt, false, true);
    }
  }

  void PCF85063A::reset() {
    // magic soft reset command
    i2c->reg_write_uint8(address, Registers::CONTROL_1, 0x58);

    // read the oscillator status bit until it is cleared
    uint8_t status = 0x80;
    while(status & 0x80) {
      // attempt to clear oscillator stop flag, then read it back
      i2c->reg_write_uint8(address, Registers::OSCILLATOR_STATUS, 0x00);
      status = i2c->reg_read_uint8(address, Registers::OSCILLATOR_STATUS);
    }
  }

  // i2c helper methods
  i2c_inst_t* PCF85063A::get_i2c() const {
    return i2c->get_i2c();
  }

  int PCF85063A::get_address() const {
    return address;
  }

  int PCF85063A::get_sda() const {
    return i2c->get_sda();
  }

  int PCF85063A::get_scl() const {
    return i2c->get_scl();
  }

  int PCF85063A::get_int() const {
    return interrupt;
  }

  datetime_t PCF85063A::get_datetime() {
    uint8_t result[7] = {0};

    i2c->read_bytes(address, Registers::SECONDS, result, 7);

    datetime_t dt = {
      .year   = (int16_t)(bcd_decode(result[6]) + 2000), // offset year
      .month  = ( int8_t) bcd_decode(result[5]),
      .day    = ( int8_t) bcd_decode(result[3]),
      .dotw   = ( int8_t) bcd_decode(result[4]),
      .hour   = ( int8_t) bcd_decode(result[2]),
      .min    = ( int8_t) bcd_decode(result[1]),
      .sec    = ( int8_t) bcd_decode(result[0] & 0x7f) // mask out status bit
    };

    return dt;
  }

  void PCF85063A::set_datetime(datetime_t *t) {
    uint8_t data[7] = {
      bcd_encode((uint)t->sec),
      bcd_encode((uint)t->min),
      bcd_encode((uint)t->hour),
      bcd_encode((uint)t->day),
      bcd_encode((uint)t->dotw),
      bcd_encode((uint)t->month),
      bcd_encode((uint)t->year - 2000) // offset year
    };

    i2c->write_bytes(address, Registers::SECONDS, data, 7);
  }

  void PCF85063A::set_alarm(int second, int minute, int hour, int day) {
    uint8_t alarm[5] = {
      uint8_t(second != PARAM_UNUSED ? bcd_encode(second) : 0x80),
      uint8_t(minute != PARAM_UNUSED ? bcd_encode(minute) : 0x80),
      uint8_t(hour   != PARAM_UNUSED ? bcd_encode(  hour) : 0x80),
      uint8_t(day    != PARAM_UNUSED ? bcd_encode(   day) : 0x80),
      uint8_t(0x80)
    };

    i2c->write_bytes(address, Registers::SECOND_ALARM, alarm, 5);
  }

  void PCF85063A::set_weekday_alarm(
    int second, int minute, int hour, DayOfWeek dotw) {

    uint8_t alarm[5] = {
      uint8_t(second != PARAM_UNUSED ? bcd_encode(second) : 0x80),
      uint8_t(minute != PARAM_UNUSED ? bcd_encode(minute) : 0x80),
      uint8_t(hour   != PARAM_UNUSED ? bcd_encode(  hour) : 0x80),
      uint8_t(0x80),
      uint8_t(dotw   != DayOfWeek::NONE ? bcd_encode(  dotw) : 0x80)
    };

    i2c->write_bytes(address, Registers::SECOND_ALARM, alarm, 5);
  }

  void PCF85063A::enable_alarm_interrupt(bool enable) {
    uint8_t bits = i2c->reg_read_uint8(address, Registers::CONTROL_2);
    bits = enable ? (bits | 0x80) : (bits & ~0x80);
    bits |= 0x40; // ensure alarm flag isn't reset
    i2c->reg_write_uint8(address, Registers::CONTROL_2, bits);
  }

  bool PCF85063A::read_alarm_flag() {
    uint8_t bits = i2c->reg_read_uint8(address, Registers::CONTROL_2);
    return bits & 0x40;
  }

  void PCF85063A::clear_alarm_flag() {
    uint8_t bits = i2c->reg_read_uint8(address, Registers::CONTROL_2);
    bits &= ~0x40;
    i2c->reg_write_uint8(address, Registers::CONTROL_2, bits);
  }

  void PCF85063A::unset_alarm() {
    uint8_t dummy[5] = {0};
    i2c->write_bytes(address, Registers::SECOND_ALARM, dummy, 5);
  }

  void PCF85063A::set_timer(uint8_t ticks, TimerTickPeriod ttp) {
    uint8_t bits = i2c->reg_read_uint8(address, Registers::TIMER_MODE);

    uint8_t timer[2] = {
      ticks,
      uint8_t((bits & ~0x18) | (ttp << 3) | 0x04) // mask out current ttp and set new + enable
    };

    i2c->write_bytes(address, Registers::TIMER_VALUE, timer, 2);
  }

  void PCF85063A::enable_timer_interrupt(bool enable, bool flag_only) {
    uint8_t bits = i2c->reg_read_uint8(address, Registers::TIMER_MODE);
    bits = (bits & ~0x03) | (enable ? 0x02 : 0x00) | (flag_only ? 0x01 : 0x00);
    i2c->reg_write_uint8(address, Registers::TIMER_MODE, bits);
  }

  bool PCF85063A::read_timer_flag() {
    uint8_t bits = i2c->reg_read_uint8(address, Registers::CONTROL_2);
    return bits & 0x08;
  }

  void PCF85063A::clear_timer_flag() {
    uint8_t bits = i2c->reg_read_uint8(address, Registers::CONTROL_2);
    bits &= ~0x08;
    i2c->reg_write_uint8(address, Registers::CONTROL_2, bits);
  }

  void PCF85063A::unset_timer() {
    uint8_t bits = i2c->reg_read_uint8(address, Registers::TIMER_MODE);
    bits &= ~0x04;
    i2c->reg_write_uint8(address, Registers::TIMER_MODE, bits);
  }

  // set the speed of (or turn off) the clock output
  void PCF85063A::set_clock_output(ClockOut co) {
    uint8_t bits = i2c->reg_read_uint8(address, Registers::CONTROL_2);
    bits = (bits & ~0x07) | uint8_t(co);
    i2c->reg_write_uint8(
      address, Registers::CONTROL_2, bits);
  }

  void PCF85063A::set_byte(uint8_t v) {
    i2c->reg_write_uint8(address, Registers::RAM_BYTE, v);
  }

  uint8_t PCF85063A::get_byte() {
    return i2c->reg_read_uint8(address, Registers::RAM_BYTE);
  }

}
