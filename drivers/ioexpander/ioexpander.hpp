#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_i2c.hpp"

namespace pimoroni {

  class IOExpander {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  private:
    // These values encode our desired pin function: IO, ADC, PWM
    // alongwide the GPIO MODE for that port and pin (section 8.1)
    // 1st and 2nd bits encode the gpio state
    // 3rd and 4th bits encode the IO mode (i.e. IO, PWM, ADC)
    // the 5th bit additionally encodes the default output state
    static const uint8_t PIN_MODE_IO = 0b00000;   // General IO mode, IE: not ADC or PWM
    static const uint8_t PIN_MODE_QB = 0b00000;   // Output, Quasi-Bidirectional mode
    static const uint8_t PIN_MODE_PP = 0b00001;   // Output, Push-Pull mode
    static const uint8_t PIN_MODE_IN = 0b00010;   // Input-only (high-impedance)
    static const uint8_t PIN_MODE_PU = 0b10000;   // Input (with pull-up)
    static const uint8_t PIN_MODE_OD = 0b00011;   // Output, Open-Drain mode
    static const uint8_t PIN_MODE_PWM = 0b00101;  // PWM, Output, Push-Pull mode
    static const uint8_t PIN_MODE_ADC = 0b01010;  // ADC, Input-only (high-impedance)

    static const uint32_t RESET_TIMEOUT_MS = 1000;

  public:
    static const uint8_t DEFAULT_I2C_ADDRESS = 0x18;

    static const uint16_t CHIP_ID = 0xE26A;
    static const uint8_t CHIP_VERSION = 2;

    static const uint8_t PIN_IN = PIN_MODE_IN;          // 0b00010
    static const uint8_t PIN_IN_PULL_UP = PIN_MODE_PU;  // 0b10000
    static const uint8_t PIN_IN_PU = PIN_MODE_PU;       // 0b10000
    static const uint8_t PIN_OUT = PIN_MODE_PP;         // 0b00001
    static const uint8_t PIN_OD = PIN_MODE_OD;          // 0b00001
    static const uint8_t PIN_PWM = PIN_MODE_PWM;        // 0b00101
    static const uint8_t PIN_ADC = PIN_MODE_ADC;        // 0b01010

    static const uint8_t NUM_PINS = 14;

    static const uint16_t LOW = 0;
    static const uint16_t HIGH = 1;

    static const uint32_t CLOCK_FREQ = 24000000;
    static const uint32_t MAX_PERIOD = (1 << 16) - 1;
    static const uint32_t MAX_DIVIDER = (1 << 7);


    //--------------------------------------------------
    // Subclasses
    //--------------------------------------------------
  private:
    class Pin {
    public:
      //--------------------------------------------------
      // Enums
      //--------------------------------------------------
      enum IOType {
        TYPE_IO =         0b00,
        TYPE_PWM =        0b01,
        TYPE_ADC =        0b10,
        TYPE_ADC_OR_PWM = 0b11
      };


      //--------------------------------------------------
      // Constants
      //--------------------------------------------------
    private:
      // The PxM1 and PxM2 registers encode GPIO MODE
      // 0 0 = Quasi-bidirectional
      // 0 1 = Push-pull
      // 1 0 = Input-only (high-impedance)
      // 1 1 = Open-drain
      static const uint8_t PxM1[4];     // [reg::P0M1, reg::P1M1, -1, reg::P3M1]
      static const uint8_t PxM2[4];     // [reg::P0M2, reg::P1M2, -1, reg::P3M2]

      // The Px input register
      static const uint8_t Px[4];       // [reg::P0, reg::P1, -1, reg::P3]

      // The PxS Schmitt trigger register
      static const uint8_t PxS[4];      // [reg::P0S, reg::P1S, -1, reg::P3S]
      static const uint8_t MASK_P[4];   // [reg::INT_MASK_P0, reg::INT_MASK_P1, -1, reg::INT_MASK_P3]

      static const uint8_t PWML[6];     // [reg::PWM0L, reg::PWM1L, reg::PWM2L, reg::PWM3L, reg::PWM4L, reg::PWM5L]
      static const uint8_t PWMH[6];     // [reg::PWM0H, reg::PWM1H, reg::PWM2H, reg::PWM3H, reg::PWM4H, reg::PWM5H]


      //--------------------------------------------------
      // Variables
      //--------------------------------------------------
    private:
      const IOType type;
      uint8_t mode;
    public:
      const uint8_t port;
      const uint8_t pin;
      const uint8_t adc_channel;
      const uint8_t pwm_channel;

      const uint8_t reg_m1;
      const uint8_t reg_m2;
      const uint8_t reg_p;
      const uint8_t reg_ps;
      const uint8_t reg_int_mask_p;

      const uint8_t reg_io_pwm;
      const uint8_t reg_pwml;
      const uint8_t reg_pwmh;


      //--------------------------------------------------
      // Constructors
      //--------------------------------------------------
    private:
      Pin(uint8_t port, uint8_t pin);                                                               // Constructor for IO pin
      Pin(uint8_t port, uint8_t pin, uint8_t pwm_channel, uint8_t reg_iopwm);                       // Constructor for PWM pin
      Pin(uint8_t port, uint8_t pin, uint8_t adc_channel);                                          // Constructor for ADC pin
      Pin(uint8_t port, uint8_t pin, uint8_t adc_channel, uint8_t pwm_channel, uint8_t reg_iopwm);  // Constructor for ADC or PWM pin


      //--------------------------------------------------
      // Methods
      //--------------------------------------------------
    public:    
      static Pin io(uint8_t port, uint8_t pin);                                                                       // Nicer function for creating an IO pin
      static Pin pwm(uint8_t port, uint8_t pin, uint8_t channel, uint8_t reg_iopwm);                                  // Nicer function for creating a PWM pin
      static Pin adc(uint8_t port, uint8_t pin, uint8_t channel);                                                     // Nicer function for creating an ADC pin
      static Pin adc_or_pwm(uint8_t port, uint8_t pin, uint8_t adc_channel, uint8_t pwm_channel, uint8_t reg_iopwm);  // Nicer function for creating an ADC or PWM pin

      IOType get_type(void);
      uint8_t get_mode(void);
      void set_mode(uint8_t mode);
      
      bool mode_supported(uint8_t mode);
    };

  
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    I2C *i2c;

    // interface pins with our standard defaults where appropriate
    int8_t address    = DEFAULT_I2C_ADDRESS;
    uint interrupt    = PIN_UNUSED;

    uint32_t timeout;
    bool debug;
    float vref;
    int16_t encoder_offset[4];
    int16_t encoder_last[4];
    Pin pins[NUM_PINS];

  
    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    IOExpander() :
        IOExpander(new I2C(), DEFAULT_I2C_ADDRESS, PIN_UNUSED, timeout, debug) {};

    IOExpander(uint8_t address, uint32_t timeout = 1, bool debug = false) :
        IOExpander(new I2C(), address, PIN_UNUSED, timeout, debug) {};

    IOExpander(I2C *i2c, uint8_t address=DEFAULT_I2C_ADDRESS, uint interrupt = PIN_UNUSED, uint32_t timeout = 1, bool debug = false);


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(bool skip_chip_id_check = false, bool perform_reset = false);

  private:
    uint8_t check_reset();
  public:
    bool reset();

    // For print access in micropython
    i2c_inst_t* get_i2c() const;
    int get_address() const;
    int get_sda() const;
    int get_scl() const;
    int get_int() const;

    uint16_t get_chip_id();

    void set_address(uint8_t address);

    float get_adc_vref();
    void set_adc_vref(float vref);

    void enable_interrupt_out(bool pin_swap = false);
    void disable_interrupt_out();
    bool get_interrupt_flag();
    void clear_interrupt_flag();
    bool set_pin_interrupt(uint8_t pin, bool enabled);

    void pwm_load(bool wait_for_load = true);
    bool pwm_loading();
    void pwm_clear(bool wait_for_clear = true);
    bool pwm_clearing();
    bool set_pwm_control(uint8_t divider);
    void set_pwm_period(uint16_t value, bool load = true, bool wait_for_load = true);
    uint16_t set_pwm_frequency(float frequency, bool load = true, bool wait_for_load = true);

    uint8_t get_mode(uint8_t pin);
    void set_mode(uint8_t pin, uint8_t mode, bool schmitt_trigger = false, bool invert = false);

    int16_t input(uint8_t pin, uint32_t adc_timeout = 1);
    float input_as_voltage(uint8_t pin, uint32_t adc_timeout = 1);

    void output(uint8_t pin, uint16_t value, bool load = true, bool wait_for_load = true);

    void setup_rotary_encoder(uint8_t channel, uint8_t pin_a, uint8_t pin_b, uint8_t pin_c = 0, bool count_microsteps = false);
    int16_t read_rotary_encoder(uint8_t channel);
    void clear_rotary_encoder(uint8_t channel);

  private:
    uint8_t i2c_reg_read_uint8(uint8_t reg);
    void i2c_reg_write_uint8(uint8_t reg, uint8_t value);

    uint8_t get_bit(uint8_t reg, uint8_t bit);
    void set_bits(uint8_t reg, uint8_t bits);
    void set_bit(uint8_t reg, uint8_t bit);
    void clr_bits(uint8_t reg, uint8_t bits);
    void clr_bit(uint8_t reg, uint8_t bit); 
    void change_bit(uint8_t reg, uint8_t bit, bool state);

    void wait_for_flash();

    uint32_t millis();
  };

}
