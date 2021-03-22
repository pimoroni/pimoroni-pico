#pragma once

/**
 * This Library was originally written by Olivier Van den Eede (4ilo) in 2016.
 * Some refactoring was done and SPI support was added by Aleksander Alekseev (afiskon) in 2018.
 * Ported to Raspberry Pico to drive Pimoroni OLED breakouts by Simon Reap (simon3270) in 2021.
 *
 * https://github.com/afiskon/stm32-ssd1306
 * https://github.com/simon3270/pimoroni-pico (driver-sh1107 branch)
 */


#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include "sh1107_fonts.h"

namespace pimoroni {

  // Enumeration for screen colors
  enum class SH1107_COLOR : uint8_t {
      Black = 0, // Black color, no pixel
      White = 1  // Pixel is set. Color depends on OLED
  };

  typedef enum {
      SH1107_OK = 0x00,
      SH1107_ERR = 0x01  // Generic error.
  } SH1107_Error_t;

  // Struct to store transformations
  typedef struct {
      uint16_t CurrentX;
      uint16_t CurrentY;
      uint8_t Inverted;
      uint8_t Initialized;
      uint8_t DisplayOn;
  } SH1107_t;
  typedef struct {
      uint8_t x;
      uint8_t y;
  } SH1107_VERTEX;

  class SH1107 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS  = 0x3C;
    static const uint8_t DEFAULT_SDA_PIN      = 20;
    static const uint8_t DEFAULT_SCL_PIN      = 21;
    static const uint8_t DEFAULT_INT_PIN      = 22;
    static const uint8_t PIN_UNUSED           = UINT8_MAX;

  private:
    /***** Private constants here *****/


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    i2c_inst_t *i2c   = nullptr;
    uint32_t i2c_baud = 400000;

    // i2c interface pins with our standard defaults where appropriate
    int8_t address    = DEFAULT_I2C_ADDRESS;
    int8_t sda        = DEFAULT_SDA_PIN;
    int8_t scl        = DEFAULT_SCL_PIN;
    int8_t interrupt  = DEFAULT_INT_PIN;

    // spi interface pins
    spi_inst_t *spi   = nullptr;
    uint32_t spi_baud = 64 * 1024 * 1024;

    int8_t cs         = 17;
    int8_t dc         = 16;
    int8_t sck        = 18;
    int8_t mosi       = 19;
    int8_t miso       = -1; // we generally don't use this pin
    int8_t rst        = -1; // Not broken out on BG breakouts

    // Screen object
    SH1107_t SH1107_State;

    uint16_t width;
    uint16_t height;

    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    // frame buffer where pixel data is stored
    uint8_t *frame_buffer;

    SH1107(uint16_t width, uint16_t height, uint8_t *frame_buffer,
          i2c_inst_t *i2c,
          uint8_t address, uint8_t sda, uint8_t scl, uint8_t interrupt = PIN_UNUSED) :
      i2c(i2c), 
      width(width), height(height), 
      address(address), sda(sda), scl(scl), interrupt(interrupt),
      frame_buffer(frame_buffer) {}

    SH1107(uint16_t width, uint16_t height,
          i2c_inst_t *i2c,
          uint8_t address, uint8_t sda, uint8_t scl, uint8_t interrupt = PIN_UNUSED) :
      i2c(i2c), 
      width(width), height(height), 
      address(address), sda(sda), scl(scl), interrupt(interrupt) {
        frame_buffer = new uint8_t[width * height / 8];
      }

    SH1107(uint16_t width, uint16_t height, uint8_t *frame_buffer,
           spi_inst_t *spi,
           uint8_t cs, uint8_t dc, uint8_t sck, uint8_t mosi, uint8_t miso = -1) :
      spi(spi),
      width(width), height(height),      
      cs(cs), dc(dc), sck(sck), mosi(mosi), miso(miso),
      frame_buffer(frame_buffer) {}

    SH1107(uint16_t width, uint16_t height,
           spi_inst_t *spi,
           uint8_t cs, uint8_t dc, uint8_t sck, uint8_t mosi, uint8_t miso = -1) :
      spi(spi),
      width(width), height(height),      
      cs(cs), dc(dc), sck(sck), mosi(mosi), miso(miso) {
        frame_buffer = new uint8_t[width * height / 8];
      }

    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(); //This should be present in all drivers

    /***** More public methods here *****/
    // Procedure definitions
    void Screen_Init(void);
    void Fill(SH1107_COLOR color);
    void UpdateScreen(void);
    void DrawPixel(uint8_t x, uint8_t y, SH1107_COLOR color);
    char WriteChar(char ch, FontDef Font, SH1107_COLOR color);
    char WriteString(const char* str, FontDef Font, SH1107_COLOR color);
    void SetCursor(uint8_t x, uint8_t y);
    void Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SH1107_COLOR color);
    void DrawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SH1107_COLOR color);
    void DrawCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, SH1107_COLOR color);
    void Polyline(const SH1107_VERTEX *par_vertex, uint16_t par_size, SH1107_COLOR color);
    void DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SH1107_COLOR color);
    /**
     * @brief Sets the contrast of the display.
     * @param[in] value contrast to set.
     * @note Contrast increases as the value increases.
     * @note RESET = 7Fh.
     */
    void SetContrast(const uint8_t value);
    /**
     * @brief Set Display ON/OFF.
     * @param[in] on 0 for OFF, any for ON.
     */
    void SetDisplayOn(const uint8_t on);
    /**
     * @brief Reads DisplayOn state.
     * @return  0: OFF.
     *          1: ON.
     */
    uint8_t GetDisplayOn();

    // Low-level procedures
    void Reset(void);
    void WriteCommand(uint8_t byte);
    void WriteData(uint8_t* buffer, size_t buff_size);
    SH1107_Error_t FillBuffer(uint8_t* buf, uint32_t len);

  private:
    /***** Private methods here *****/
    float DegToRad(float par_deg);
    uint16_t NormalizeTo0_360(uint16_t par_deg);
  };

}
