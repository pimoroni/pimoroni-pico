#pragma once

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "../../common/pimoroni_common.hpp"


namespace pimoroni {

  class ST7789 {
    spi_inst_t *spi = PIMORONI_SPI_DEFAULT_INSTANCE;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    // screen properties
    uint16_t width;
    uint16_t height;
    bool round;

    // interface pins with our standard defaults where appropriate
    uint cs;
    uint dc;
    uint sck;
    uint mosi;
    uint bl;
    uint vsync  = PIN_UNUSED; // only available on some products

    // The ST7789 requires 16 ns between SPI rising edges.
    // 16 ns = 62,500,000 Hz
    static const uint32_t SPI_BAUD = 62'500'000;


  public:
    // frame buffer where pixel data is stored
    uint16_t *frame_buffer;

    ST7789(uint16_t width, uint16_t height, bool round, uint16_t *frame_buffer,
           spi_inst_t *spi,
           uint cs, uint dc, uint sck, uint mosi, uint bl = PIN_UNUSED) :
      spi(spi),
      width(width), height(height), round(round),
      cs(cs), dc(dc), sck(sck), mosi(mosi), bl(bl), frame_buffer(frame_buffer) {

        if(!this->frame_buffer) {
          this->frame_buffer = new uint16_t[width * height];
        }

        // configure spi interface and pins
        spi_init(spi, SPI_BAUD);

        gpio_set_function(dc, GPIO_FUNC_SIO);
        gpio_set_dir(dc, GPIO_OUT);

        gpio_set_function(cs, GPIO_FUNC_SIO);
        gpio_set_dir(cs, GPIO_OUT);

        gpio_set_function(sck,  GPIO_FUNC_SPI);
        gpio_set_function(mosi, GPIO_FUNC_SPI);

        // if a backlight pin is provided then set it up for
        // pwm control
        if(bl != PIN_UNUSED) {
          pwm_config cfg = pwm_get_default_config();
          pwm_set_wrap(pwm_gpio_to_slice_num(bl), 65535);
          pwm_init(pwm_gpio_to_slice_num(bl), &cfg, true);
          gpio_set_function(bl, GPIO_FUNC_PWM);
          set_backlight(0); // Turn backlight off initially to avoid nasty surprises
        }

      }


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void init();
    void configure_display(bool rotate180);

    spi_inst_t* get_spi() const;
    uint get_cs() const;
    uint get_dc() const;
    uint get_sck() const;
    uint get_mosi() const;
    uint get_bl() const;

    void command(uint8_t command, size_t len = 0, const char *data = NULL);
    void update();
    void set_backlight(uint8_t brightness);
    void flip();

    static uint get_slot_cs(BG_SPI_SLOT slot) {
      switch(slot) {
        case PICO_EXPLORER_ONBOARD:
          return SPI_BG_FRONT_CS;
        case BG_SPI_FRONT:
          return SPI_BG_FRONT_CS;
        case BG_SPI_BACK:
          return SPI_BG_BACK_CS;
      }
      return PIN_UNUSED;
    };

    static uint get_slot_bl(BG_SPI_SLOT slot) {
      switch(slot) {
        case PICO_EXPLORER_ONBOARD:
          return PIN_UNUSED;
        case BG_SPI_FRONT:
          return SPI_BG_FRONT_PWM;
        case BG_SPI_BACK:
          return SPI_BG_BACK_PWM;
      }
      return PIN_UNUSED;
    };
  };

}
