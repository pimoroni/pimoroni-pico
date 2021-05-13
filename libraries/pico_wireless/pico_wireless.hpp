#pragma once

#include "pico/stdlib.h"
#include "drivers/esp32spi/esp32spi.hpp"

namespace pimoroni {

  class PicoWireless : public Esp32Spi {
    typedef Esp32Spi parent;
    
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t A = 12;

    static const uint8_t ESP_LED_R = 25;
    static const uint8_t ESP_LED_G = 26;
    static const uint8_t ESP_LED_B = 27;

    static const uint8_t ESP_SD_DETECT = 15;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:


  public:
    //PicoWireless();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    virtual bool init();

    void set_led(uint8_t r, uint8_t g, uint8_t b);
    bool is_pressed(uint8_t button);
    bool is_sdcard_detected();
  };

}