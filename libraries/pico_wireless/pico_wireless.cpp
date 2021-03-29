#include "pico_wireless.hpp"
#include "../../drivers/esp32wireless/spi_drv.hpp"

namespace pimoroni {

  bool PicoWireless::init() {
    bool success = parent::init();

    // setup button input
    gpio_init(A);
    gpio_set_dir(A, GPIO_IN);
    gpio_pull_up(A);

    set_led(0, 0, 0);

    return success;
  }

  void PicoWireless::set_led(uint8_t r, uint8_t g, uint8_t b) {
    analog_write(ESP_LED_R, 255 - r);
    analog_write(ESP_LED_G, 255 - g);
    analog_write(ESP_LED_B, 255 - b);
  }

  bool PicoWireless::is_pressed(uint8_t button) {
    return !gpio_get(button);
  }

}