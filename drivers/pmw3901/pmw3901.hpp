#pragma once

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "../../common/pimoroni_common.hpp"

namespace pimoroni {

  class PMW3901 {
    spi_inst_t *spi = PIMORONI_SPI_DEFAULT_INSTANCE;

    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t FRAME_SIZE                         = 35;
    static const uint16_t FRAME_BYTES                       = 1225;
    static const uint16_t DEFAULT_MOTION_TIMEOUT_MS         = 5000;
    static const uint16_t DEFAULT_FRAME_CAPTURE_TIMEOUT_MS  = 10000;
  protected:
    static const uint8_t WAIT = -1;

    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum Degrees {
      DEGREES_0 = 0,
      DEGREES_90,
      DEGREES_180,
      DEGREES_270,
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    // interface pins with our standard defaults where appropriate
    uint cs         = SPI_BG_FRONT_CS;
    uint sck        = SPI_DEFAULT_SCK;
    uint mosi       = SPI_DEFAULT_MOSI;
    uint miso       = SPI_DEFAULT_MISO;
    uint interrupt  = SPI_BG_FRONT_PWM;

    uint32_t spi_baud = 400000;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    PMW3901() {}

    PMW3901(BG_SPI_SLOT slot) {
      switch(slot) {
        case PICO_EXPLORER_ONBOARD: // Don't read too much into this, the case is just here to avoid a compile error
          cs = SPI_BG_FRONT_CS;
          interrupt = PIN_UNUSED;
          break;
        case BG_SPI_FRONT:
          cs = SPI_BG_FRONT_CS;
          interrupt = SPI_BG_FRONT_PWM;
          break;
        case BG_SPI_BACK:
          cs = SPI_BG_BACK_CS;
          interrupt = SPI_BG_BACK_PWM;
          break;
      }
    }

    PMW3901(spi_inst_t *spi,
            uint cs, uint sck, uint mosi, uint miso, uint interrupt) :
      spi(spi),
      cs(cs), sck(sck), mosi(mosi), miso(miso), interrupt(interrupt) {}

    virtual ~PMW3901() {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    spi_inst_t* get_spi() const;
    int get_cs() const;
    int get_sck() const;
    int get_mosi() const;
    int get_miso() const;
    int get_int() const;

    uint8_t get_id();
    uint8_t get_revision();
    void set_rotation(Degrees degrees = DEGREES_0);
    void set_orientation(bool invert_x = true, bool invert_y = true, bool swap_xy = true);
    bool get_motion(int16_t& x_out, int16_t& y_out, uint16_t timeout_ms = DEFAULT_MOTION_TIMEOUT_MS);
    bool get_motion_slow(int16_t& x_out, int16_t& y_out, uint16_t timeout_ms = DEFAULT_MOTION_TIMEOUT_MS);
    bool frame_capture(uint8_t (&data_out)[FRAME_BYTES], uint16_t& data_size_out, uint16_t timeout_ms = DEFAULT_FRAME_CAPTURE_TIMEOUT_MS);

  protected:
    virtual void secret_sauce();

    void cs_select();
    void cs_deselect();
    void write_register(uint8_t reg, uint8_t data);
    void write_buffer(uint8_t *buf, uint16_t len);
    void read_registers(uint8_t reg, uint8_t *buf, uint16_t len);
    uint8_t read_register(uint8_t reg);
    uint32_t millis();
  };

  class PAA5100 : public PMW3901 {
  public:
    virtual ~PAA5100() {}
  protected:
    virtual void secret_sauce();
  };
}
