#pragma once

#include "pico/stdlib.h"
#include "../../drivers/esp32wireless/esp32wireless.hpp"

namespace pimoroni {

  class PicoWireless : public WifiNINA {
    typedef WifiNINA parent;
    
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t A = 12;

    static const uint8_t ESP_LED_R = 25;
    static const uint8_t ESP_LED_G = 26;
    static const uint8_t ESP_LED_B = 27;


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
  };

}