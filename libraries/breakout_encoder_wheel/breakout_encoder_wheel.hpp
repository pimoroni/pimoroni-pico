#pragma once

#include "drivers/ioexpander/ioexpander.hpp"
#include "drivers/is31fl3731/is31fl3731.hpp"
#include "common/pimoroni_common.hpp"

namespace pimoroni {
namespace encoderwheel {
  static const uint8_t NUM_LEDS = 24;
  static const uint8_t NUM_BUTTONS = 5;
  static const uint8_t NUM_GPIOS = 3;

  static const uint8_t UP = 0;
  static const uint8_t DOWN = 1;
  static const uint8_t LEFT = 2;
  static const uint8_t RIGHT = 3;
  static const uint8_t CENTRE = 4;

  static const uint8_t GP7 = 7;
  static const uint8_t GP8 = 8;
  static const uint8_t GP9 = 9;
  static const uint8_t GPIOS[] = {GP7, GP8, GP9};

  class BreakoutEncoderWheel {
    struct RGBLookup {
      uint8_t r;
      uint8_t g;
      uint8_t b;
    };


    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_IOE_I2C_ADDRESS    = 0x13;
    static const uint8_t DEFAULT_LED_I2C_ADDRESS    = 0x77;
    static const uint8_t ALTERNATE_LED_I2C_ADDRESS  = 0x74;

    static const Direction DEFAULT_DIRECTION  = NORMAL_DIR;
    static const uint32_t DEFAULT_TIMEOUT     = 1;

  private:
    static const uint8_t ENC_CHANNEL          = 1;
    static const uint8_t ENC_TERM_A           = 3;
    static const uint8_t ENC_TERM_B           = 12;
    static const uint8_t ENC_COUNTS_PER_REV   = 24;
    static const uint8_t ENC_COUNT_DIVIDER    = 2;

    static const uint8_t SW_UP                = 13;
    static const uint8_t SW_DOWN              = 4;
    static const uint8_t SW_LEFT              = 11;
    static const uint8_t SW_RIGHT             = 2;
    static const uint8_t SW_CENTRE            = 1;

    // This wonderful lookup table maps the LEDs on the encoder wheel
    // from their 3x24 (remember, they're RGB) configuration to
    // their specific location in the 144 pixel buffer.
    static constexpr RGBLookup lookup_table[24] = {
      {128, 32, 48},
      {129, 33, 49},
      {130, 17, 50},
      {131, 18, 34},
      {132, 19, 35},
      {133, 20, 36},
      {134, 21, 37},
      {112, 80, 96},
      {113, 81, 97},
      {114, 82, 98},
      {115, 83, 99},
      {116, 84, 100},
      {117, 68, 101},
      {118, 69, 85},
      {127, 47, 63},
      {121, 41, 57},
      {122, 25, 58},
      {123, 26, 42},
      {124, 27, 43},
      {125, 28, 44},
      {126, 29, 45},
      {15, 95, 111},
      {8, 89, 105},
      {9, 90, 106},
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    IOExpander ioe;
    IS31FL3731 led_ring;

    Direction enc_direction = DEFAULT_DIRECTION;
    int16_t enc_count = 0;
    int16_t enc_step = 0;
    int16_t enc_turn = 0;
    int16_t last_raw_count = 0;
    int16_t last_delta_count = 0;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BreakoutEncoderWheel(uint8_t ioe_address = DEFAULT_IOE_I2C_ADDRESS, uint8_t led_address = DEFAULT_LED_I2C_ADDRESS)
      : BreakoutEncoderWheel(new I2C(), ioe_address, led_address) {}

    BreakoutEncoderWheel(I2C *i2c, uint8_t ioe_address = DEFAULT_IOE_I2C_ADDRESS, uint8_t led_address = DEFAULT_LED_I2C_ADDRESS, uint interrupt = PIN_UNUSED, uint32_t timeout = DEFAULT_TIMEOUT, bool debug = false)
      : ioe(i2c, ioe_address, interrupt, timeout, debug), led_ring(i2c, led_address) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(bool skip_chip_id_check = false);

    // For print access in micropython
    i2c_inst_t* get_i2c() const;
    int get_ioe_address() const;
    int get_led_address() const;
    int get_sda() const;
    int get_scl() const;
    int get_int() const;

    // Calls through to IOExpander class
    void set_ioe_address(uint8_t address);
    bool get_interrupt_flag();
    void clear_interrupt_flag();

    // Encoder breakout specific
    bool pressed(uint button);
    int16_t count();
    int16_t delta();
    void zero();
    int16_t step();
    int16_t turn();
    float revolutions();
    float degrees();
    float radians();
    Direction direction();
    void direction(Direction direction);
    void set_rgb(int index, int r, int g, int b);
    void set_hsv(int index, float h, float s = 1.0f, float v = 1.0f);
    void clear();
    void show();

    uint8_t gpio_pin_mode(uint8_t gpio);
    void gpio_pin_mode(uint8_t gpio, uint8_t mode);
    int16_t gpio_pin_value(uint8_t gpio);
    float gpio_pin_value_as_voltage(uint8_t gpio);
    void gpio_pin_value(uint8_t gpio, uint16_t value, bool load = true, bool wait_for_load = false);
    void gpio_pwm_load(bool wait_for_load = true);
    uint16_t gpio_pwm_frequency(float frequency, bool load = true, bool wait_for_load = false);

  private:
    void take_encoder_reading();
  };
}
}