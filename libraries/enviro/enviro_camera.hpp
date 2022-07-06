#pragma once
#include <cstring>
#include "enviro.hpp"
#include "hardware/pio.h"
#include "hardware/dma.h"

using namespace enviro;


namespace enviro {

  class EnviroCamera : public EnviroBase {
    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum Pins {
      XCLK = 0,
      PCLK = 1,
      SCCB_DATA = 4,
      SCCB_CLOCK = 5,
      DATA1 = 9,
      DATA2 = 10,
      DATA3 = 11,
      DATA4 = 12,
      DATA5 = 13,
      DATA6 = 14,
      DATA7 = 15,
      DATA8 = 16,
      DATA9 = 17,
      HSYNC = 18,
      VSYNC = 19,
      QSPI_CHIP_SELECT = 20,
      QSPI_SCLK = 21,
      QSPI_DATA0 = 22,
      QSPI_DATA1 = 26,
      QSPI_DATA2 = 27,
      QSPI_DATA3 = 28
    };
    static const uint EXTERNAL_INTERRUPT_PIN = 3;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    PIO pio = pio0;
    uint pio_sm = 0;

    int width = 352;
    int height = 288;

    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    EnviroCamera();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    bool capture(uint8_t *buffer);

  protected:
    void reg_write(uint8_t reg, uint8_t value);
    uint8_t reg_read(uint8_t reg);
    void regs_write(const uint8_t (*regs)[2]);
  };

}
