#pragma once

#include "hardware/pio.h"
#include "common/pimoroni_common.hpp"

using namespace pimoroni;

namespace encoder {
  static constexpr float MMME_CPR = 12.0f;
  static constexpr float ROTARY_CPR = 24.0f;

  class Encoder {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static constexpr float DEFAULT_COUNTS_PER_REV   = ROTARY_CPR;
    static const bool DEFAULT_COUNT_MICROSTEPS      = false;
    static const uint16_t DEFAULT_FREQ_DIVIDER      = 1;

  private:
    static const uint32_t STATE_A_MASK      = 0x80000000;
    static const uint32_t STATE_B_MASK      = 0x40000000;
    static const uint32_t STATE_A_LAST_MASK = 0x20000000;
    static const uint32_t STATE_B_LAST_MASK = 0x10000000;

    static const uint32_t STATES_MASK = STATE_A_MASK | STATE_B_MASK |
                                        STATE_A_LAST_MASK | STATE_B_LAST_MASK;

    static const uint32_t TIME_MASK   = 0x0fffffff;


    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  private:
    enum StepDir {
      NO_DIR    = 0,
      INCREASING = 1,
      DECREASING = -1,
    };

    enum MicroStep : uint8_t {
      MICROSTEP_0 = 0b00,
      MICROSTEP_1 = 0b10,
      MICROSTEP_2 = 0b11,
      MICROSTEP_3 = 0b01,
    };


    //--------------------------------------------------
    // Substructures
    //--------------------------------------------------
  public:
    class Capture {
        //--------------------------------------------------
        // Variables
        //--------------------------------------------------
      private:
        int32_t captured_count;
        int32_t captured_delta;
        float captured_frequency;
        float counts_per_rev;


        //--------------------------------------------------
        // Constructors
        //--------------------------------------------------
      public:
        Capture();
        Capture(int32_t count, int32_t delta, float frequency, float counts_per_rev);


        //--------------------------------------------------
        // Methods
        //--------------------------------------------------
      public:
        int32_t count() const;
        int32_t delta() const;
        float frequency() const;

        float revolutions() const;
        float degrees() const;
        float radians() const;

        float revolutions_delta() const;
        float degrees_delta() const;
        float radians_delta() const;

        float revolutions_per_second() const;
        float revolutions_per_minute() const;
        float degrees_per_second() const;
        float radians_per_second() const;
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    PIO pio;
    uint sm;
    pin_pair enc_pins;
    uint enc_common_pin;
    Direction enc_direction;
    float enc_counts_per_rev;

    const bool count_microsteps;
    const uint16_t freq_divider;
    const float clocks_per_time;

    //--------------------------------------------------

    volatile bool enc_state_a             = false;
    volatile bool enc_state_b             = false;
    volatile int32_t enc_count            = 0;
    volatile int32_t enc_cumulative_time  = 0;
    volatile int16_t enc_step             = 0;
    volatile int16_t enc_turn             = 0;

    volatile int32_t microstep_time       = 0;
    volatile StepDir step_dir             = NO_DIR;

    int32_t last_count                    = 0;
    int32_t last_capture_count            = 0;

    bool initialised                      = false;


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
    Encoder(PIO pio, uint sm, const pin_pair &pins, uint common_pin = PIN_UNUSED, Direction direction = NORMAL_DIR,
            float counts_per_rev = DEFAULT_COUNTS_PER_REV, bool count_microsteps = DEFAULT_COUNT_MICROSTEPS,
            uint16_t freq_divider = DEFAULT_FREQ_DIVIDER);
    ~Encoder();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    // For print access in micropython
    pin_pair pins() const;
    uint common_pin() const;

    bool_pair state() const;
    int32_t count() const;
    int32_t delta();
    void zero();

    int16_t step() const;
    int16_t turn() const;

    float revolutions() const;
    float degrees() const;
    float radians() const;

    //--------------------------------------------------

    Direction direction() const;
    void direction(Direction direction);

    float counts_per_rev() const;
    void counts_per_rev(float counts_per_rev);

    Capture capture();

    //--------------------------------------------------
  private:
    void process_steps();
    void microstep(int32_t time_since, bool up);
  };

}