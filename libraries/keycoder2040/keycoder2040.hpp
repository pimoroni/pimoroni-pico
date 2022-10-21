#pragma once
#include <cfloat>
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "keycoder.pio.h"
#include "common/pimoroni_i2c.hpp"
#include "drivers/is31fl3731/is31fl3731.hpp"

#define DEFAULT_PIO pio0

namespace pimoroni {

  
    struct RGBLookup {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

  namespace keycoder2040 {


    class LEDs : public IS31FL3731{

      public:
        static constexpr uint8_t WIDTH = 4;
        static constexpr uint8_t HEIGHT = 4;
        static constexpr int8_t DEFAULT_I2C_ADDRESS    = 0x74;
        static constexpr int8_t ALTERNATE_I2C_ADDRESS  = 0x77;

        bool init();
        void set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
        void set_pixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
      
        LEDs(): IS31FL3731(new I2C(I2C_BG_SDA, I2C_BG_SCL), DEFAULT_I2C_ADDRESS) {};

        private:
        RGBLookup lookup_pixel(uint8_t index);

        // This wonderful lookup table maps the LEDs on RGB Matrix 5x5
        // from their 3x5x5 (remember, they're RGB) configuration to
        // their specific location in the 144 pixel buffer.
        const RGBLookup lookup_table[16] = {
            {128, 32, 64},
            {129, 33, 65},
            {130, 17, 66},
            {131, 18, 67},
            {112, 80, 96},
            {113, 81, 97},
            {114, 82, 98},
            {115, 83, 99},
            {136, 40, 72},
            {137, 41, 73},
            {138, 25, 74},
            {139, 26, 75},
            {120, 88, 104},
            {121, 89, 105},
            {122, 90, 106},
            {123, 91, 107}
            
        };
    };

    

    static constexpr float ROTARY_CPR = 24.0f;
    // Constants

    const uint ENCODER_ROW1_A = 8;
    const uint ENCODER_ROW1_B = 9;
    const uint ENCODER_ROW2_A = 10;
    const uint ENCODER_ROW2_B = 11;
    const uint ENCODER_ROW3_A = 12;
    const uint ENCODER_ROW3_B = 13;
    const uint ENCODER_ROW4_A = 14;
    const uint ENCODER_ROW4_B = 15;

    const uint ENCODER_COL1 = 16;
    const uint ENCODER_COL2 = 17;
    const uint ENCODER_COL3 = 18;
    const uint ENCODER_COL4 = 19;


    static constexpr float DEFAULT_COUNTS_PER_REV   = ROTARY_CPR;
    static const bool DEFAULT_COUNT_MICROSTEPS      = true;
    static const uint16_t DEFAULT_FREQ_DIVIDER      = 20;


    const uint ENCODERS_START = 8;
    const uint ENCODERS_WIDTH = 8;
    const uint32_t ENCODERS_MASK = 0x0000FF00;

    const uint COLUMN_START = 16;
    const uint COLUMN_WIDTH = 4;
    const uint32_t COLUMN_MASK = 0x000F0000;



    

    class Keycoder2040{

    // The counts per revolution of the motor's output shaft
    static constexpr float COUNTS_PER_REV = 96.0;

    // The direction to spin the motor in. NORMAL_DIR (0), REVERSED_DIR (1)
    const Direction DIRECTION = NORMAL_DIR;


    const uint USER_SW = 23;

      public:
        static uint32_t last_state ;
        static uint32_t new_state ;
      
      private:
              // state stores for the whole encoder grid 
        
        static const uint32_t COL_1_MASK = 0xFF000000;
        static const uint32_t COL_2_MASK = 0x00FF0000;
        static const uint32_t COL_3_MASK = 0x0000FF00;
        static const uint32_t COL_4_MASK = 0x000000FF;
        static const uint8_t  COL_1_LSHFT = 24;
        static const uint8_t  COL_2_LSHFT = 16;
        static const uint8_t  COL_3_LSHFT = 8;
        static const uint8_t  COL_4_LSHFT = 0;


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




      // Public Variables
      
      public:

        // Statics
        static Keycoder2040* encoders[NUM_PIOS][NUM_PIO_STATE_MACHINES];
        static uint8_t claimed_sms[NUM_PIOS];
        static uint pio_program_offset[NUM_PIOS];
        static void pio_interrupt_handler(uint pio_idx);
        static void pio0_interrupt_handler();
        static void pio1_interrupt_handler();
        static void process_steps(PIO pio, uint sm);
        static void microstep(uint key_number , bool up);
        static PIO keycoder2040_pio;
        static uint keycoder2040_sm;
        static uint8_t states_new[16];
        static uint8_t states_last[16];
        static int32_t encoders_count[16];
        static bool count_microsteps;
        static StepDir step_dir;

      
      // Constructor
      public:
          
        Keycoder2040(PIO pio, uint sm,  Direction direction = NORMAL_DIR,
          float counts_per_rev = DEFAULT_COUNTS_PER_REV, bool count_microsteps = DEFAULT_COUNT_MICROSTEPS,
          uint16_t freq_divider = DEFAULT_FREQ_DIVIDER);
      

    
        

      // Private Variables
      private:
        PIO pio;
        uint sm;
        uint enc_common_pin;
        Direction enc_direction;
        float enc_counts_per_rev;
        const uint16_t freq_divider;
        
        
      public:
      //Public Methods 
        int32_t count() const;
        int32_t delta() const;
        float frequency() const;
        uint32_t get_last_state();

        float revolutions() const;
        float degrees() const;
        float radians() const;

        float revolutions_delta() const;
        float degrees_delta() const;
        float radians_delta() const;
        bool init();
        
    };




    


  }
}