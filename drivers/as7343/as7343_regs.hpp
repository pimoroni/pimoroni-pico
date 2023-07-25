#pragma once
#include <stdint.h>

namespace pimoroni {
  /***** Device registers and masks here *****/

  enum reg {
    // Bank 1
    AUXID = 0x58, // AUXID    = 0b00001111
    REVID = 0x59, // REVID    = 0b00000111
    ID = 0x5A,    // ID       = 0b11111111
    CFG12 = 0x66, // SP_TH_CH = 0b00000111

    // Bank 0
    ENABLE = 0x80, // FDEN    = 0b01000000
                   // SMUXEN  = 0b00010000
                   // WEN     = 0b00001000
                   // SP_EN   = 0b00000010
                   // PON     = 0b00000001
    ATIME = 0x81,  // ATIME   = 0b11111111
    // Integration Time Step Size
    // 0 = 2.87us
    // n 2.87us x (n + 1)
    ASTEP = 0xD4,  // STEP    = 0xFFFF
    // Spectral Measurement Wait Time
    // 0 = 1 cycle = 2.78ms
    // n = 2.78ms x (n + 1)
    WTIME = 0x83,  // WTIME   = 0b11111111
    SP_TH = 0x84,  // SP_TH_L = 0xFFFF0000       Spectral Low Threshold
                   // SP_TH_H = 0x0000FFFF       Spectral High Threshold
    STATUS = 0x93, // ASAT    = 0b10000000       Spectral Saturation (if ASIEN set)
                   // AINT    = 0b00001000       Spectral Channel Interrupt (if SP_IEN set)
                   // FINT    = 0b00000100       FIFO Buffer Interrupt
                   // SINT    = 0b00000001       System Interrupt
    ASTATUS = 0x94,// ASAT    = 0b10000000       Saturation Status
                   // AGAIN   = 0b00000111       Gain Status
    DATA = 0x95,   // 36 bytes, fields 0 to 17 (2 bytes eac)
    STATUS2 = 0x90,// AVALID   = 0b01000000      Spectral Data Valid
                   // ASAT_DIG = 0b00010000      Digital Saturation
                   // ASAT_ANA = 0b00001000      Analog Saturation
                   // FDSAT_ANA= 0b00000010      Flicker Analog Saturation
                   // FDSAT_DIG= 0b00000001      Flicker Digital Saturation
    STATUS3 = 0x91,// INT_SP_H = 0b00100000      Spectral Above High Threshold
                   // INT_SP_L = 0b00010000      Spectral Below Low Threshold
    STATUS5 = 0xBB,// SINT_FD  = 0b00001000      Flicker Detect Interrupt (if SIEN_FD set)
                   // SINT_SMUX= 0b00000100      SMUS Operation Interrupt (SMUX exec finished)
    STATUS4 = 0xBC,// FIFO_OV  = 0b10000000      FIFO Buffer Overflow
                   // OVTEMP   = 0b00100000      Over Temperature
                   // FD_TRIG  = 0b00010000      Flicker Detect Trigger Error
                   // SD_TRIG  = 0b00000100      Spectral Trigger Error
                   // SAI_ACT  = 0b00000010      Sleep After Interrupt Active
                   // INT_BUSY = 0b00000001      Initialization busy (1 for ~300us after power on)
    CFG0 = 0xBF,   // LOW_POWER= 0b00100000
                   // REG_BANK = 0b00010000  0 - Register 0x80 and above
                   //                        1 - Register 0x20 to 0x7f
                   // WLONG    = 0b00000100      Increase WTIME by factor of 16
    // Spectral Engines Gain Setting
    // 0 = 0.5x, # 1 = 1x, 2 = 2x, 12 = 2048x
    // GAINx = 1 << (n - 1)
    CFG1 = 0xC6,
    CFG3 = 0xC7,   // SAI      = 0b00100000      Sleep after interrupt
    CFG6 = 0xF5,   // SMUS_CMD = 0b00011000  0 - ROM_init
                   //                        1 - Read_SMUX
                   //                        2 - Write_SMUX
    CFG8 = 0xC9,   // FIFO_TH  = 0b11000000  0b00, 0b01, 0b10, 0b11
    CFG9 = 0xCA,   // SIEN_FD  = 0b01000000      System Interrupt Flicker Detection
                   // SIEN_SMUX= 0b00010000      System Interrupt SMUX Operation
    CFG10 = 0x65,  // FD_PERS  = 0b00000111      Flicker Detect Persistence
                   //                            Number of results that must differ before status change
    PERS = 0xCF,   // APERS    = 0b00001111
    GPIO = 0x6B,   // GPIO_INV = 0b00001000      Invert GPIO output
                   // GPIO_IN_EN=0b00000100      Enable GPIO input
                   // GPIO_OUT = 0b00000010      GPIO Output
                   // GPIO_IN  = 0b00000001      GPIO Input
    CFG20 = 0xD6,  // FD_FIFO_8b=0b10000000      Enable 8bit FIFO mode for Flicker Detect (FD_TIME < 256)
                   // auto_SMUX= 0b01100000      Auto channel read-out
    LED = 0xCD,    // LED_ACT  = 0b10000000      External LED (LDR) Control
                   // LED_DRIVE= 0b01111111      External LED drive strength (N - 4) >> 1
    // Flicker Detection AGC Gain Max
    // Max = 2^N (0 = 0.5x)
    AGC_GAIN_MAX = 0xD7, // ADC_FD_GAIN_MAX = 0b11110000
    AZ_CONFIG = 0xDE, // AT_NTH_ITERATION = 0b11111111 Auto-zero Frequency
                      //                     0 - Never (Not Recommended)
                      //                     n - Every n integration cycles
                      //                   255 - only before first measurement cycle
    FD_TIME_1 = 0xE0, // FD_TIME=0b11111111      Flicker Detection Integration Time (Do not change if FDEN = 1 & PON = 1)
    FD_TIME_2 = 0xE2, // FD_GAIN=0b11111000      Flicker Detection Gain (0 = 0.5x, 1 = 1x, 2 = 2x, 12 = 2048x)
                      // FD_TIME=0b00000111      Flicker Detection Time (Do not change if FDEN = 1 & PON = 1)
    FD_CFG0 = 0xDF,   // FIFO_WRITE_FD = 0b10000000 Write flicker raw data to FIFO (1 byte per sample)
    FD_STATUS = 0xE3, // FD_VALID=0b00100000
                      // FD_SAT = 0b00010000
                      // FD_120HZ_VALID = 0b00001000
                      // FD_100HZ_VALID = 0b00000100
                      // FD_120HZ = 0b00000010
                      // FD_100HZ = 0b00000001
    INTERNAB = 0xF9,  // ASIEN  = 0b10000000     Saturation Interrupt Enable
                      // SP_IEN = 0b00001000     Spectral Interrupt Enable
                      // FIEN   = 0b00000100     FIFO Buffer Interrupt Enable
                      // SIEN   = 0b00000001     System Interrupt Enable
    CONTROL = 0xFA,   // SW_RESET = 0b00001000   Software Reset
                      // SP_MAN_AZ= 0b00000100   Spectral Manual Auto-zero
                      // FIFO_CLR = 0b00000010   FIFO Buffer Clear
                      // CLEAR_SAI_ACT = 0b00000001 Clear sleep-after-interrupt
    FIFO_MAP = 0xFC,  // CH5     = 0b01000000
                      // CH4     = 0b00100000
                      // CH3     = 0b00010000
                      // CH2     = 0b00001000
                      // CH1     = 0b00000100
                      // CH0     = 0b00000010
                      // ASTATUS = 0b00000001
    FIFO_LVL = 0xFD,
    FDATA = 0xFE      // 0xFFFF
  };

  constexpr uint8_t CFG0_LOW_POWER     = 0b00100000;
  constexpr uint8_t CFG0_BANK          = 0b00010000;
  constexpr uint8_t CFG0_WLONG         = 0b00000100;

  constexpr uint8_t CFG20_6_CH        = 0b00000000;
  constexpr uint8_t CFG20_12_CH       = 0b01000000;
  constexpr uint8_t CFG20_18_CH       = 0b01100000;

  constexpr uint8_t ENABLE_FDEN        = 0b01000000;
  constexpr uint8_t ENABLE_SMUXEN      = 0b00010000;
  constexpr uint8_t ENABLE_WEN         = 0b00001000;
  constexpr uint8_t ENABLE_SP_EN       = 0b00000010;
  constexpr uint8_t ENABLE_PON         = 0b00000001;

  constexpr uint8_t FD_CFG0_FIFO_WRITE = 0b10000000;

  constexpr uint8_t FD_VALID           = 0b00100000;
  constexpr uint8_t FD_SAT             = 0b00010000;
  constexpr uint8_t FD_120HZ_VALID     = 0b00001000;
  constexpr uint8_t FD_100HZ_VALID     = 0b00000100;
  constexpr uint8_t FD_120HZ           = 0b00000010;
  constexpr uint8_t FD_100HZ           = 0b00000001;

  constexpr uint8_t INTERNAB_ASIEN     = 0b10000000;
  constexpr uint8_t INTERNAB_SP_IEN    = 0b00001000;
  constexpr uint8_t INTERNAB_FIEN      = 0b00000100;
  constexpr uint8_t INTERNAB_SIEN      = 0b00000001;

  constexpr uint8_t CONTROL_SW_RESET   = 0b00001000;
  constexpr uint8_t CONTROL_SP_MAN_AZ  = 0b00000100;
  constexpr uint8_t CONTROL_FIFO_CLR   = 0b00000010;
  constexpr uint8_t CONTROL_CLEAR_SAI_ACT = 0b00000001;

  constexpr uint8_t FIFO_MAP_CH5       = 0b01000000;
  constexpr uint8_t FIFO_MAP_CH4       = 0b00100000;
  constexpr uint8_t FIFO_MAP_CH3       = 0b00010000;
  constexpr uint8_t FIFO_MAP_CH2       = 0b00001000;
  constexpr uint8_t FIFO_MAP_CH1       = 0b00000100;
  constexpr uint8_t FIFO_MAP_CH0       = 0b00000010;
  constexpr uint8_t FIFO_MAP_ASTATUS   = 0b00000001;
}