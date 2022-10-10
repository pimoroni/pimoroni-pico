#include "st7567.hpp"

#include <cstdlib>
#include <math.h>

#include "hardware/dma.h"
#include "hardware/pwm.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

namespace pimoroni {

  enum reg : uint8_t {
    DISPOFF   = 0xAE,//
    DISPON    = 0xAF,//
    SETSTARTLINE = 0x40,//
    STARTLINE_MASK = 0x3f,//
    REG_RATIO = 0x20,//
    SETPAGESTART = 0xb0,//
    PAGESTART_MASK = 0x07,//
    SETCOLL = 0x00,  //       # 0x00-0x0f: Set lower column address */
    COLL_MASK = 0x0f,//
    SETCOLH = 0x10,         //# 0x10-0x1f: Set higher column address */
    COLH_MASK = 0x0F,//
    SEG_DIR_NORMAL = 0xa0,  //# 0xa0: Column address 0 is mapped to SEG0 */
    SEG_DIR_REV = 0xa1,     //# 0xa1: Column address 128 is mapped to S
    DISPNORMAL = 0xa6,      //# 0xa6: Normal display */
    DISPINVERSE = 0xa7,     //# 0xa7: Inverse disp
    DISPRAM = 0xa4,         //# 0xa4: Resume to RAM content display */
    DISPENTIRE = 0xa5,      //# 0xa5: Entire display
    BIAS_1_9 = 0xa2,        //# 0xa2: Select BIAS setting 1/9 */
    BIAS_1_7 = 0xa3,      //  # 0xa3: Select BIAS setting 
    ENTER_RMWMODE = 0xe0,   //# 0xe0: Enter the Read Modify Write mode */
    EXIT_RMWMODE = 0xee,    //# 0xee: Leave the Read Modify Write mode */
    EXIT_SOFTRST = 0xe2,  //  # 0xe2: Software RE
    SETCOMNORMAL = 0xc0,    //# 0xc0: Set COM output direction, normal mode */
    SETCOMREVERSE = 0xc8, //  # 0xc8: Set COM output direction, reverse m
    POWERCTRL_VF = 0x29,    //# 0x29: Control built-in power circuit */
    POWERCTRL_VR = 0x2a,    //# 0x2a: Control built-in power circuit */
    POWERCTRL_VB = 0x2c,    //# 0x2c: Control built-in power circuit */
    POWERCTRL = 0x2f,     //  # 0x2c: Control built-in power circ
    REG_RES_RR0 = 0x21,     //# 0x21: Regulation Resistior ratio */
    REG_RES_RR1 = 0x22,     //# 0x22: Regulation Resistior ratio */
    REG_RES_RR2 = 0x24,   //  # 0x24: Regulation Resistior ra
    SETCONTRAST = 0x81,   //  # 0x81: Set contrast cont
    SETBOOSTER = 0xf8,      //# Set booster level */
    SETBOOSTER4X = 0x00,    //# Set booster level */
    SETBOOSTER5X = 0x01 , //  # Set booster le
    NOP = 0xe3,            //# 0xe3: NOP Command for no operation */
    STARTBYTES = 0,
  };

  void ST7567::reset() {
    if(reset_pin == PIN_UNUSED) return;
    gpio_put(reset_pin, 0); sleep_ms(10);
    gpio_put(reset_pin, 1); sleep_ms(10);
    sleep_ms(100);
  }


  void ST7567::init(bool auto_init_sequence) {
    spi_init(spi, spi_baud);

    gpio_set_function(reset_pin, GPIO_FUNC_SIO);
    gpio_set_dir(reset_pin, GPIO_OUT);

    gpio_set_function(dc, GPIO_FUNC_SIO);
    gpio_set_dir(dc, GPIO_OUT);

    gpio_set_function(cs, GPIO_FUNC_SIO);
    gpio_set_dir(cs, GPIO_OUT);

    gpio_set_function(sck, GPIO_FUNC_SPI);
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

    //reset display

    reset();



    // if auto_init_sequence then send initialisation sequence
    // for our standard displays based on the width and height
    if(auto_init_sequence) {

      command(reg::BIAS_1_7);
      command(reg::SEG_DIR_NORMAL);
      command(reg::SETCOMREVERSE);
      command(reg::DISPNORMAL);
      command(reg::SETSTARTLINE | 0x00); //Startline from 0x40-0x7F
      command(reg::POWERCTRL);
      command(reg::REG_RATIO | 4);
      command(reg::DISPON);
      command(reg::SETCONTRAST);
      command(30); // defalut contrast level
    }

    if(bl != PIN_UNUSED) {
      set_backlight(255); // Turn backlight on now surprises have passed
    }
  }

  void ST7567::command(uint8_t command, size_t len, const char *data) {
    gpio_put(cs, 0);

    gpio_put(dc, 0); // command mode
    spi_write_blocking(spi, &command, 1);
    gpio_put(cs, 1);

    sleep_us(100);
    if(data) {
      gpio_put(cs, 0);
      gpio_put(dc, 1); // data mode
      spi_write_blocking(spi, (const uint8_t*)data, len);
      gpio_put(cs, 1);
    }

    
  }

  // Native 16-bit framebuffer update
  void ST7567::update(PicoGraphics *graphics) {
    
    uint8_t *fb = (uint8_t *)graphics->frame_buffer;
    uint8_t *page_p = (uint8_t *)graphics->frame_buffer;
    uint8_t *byte_p = page_p;
    uint8_t page_buffer[128];
    for (int i=0 ; i < 128 ; i++){
      page_buffer[i] = i;
    }
    uint8_t col_byte = 0;
    /*
    1111111111
    0000000000
    1111111111
    0000000000
    to
    1010101010
    1010101010
    1010101010
    1010101010
    */


    //0b00000011 = 0b000000001 0b00000001
    
  uint8_t row_offset=0
  
  for (uint8_t fb_byte_index=0; fb_byte_index < 16; fb_byte_index ++ ){

    uint8_t new_loc = fb_byte_index * 8

    




  }

  
  
    
    

    if(graphics->pen_type == PicoGraphics::PEN_1BIT) {
      command(reg::ENTER_RMWMODE);
      for (uint8_t page=0; page < 8 ; page++){
        command(reg::SETPAGESTART | page);
        command(reg::SETCOLL);
        command(reg::SETCOLH);
        gpio_put(dc, 1); // data mode
        gpio_put(cs, 0);
        spi_write_blocking(spi, &page_buffer[0], PAGESIZE );
        fb += (PAGESIZE);
        gpio_put(cs, 1);
        gpio_put(dc, 0); // Back to command mode
        
      }

    } 
    
    /*else {
      command(reg::ENTER_RMWMODE);
      gpio_put(dc, 1); // data mode
      gpio_put(cs, 0);

      graphics->frame_convert(PicoGraphics::PEN_1BIT, [this](void *data, size_t length) {
        if (length > 0) {
          for (uint8_t page=0; page < 8 ; page++){
            
            command(reg::SETPAGESTART | page);
            command(reg::SETCOLL);
            command(reg::SETCOLH);
            gpio_put(dc, 1); // data mode
            gpio_put(cs, 0);
            spi_write_blocking(spi, fb, PAGESIZE / 8);
            fb += PAGESIZE / 8;
            gpio_put(cs, 1);
        }
      };
*/
      gpio_put(cs, 1);
    
  }

  void ST7567::set_backlight(uint8_t brightness) {
    // gamma correct the provided 0-255 brightness value onto a
    // 0-65535 range for the pwm counter
    float gamma = 2.8;
    uint16_t value = (uint16_t)(pow((float)(brightness) / 255.0f, gamma) * 65535.0f + 0.5f);
    pwm_set_gpio_level(bl, value);
  }
} 
