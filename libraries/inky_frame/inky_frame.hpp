#pragma once

#include <string>

#include "drivers/uc8159/uc8159.hpp"
#include "drivers/pcf85063a/pcf85063a.hpp"
#include "drivers/fatfs/ff.h"

#include "libraries/pico_graphics/pico_graphics.hpp"

namespace pimoroni {

  class InkyFrame : public PicoGraphics_Pen3Bit {
  public:
    enum Button : uint8_t {
      BUTTON_A = 0,
      BUTTON_B = 1,
      BUTTON_C = 2,
      BUTTON_D = 3,
      BUTTON_E = 4
    };

    enum LED : uint8_t {
      LED_ACTIVITY    = 6,
      LED_CONNECTION  = 7,
      LED_A           = 11,
      LED_B           = 12,
      LED_C           = 13,
      LED_D           = 14,
      LED_E           = 15
    };

    enum Flags : uint8_t {
      RTC_ALARM = 5,
      EXTERNAL_TRIGGER = 6,
      EINK_BUSY = 7
    };

    enum WakeUpEvent : uint8_t {
      UNKNOWN_EVENT = 0,
      BUTTON_A_EVENT = 1,
      BUTTON_B_EVENT = 2,
      BUTTON_C_EVENT = 3,
      BUTTON_D_EVENT = 4,
      BUTTON_E_EVENT = 5,
      RTC_ALARM_EVENT = 6,
      EXTERNAL_TRIGGER_EVENT = 7,
    };

    enum Pen : uint8_t {
      BLACK  = 0,
      WHITE  = 1,
      GREEN  = 2,
      BLUE   = 3,
      RED    = 4,
      YELLOW = 5,
      ORANGE = 6,
      CLEAN  = 7,
      TAUPE  = 7
    };

  protected:
    WakeUpEvent _wake_up_event = UNKNOWN_EVENT;

    enum Pin {
      HOLD_VSYS_EN  =  2,
      I2C_INT       =  3,
      I2C_SDA       =  4,
      I2C_SCL       =  5,
      SR_CLOCK      =  8,
      SR_LATCH      =  9,
      SR_OUT        = 10,
      MISO          = 16,
      EINK_CS       = 17,
      CLK           = 18,
      MOSI          = 19,
      SD_DAT0       = 19,
      SD_DAT1       = 20,
      SD_DAT2       = 21,
      SD_DAT3       = 22,
      SD_CS         = 22,
      ADC0          = 26,
      EINK_RESET    = 27,
      EINK_DC       = 28
    };

  public:
    UC8159 uc8159;
    I2C i2c;
    PCF85063A rtc;

    int width;
    int height;

    [[deprecated("Use instance variable width.")]] 
    static const int WIDTH = 600;
    [[deprecated("Use instance variable height.")]]
    static const int HEIGHT = 448;

    // Default 5.7" constructor
    InkyFrame() : InkyFrame(600, 448) {};

    // 600x448 for 5.7"
    // 640x400 for 4.0"
    InkyFrame(int width, int height) :
      PicoGraphics_Pen3Bit(width, height, nullptr),
      uc8159(width, height, {spi0, EINK_CS, CLK, MOSI, PIN_UNUSED, EINK_DC, PIN_UNUSED}),
      i2c(4, 5),
      rtc(&i2c),
      width(width),
      height(height) {
    }

    void init();

    // wake/sleep management
    void sleep(int wake_in_minutes = -1);
    void sleep_until(int second = -1, int minute = -1, int hour = -1, int day = -1);
    WakeUpEvent get_wake_up_event() {return _wake_up_event;}

    // screen management
    void update(bool blocking=false);
    static bool is_busy();

    // state
    bool pressed(Button button);
    static uint8_t read_shift_register();
    static bool read_shift_register_bit(uint8_t index);
    void led(LED led, uint8_t brightness);

    void icon(const uint8_t *data, int sheet_width, int icon_size, int index, int dx, int dy);
    void image(const uint8_t* data);
    void image(const uint8_t *data, int w, int h, int x, int y);
    void image(const uint8_t *data, int stride, int sx, int sy, int dw, int dh, int dx, int dy);
  };

}
