#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_i2c.hpp"

namespace pimoroni {

  class MSA301 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS    = 0x26;
    
    static const uint8_t SOFT_RESET             = 0x00;
    static const uint8_t PART_ID                = 0x01;
    static const uint8_t MOTION_INTERRUPT       = 0x09;
    static const uint8_t DATA_INTERRUPT         = 0x0a;
    static const uint8_t ORIENTATION_STATUS     = 0x0c;
    static const uint8_t RESOLUTION_RANGE       = 0x0f;
    static const uint8_t POWER_MODE_BANDWIDTH   = 0x11;
    static const uint8_t SET_AXIS_POLARITY      = 0x12;
    static const uint8_t INTERRUPT_ENABLE_0     = 0x16;
    static const uint8_t INTERRUPT_ENABLE_1     = 0x17;
    static const uint8_t INTERRUPT_LATCH_PERIOD = 0x21;
    static const uint8_t FREEFALL_DURATION      = 0x22;


    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum Axis {
      X = 0x02,
      Y = 0x04,
      Z = 0x06
    };

    enum Orientation {
      PORTRAIT            = 0b00,
      PORTRAIT_INVERTED   = 0b01,
      LANDSCAPE           = 0b10,
      LANDSCAPE_INVERTED  = 0b11
    };

    enum PowerMode {
      NORMAL  = 0b00,
      LOW     = 0b01,
      SUSPEND = 0b10
    };

    enum Range {
      G_2   = 0b00,
      G_4   = 0b01,
      G_8   = 0b10,
      G_16  = 0b11
    };

    enum Resolution {
      BITS_14   = 0b0000,
      BITS_12   = 0b0100,
      BITS_10   = 0b1000,
      BITS_8    = 0b1100
    };
    
    enum AxisPolarity {
      INVERT_X  = 0b1000,
      INVERT_Y  = 0b0100,
      INVERT_Z  = 0b0010,
      XY_SWAP   = 0b0001
    };

    enum Interrupt {
      NONE          = 0,
      ACTIVE        = 0b0000111,
      NEW_DATA      = 0b1000000000000,
      FREEFALL      = 0b0100000000000,
      ORIENTATION   = 0b1000000,
      SINGLE_TAP    = 0b0100000,
      DOUBLE_TAP    = 0b0010000,
      Z_ACTIVE      = 0b0000100,
      Y_ACTIVE      = 0b0000010,
      X_ACTIVE      = 0b0000001
    };

    enum InterruptLatchPeriod {
      LATCH_1MS     = 0b1001,
      LATCH_2MS     = 0b1011,
      LATCH_25MS    = 0b1100,
      LATCH_50MS    = 0b1101,
      LATCH_100MS   = 0b1110,
      LATCH_250MS   = 0b0001,
      LATCH_500MS   = 0b0010,
      LATCH_1S      = 0b0011,
      LATCH_2S      = 0b0100,
      LATCH_4S      = 0b0101,
      LATCH_8S      = 0b0110
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    I2C *i2c;
    const uint8_t address  = DEFAULT_I2C_ADDRESS;
    uint interrupt         = PIN_UNUSED;

    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    MSA301() : MSA301(new I2C()) {};

    MSA301(I2C *i2c, uint interrupt = PIN_UNUSED) : i2c(i2c), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();
    void reset();

    i2c_inst_t* get_i2c() const;
    int get_sda() const;
    int get_scl() const;
    int get_int() const;

    uint8_t part_id();
    float get_axis(Axis axis, uint8_t sample_count = 1);
    float get_x_axis(uint8_t sample_count = 1);
    float get_y_axis(uint8_t sample_count = 1);
    float get_z_axis(uint8_t sample_count = 1);
    Orientation get_orientation();

    void set_power_mode(MSA301::PowerMode power_mode);
    void set_range_and_resolution(Range range, MSA301::Resolution resolution);
    void set_axis_polarity(uint8_t polarity);

    void disable_all_interrupts();
    void enable_interrupts(uint16_t interrupts);
    void set_interrupt_latch(InterruptLatchPeriod latch_period, bool reset_latched);
    bool read_interrupt(Interrupt interrupt);
  };
}

