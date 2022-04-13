#pragma once

#include "hardware/pio.h"
#include "common/pimoroni_common.hpp"
#include "capture.hpp"

namespace pimoroni {

  struct bool_pair {
    union {
      bool first;
      bool a;
    };
    union {
      bool second;
      bool b;
    };

    bool_pair() : first(false), second(false) {}
    bool_pair(bool first, bool second) : first(first), second(second) {}
  };

  class Encoder {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static constexpr float DEFAULT_COUNTS_PER_REV   = 24;
    static const uint16_t DEFAULT_COUNT_MICROSTEPS  = false;
    static const uint16_t DEFAULT_FREQ_DIVIDER      = 1;

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
    PIO pio;
    uint sm;
    pin_pair enc_pins;
    uint pin_c;

    const float counts_per_revolution   = DEFAULT_COUNTS_PER_REV;
    const bool count_microsteps         = DEFAULT_COUNT_MICROSTEPS;
    const uint16_t freq_divider         = DEFAULT_FREQ_DIVIDER;
    const float clocks_per_time         = 0;

    //--------------------------------------------------

    volatile bool enc_state_a           = false;
    volatile bool enc_state_b           = false;
    volatile int32_t enc_count          = 0;
    volatile int32_t time_since         = 0;
    volatile Direction last_travel_dir  = NO_DIR;
    volatile int32_t microstep_time     = 0;
    volatile int32_t cumulative_time    = 0;

    int32_t count_offset                = 0;
    int32_t last_captured_count         = 0;

    bool initialised = false;


    //--------------------------------------------------
    // Statics
    //--------------------------------------------------
    static Encoder* encoders[NUM_PIOS][NUM_PIO_STATE_MACHINES];
    static uint8_t claimed_sms[NUM_PIOS];
    static uint pio_program_offset[NUM_PIOS];
    static void pio_interrupt_handler(uint pio_idx);
    static void pio0_interrupt_handler();
    static void pio1_interrupt_handler();


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Encoder() {}
    Encoder(PIO pio, uint sm, const pin_pair &pins, uint pin_c = PIN_UNUSED,
            float counts_per_revolution = DEFAULT_COUNTS_PER_REV, bool count_microsteps = DEFAULT_COUNT_MICROSTEPS,
            uint16_t freq_divider = DEFAULT_FREQ_DIVIDER);
    ~Encoder();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    // For print access in micropython
    pin_pair pins() const;

    bool_pair state() const;
    int32_t count() const;
    float revolutions() const;
    float angle_degrees() const;
    float angle_radians() const;

    float frequency() const;
    float revolutions_per_second() const;
    float revolutions_per_minute() const;
    float degrees_per_second() const;
    float radians_per_second() const;
    
    void zero_count();
    Capture perform_capture(); //TODO rename capture to snapshot

  private:
    void microstep_up(int32_t time_since);
    void microstep_down(int32_t time_since);
    void check_for_transition();
  };

}