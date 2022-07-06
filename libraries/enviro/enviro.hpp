#pragma once
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pimoroni_i2c.hpp"
#include "pcf85063a.hpp"

uint32_t time();

using namespace pimoroni;

namespace enviro {

  enum TemperatureUnits {
    CELCIUS,
    FAHRENHEIT,
    KELVIN
  };

  enum PressureUnits {
    HECTOPASCAL,
    INHG,
    PSI,
    BAR
  };

  class EnviroBase {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint HOLD_VSYS_EN_PIN = 2;
    static const uint I2C_INTERRUPT_PIN = 3;
    static const uint I2C_SDA_PIN = 4;
    static const uint I2C_SCL_PIN = 5;
    static const uint ACTIVITY_LED_PIN = 6;
    static const uint SWITCH_PIN = 7;
    static const uint RTC_ALARM_PIN = 8;


    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum WarnLEDState {
      WARN_LED_OFF = 0,
      WARN_LED_ON = 1,
      WARN_LED_BLINK = 2
    };

    enum WakeReason {
      REASON_SWITCH_PRESSED = 0,
      REASON_RTC_ALARM = 1,
      REASON_CAMERA_TRIGGER = 2,
      REASON_RAIN_TRIGGER = 3,
      REASON_NONE = -1,
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    I2C i2c;
    PCF85063A rtc;
  protected:
    WakeReason woken_up_by;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    EnviroBase();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    void sleep(int seconds = -1);
    void activity_led(bool on);
    void activity_led(float brightness);
    void warn_led(WarnLEDState wls);
    bool switch_pressed();
    WakeReason wake_reason(bool clear = false);
  };

}
