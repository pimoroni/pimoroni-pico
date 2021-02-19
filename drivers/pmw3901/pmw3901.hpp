#pragma once

#include "hardware/spi.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class PMW3901 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_SCK_PIN    = 18;
    static const uint8_t DEFAULT_CS_PIN     = 22;
    static const uint8_t DEFAULT_MOSI_PIN   = 19;
    static const uint8_t DEFAULT_MISO_PIN   = 16;    
    static const uint8_t DEFAULT_INT_PIN    = 21;

    /***** More public constants here *****/

  private:
    /***** Private constants here *****/


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    spi_inst_t *spi   = spi0;

    // interface pins with our standard defaults where appropriate
    int8_t sck        = DEFAULT_SCK_PIN;
    int8_t mosi       = DEFAULT_MOSI_PIN;
    int8_t miso       = DEFAULT_MISO_PIN;    
    int8_t cs         = DEFAULT_CS_PIN;
    int8_t interrupt  = DEFAULT_INT_PIN;

    /***** More variables here *****/


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    PMW3901() {}

    PMW3901(spi_inst_t *spi, uint8_t sck, uint8_t mosi, uint8_t miso, uint8_t cs, uint8_t interrupt) :
      spi(spi), sck(sck), mosi(mosi), miso(miso), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(); //This should be present in all drivers

    uint8_t get_id();
    void set_rotation(float degrees = 0.0f);
    void set_orientation(bool invert_x = true, bool invert_y = true, bool swap_xy = true);
    uint8_t get_motion(uint8_t timeout = 5);
    uint8_t get_motion_slow(uint8_t timeout = 5);

  private:
    void write(uint8_t reg, uint8_t value);
    uint8_t read(uint8_t reg, uint8_t length = 1);
    void bulk_write(uint8_t data);
    void secret_sauce();
  };

}
