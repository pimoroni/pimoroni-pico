#pragma once

#include "hardware/pio.h"
#include "capture.hpp"

namespace pimoroni {

  class Encoder {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static constexpr float DEFAULT_COUNTS_PER_REV   = 24;
    static const uint16_t DEFAULT_COUNT_MICROSTEPS  = false;
    static const uint16_t DEFAULT_FREQ_DIVIDER      = 1;    
    static const uint8_t PIN_UNUSED                 = UINT8_MAX;

  private:
    static const uint32_t STATE_A_MASK      = 0x80000000;
    static const uint32_t STATE_B_MASK      = 0x40000000;
    static const uint32_t STATE_A_LAST_MASK = 0x20000000;
    static const uint32_t STATE_B_LAST_MASK = 0x10000000;

    static const uint32_t STATES_MASK = STATE_A_MASK | STATE_B_MASK |
                                        STATE_A_LAST_MASK | STATE_B_LAST_MASK;

    static const uint32_t TIME_MASK   = 0x0fffffff;

    static const uint8_t MICROSTEP_0  = 0b00;
    static const uint8_t MICROSTEP_1  = 0b10;
    static const uint8_t MICROSTEP_2  = 0b11;
    static const uint8_t MICROSTEP_3  = 0b01;


    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  private:
    enum Direction {
      NO_DIR        = 0,
      CLOCKWISE     = 1,
      COUNTERCLOCK  = -1,
    };
  
  
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    const PIO enc_pio   = pio0;
    const uint8_t pinA  = PIN_UNUSED;
    const uint8_t pinB  = PIN_UNUSED;
    const uint8_t pinC  = PIN_UNUSED;

    const float counts_per_revolution   = DEFAULT_COUNTS_PER_REV;
    const bool count_microsteps         = DEFAULT_COUNT_MICROSTEPS;
    const uint16_t freq_divider         = DEFAULT_FREQ_DIVIDER;
    const float clocks_per_time         = 0;

    //--------------------------------------------------
    
    uint enc_sm         = 0;
    uint enc_offset     = 0;

    volatile bool stateA                = false;
    volatile bool stateB                = false;
    volatile int32_t count              = 0;
    volatile int32_t time_since         = 0;
    volatile Direction last_travel_dir  = NO_DIR;
    volatile int32_t microstep_time     = 0;
    volatile int32_t cumulative_time    = 0;

    int32_t count_offset                = 0;
    int32_t last_captured_count         = 0;


    //--------------------------------------------------
    // Statics
    //--------------------------------------------------
  public:
    static Encoder* pio_encoders[NUM_PIOS][NUM_PIO_STATE_MACHINES];
    static uint8_t pio_claimed_sms[NUM_PIOS];
    static void pio0_interrupt_callback();
    static void pio1_interrupt_callback();


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Encoder() {}
    Encoder(PIO pio, uint8_t pinA, uint8_t pinB, uint8_t pinC = PIN_UNUSED,
            float counts_per_revolution = DEFAULT_COUNTS_PER_REV, bool count_microsteps = DEFAULT_COUNT_MICROSTEPS,
            uint16_t freq_divider = DEFAULT_FREQ_DIVIDER);
    ~Encoder();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:    
    bool init();

    bool get_state_a() const;
    bool get_state_b() const;
    int32_t get_count() const;
    float get_revolutions() const;
    float get_angle_degrees() const;
    float get_angle_radians() const;

    float get_frequency() const;    
    float get_revolutions_per_second() const;
    float get_revolutions_per_minute() const;
    float get_degrees_per_second() const;
    float get_radians_per_second() const;
    
    void zero_count();
    Capture perform_capture();

  private:
    void microstep_up(int32_t time_since);
    void microstep_down(int32_t time_since);
    void check_for_transition();
  };

}