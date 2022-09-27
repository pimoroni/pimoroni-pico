#pragma once

#include "drivers/ioexpander/ioexpander.hpp"
#include "drivers/is31fl3731/is31fl3731.hpp"
#include "common/pimoroni_common.hpp"

namespace pimoroni {

  class BreakoutEncoderWheel {
    struct RGBLookup {
      uint8_t r;
      uint8_t g;
      uint8_t b;
    };

    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum Direction : bool {
      DIRECTION_CW = true,
      DIRECTION_CCW = false
    };


    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_IOE_I2C_ADDRESS    = 0x13;
    static const uint8_t DEFAULT_LED_I2C_ADDRESS    = 0x77;
    static const uint8_t LED_I2C_ADDRESS_ALTERNATE  = 0x74;

    static const Direction DEFAULT_DIRECTION  = DIRECTION_CW;
    static const uint32_t DEFAULT_TIMEOUT     = 1;

  private:
    static const uint8_t SWITCH_CENTRE  = 1;
    static const uint8_t SWITCH_UP      = 13;
    static const uint8_t SWITCH_LEFT    = 11;
    static const uint8_t SWITCH_DOWN    = 4;
    static const uint8_t SWITCH_RIGHT   = 2;

    static const uint8_t ENC_TERM_A     = 12;
    static const uint8_t ENC_TERM_B     = 3;

    static const uint8_t ENC_CHANNEL    = 1;

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
    //Direction direction = DEFAULT_DIRECTION;
    uint interrupt_pin = PIN_UNUSED;     // A local copy of the value passed to the IOExpander, used in initialisation


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
    Direction get_encoder_direction();
    void set_encoder_direction(Direction direction);

    void set_pixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
    //void update(uint8_t frame = 0);
    //void clear();

    bool wheel_available();
    int16_t read_wheel();
    void clear_wheel();
    bool read_switch();
  };

}