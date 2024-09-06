#pragma once

#include <string>

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_i2c.hpp"
#include "as7343_regs.hpp"

namespace pimoroni {

  class AS7343 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS  = 0x39;

    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum class channel_count : uint8_t {
      SIX_CHANNEL      = 6,
      TWELVE_CHANNEL   = 12,
      EIGHTEEN_CHANNEL = 18
    };
/*
    enum class COMPENSATION_GAIN : float {
        F1 = 1.84f,
        F2 = 6.03f,
        FZ = 4.88f,
        F3 = 13.74f,
        F4 = 3.37f,
        FY = 2.82f,
        F5 = 6.72f,
        FXL = 2.22f,
        F6 = 3.17f,
        F7 = 1.95f,
        F8 = 12.25f,
        NIR = 1.00f,
    };
*/

    //--------------------------------------------------
    // Substructures
    //--------------------------------------------------
  public:
    struct reading {
        // Cycle 1
        uint16_t c1_astatus = 0; // (c1_astatus >> 8) & 0b10001111
                                 // 0b10000000 = saturated
                                 // 0b00001111 = gain lowest nibble
        uint16_t FZ = 0;   // 428-480 nm
        uint16_t FY = 0;   // 534-593 nm
        uint16_t FXL = 0;  // 593-628 nm
        uint16_t NIR = 0;  // 849-903 nm

        uint16_t c1_vis_tl = 0;  // Visible top-left
        uint16_t c1_vis_br = 0;  // Visible bottom-right
        //uint16_t c1_fd = 0;    // This is where flicker detect *would* be

        // Cycle 2
        uint16_t c2_astatus = 0;
        uint16_t F2 = 0;   // 408-448 nm
        uint16_t F3 = 0;   // 448-500 mn
        uint16_t F4 = 0;   // 500-534 nm
        uint16_t F6 = 0;   // 618-665 nm

        uint16_t c2_vis_tl = 0;
        uint16_t c2_vis_br = 0;
        //uint16_t c2_fd = 0;

        // Cycle 3
        uint16_t c3_astatus = 0;
        uint16_t F1 = 0;   // 396-408 nm
        uint16_t F7 = 0;   // 685-715 nm
        uint16_t F8 = 0;   // 715-766 nm
        uint16_t F5 = 0;   // 531-594 nm

        uint16_t c3_vis_tl = 0;
        uint16_t c3_vis_br = 0;
        //uint16_t c3_fd = 0;

        float gain(uint8_t cycle) {
          uint8_t status = 0;
          switch(cycle) {
            case 1:
              status = c1_astatus & 0xf;
              break;
            case 2:
              status = c2_astatus & 0xf;
              break;
            case 3:
              status = c3_astatus & 0xf;
              break;
          }
          return status ? 1 << (status - 1) : 0.5f;
        }

        bool saturated(uint8_t cycle) {
          switch(cycle) {
            case 1:
              return c1_astatus & 0x80;
            case 2:
              return c2_astatus & 0x80;
            case 3:
              return c3_astatus & 0x80;
            default:
              return false;
          }
        }
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    I2C *i2c;

    // interface pins with our standard defaults where appropriate
    int8_t address    = DEFAULT_I2C_ADDRESS;
    uint interrupt    = PIN_UNUSED;

    uint8_t read_cycles = 1;
    uint8_t ch_count = (uint8_t)channel_count::SIX_CHANNEL;

    bool running = false;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    AS7343(uint interrupt = PIN_UNUSED) : AS7343(new I2C(), interrupt) {};

    AS7343(I2C *i2c, uint interrupt = PIN_UNUSED) : i2c(i2c), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();
    void reset();

    // For print access in micropython
    i2c_inst_t* get_i2c() const;
    int get_sda() const;
    int get_scl() const;
    int get_int() const;

    void get_version(uint8_t &auxid, uint8_t &revid, uint8_t &hwid);

    void start_measurement();
    void stop_measurement();

    reading read();

    void read_fifo(uint16_t *buf);

    void set_gain(float gain);
    void set_illumination_current(float current);
    void set_illumination_led(bool state);
    void set_measurement_time(float measurement_time_ms);
    void set_integration_time(float integration_time_us, uint8_t repeat=1);

    void set_channels(channel_count channel_count);

private:
    void bank_select(uint8_t bank=0);
  };

}
