/******************************************************************************
RV-3028-C7.hpp
bBased on the RV-3028-C7 Arduino Library by Constantin Koch, July 25, 2019
https://github.com/constiko/RV-3028_C7-Arduino_Library

This code is released under the [MIT License](http://opensource.org/licenses/MIT).
Please review the LICENSE file included with this example.
Distributed as-is; no warranty is given.
******************************************************************************/

#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

//The 7-bit I2C ADDRESS of the RV3028
#define RV3028_ADDR            0x52


//REGISTERS
//Clock registers
#define RV3028_SECONDS      0x00
#define RV3028_MINUTES      0x01
#define RV3028_HOURS        0x02
//Calendar registers
#define RV3028_WEEKDAY            0x03
#define RV3028_DATE         0x04
#define RV3028_MONTHS       0x05
#define RV3028_YEARS        0x06

//Alarm registers
#define RV3028_MINUTES_ALM  0x07
#define RV3028_HOURS_ALM    0x08
#define RV3028_DATE_ALM     0x09

//Periodic Countdown Timer registers
#define RV3028_TIMERVAL_0        0x0A
#define RV3028_TIMERVAL_1        0x0B
#define RV3028_TIMERSTAT_0        0x0C
#define RV3028_TIMERSTAT_1        0x0D

//Configuration registers
#define RV3028_STATUS        0x0E
#define RV3028_CTRL1        0x0F
#define RV3028_CTRL2        0x10
#define RV3028_GPBITS        0x11
#define RV3028_INT_MASK        0x12

//Eventcontrol/Timestamp registers
#define RV3028_EVENTCTRL        0x13
#define RV3028_COUNT_TS                0x14
#define RV3028_SECONDS_TS        0x15
#define RV3028_MINUTES_TS        0x16
#define RV3028_HOURS_TS                0x17
#define RV3028_DATE_TS                0x18
#define RV3028_MONTH_TS                0x19
#define RV3028_YEAR_TS                0x1A

//Unix Time registers
#define RV3028_UNIX_TIME0        0x1B
#define RV3028_UNIX_TIME1        0x1C
#define RV3028_UNIX_TIME2        0x1D
#define RV3028_UNIX_TIME3        0x1E

//RAM registers
#define RV3028_USER_RAM1        0x1F
#define RV3028_USER_RAM2        0x20

//Password registers
#define RV3028_PASSWORD0        0x21
#define RV3028_PASSWORD1        0x22
#define RV3028_PASSWORD2        0x23
#define RV3028_PASSWORD3        0x24

//EEPROM Memory Control registers
#define RV3028_EEPROM_ADDR        0x25
#define RV3028_EEPROM_DATA        0x26
#define RV3028_EEPROM_CMD        0x27

//ID register
#define RV3028_ID                0x28

//EEPROM Registers
#define EEPROM_Clkout_Register        0x35
#define RV3028_EEOffset_8_1        0x36        //bits 8 to 1 of EEOffset. Bit 0 is bit 7 of register 0x37 
#define EEPROM_Backup_Register        0x37


//BITS IN IMPORTANT REGISTERS

//Bits in Status Register
#define STATUS_EEBUSY        7
#define STATUS_CLKF        6
#define STATUS_BSF        5
#define STATUS_UF        4
#define STATUS_TF        3
#define STATUS_AF        2
#define STATUS_EVF        1
#define STATUS_PORF        0

//Bits in Control1 Register
#define CTRL1_TRPT        7
#define CTRL1_WADA        5//Bit 6 not implemented
#define CTRL1_USEL        4
#define CTRL1_EERD        3
#define CTRL1_TE        2
#define        CTRL1_TD1        1
#define CTRL1_TD0        0

//Bits in Control2 Register
#define CTRL2_TSE        7
#define CTRL2_CLKIE        6
#define CTRL2_UIE        5
#define CTRL2_TIE        4
#define CTRL2_AIE        3
#define CTRL2_EIE        2
#define CTRL2_12_24        1
#define CTRL2_RESET        0

//Bits in Hours register
#define HOURS_AM_PM        5

//Bits in Alarm registers
#define MINUTESALM_AE_M        7
#define HOURSALM_AE_H        7
#define DATE_AE_WD        7

//Commands for EEPROM Command Register (0x27)
#define EEPROMCMD_First                0x00
#define EEPROMCMD_Update        0x11
#define EEPROMCMD_Refresh        0x12
#define EEPROMCMD_WriteSingle        0x21
#define EEPROMCMD_ReadSingle        0x22

//Bits in EEPROM Backup Register
#define EEPROMBackup_TCE_BIT        5        //Trickle Charge Enable Bit
#define EEPROMBackup_FEDE_BIT        4        //Fast Edge Detection Enable Bit (for Backup Switchover Mode)
#define EEPROMBackup_BSM_SHIFT        2        //Backup Switchover Mode shift
#define EEPROMBackup_TCR_SHIFT        0        //Trickle Charge Resistor shift

#define EEPROMBackup_BSM_CLEAR        0b11110011        //Backup Switchover Mode clear
#define EEPROMBackup_TCR_CLEAR        0b11111100        //Trickle Charge Resistor clear
#define        TCR_3K                        0b00                //Trickle Charge Resistor 3kOhm
#define        TCR_5K                        0b01                //Trickle Charge Resistor 5kOhm
#define        TCR_9K                        0b10                //Trickle Charge Resistor 9kOhm
#define        TCR_15K                        0b11                //Trickle Charge Resistor 15kOhm


// Clock output register (0x35)
#define EEPROMClkout_CLKOE_BIT        7        //1 = CLKOUT pin is enabled. – Default value on delivery 
#define EEPROMClkout_CLKSY_BIT        6
// Bits 5 and 4 not implemented
#define EEPROMClkout_PORIE          3        //0 = No interrupt, or canceled, signal on INT pin at POR. – Default value on delivery
                        //1 = An interrupt signal on INT pin at POR. Retained until the PORF flag is cleared to 0 (no automatic cancellation). 
#define EEPROMClkout_FREQ_SHIFT        0        //frequency shift
#define FD_CLKOUT_32k        0b000          //32.768 kHz – Default value on delivery 
#define FD_CLKOUT_8192        0b001           //8192 Hz 
#define FD_CLKOUT_1024        0b010          //1024 Hz
#define FD_CLKOUT_64        0b011           //64 Hz 
#define FD_CLKOUT_32        0b100          //32 Hz
#define FD_CLKOUT_1        0b101           //1 Hz 
#define FD_CLKOUT_TIMER        0b110          //Predefined periodic countdown timer interrupt 
#define FD_CLKOUT_LOW        0b111          //CLKOUT = LOW 


#define IMT_MASK_CEIE        3        //Clock output when Event Interrupt bit. 
#define IMT_MASK_CAIE        2        //Clock output when Alarm Interrupt bit.
#define IMT_MASK_CTIE        1        //Clock output when Periodic Countdown Timer Interrupt bit.
#define IMT_MASK_CUIE        0        //Clock output when Periodic Time Update Interrupt bit.


#define TIME_ARRAY_LENGTH 7 // Total number of writable values in device

namespace pimoroni {

  enum time_order {
    TIME_SECONDS,    // 0
    TIME_MINUTES,    // 1
    TIME_HOURS,      // 2
    TIME_WEEKDAY,    // 3
    TIME_DATE,       // 4
    TIME_MONTH,      // 5
    TIME_YEAR,       // 6
  };

  class RV3028 {
    i2c_inst_t *i2c = i2c0;
    // interface pins with our standard defaults where appropriate
    int8_t sda       = 20;
    int8_t scl       = 21;
    int8_t interrupt = 22;
    public:
      RV3028() {}

      RV3028(i2c_inst_t *i2c, uint8_t sda, uint8_t scl, uint8_t interrupt) :
              i2c(i2c), sda(sda), scl(scl), interrupt(interrupt) {}

      void init();

      bool setup(bool set_24Hour = true, bool disable_TrickleCharge = true, bool set_LevelSwitchingMode = true);
      bool setTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t weekday, uint8_t date, uint8_t month, uint16_t year);
      bool setTime(uint8_t * time, uint8_t len);
      bool setSeconds(uint8_t value);
      bool setMinutes(uint8_t value);
      bool setHours(uint8_t value);
      bool setWeekday(uint8_t value);
      bool setDate(uint8_t value);
      bool setMonth(uint8_t value);
      bool setYear(uint16_t value);
      bool setToCompilerTime();        //Uses the hours, mins, etc from compile time to set RTC

      bool updateTime()        ;        //Update the local array with the RTC registers

      char* stringDateUSA();        //Return date in mm-dd-yyyy
      char* stringDate();                //Return date in dd-mm-yyyy
      char* stringTime();                //Return time hh:mm:ss with AM/PM if in 12 hour mode
      char* stringTimeStamp();        //Return timeStamp in ISO 8601 format yyyy-mm-ddThh:mm:ss

      uint8_t getSeconds();
      uint8_t getMinutes();
      uint8_t getHours();
      uint8_t getWeekday();
      uint8_t getDate();
      uint8_t getMonth();
      uint16_t getYear();


      bool is12Hour();        //Returns true if 12hour bit is set
      bool isPM();                //Returns true if is12Hour and PM bit is set
      void set12Hour();
      void set24Hour();

      bool setUNIX(uint32_t value);//Set the UNIX Time (Real Time and UNIX Time are INDEPENDENT!)
      uint32_t getUNIX();

      void enableAlarmInterrupt(uint8_t min, uint8_t hour, uint8_t date_or_weekday, bool setWeekdayAlarm_not_Date, uint8_t mode, bool enable_clock_output = false);
      void enableAlarmInterrupt();
      void disableAlarmInterrupt();
      bool readAlarmInterruptFlag();
      void clearAlarmInterruptFlag();

      void setTimer(bool timer_repeat, uint16_t timer_frequency, uint16_t timer_value, bool setInterrupt, bool start_timer, bool enable_clock_output = false);
      uint16_t getTimerCount(void);
      void enableTimer();
      void disableTimer();
      void enableTimerInterrupt();
      void disableTimerInterrupt();
      bool readTimerInterruptFlag();
      void clearTimerInterruptFlag();

      void enablePeriodicUpdateInterrupt(bool every_second, bool enable_clock_output = false);
      void disablePeriodicUpdateInterrupt();
      bool readPeriodicUpdateInterruptFlag();
      void clearPeriodicUpdateInterruptFlag();

      void enableTrickleCharge(uint8_t tcr = TCR_15K); //Trickle Charge Resistor default 15k
      void disableTrickleCharge();
      bool setBackupSwitchoverMode(uint8_t val);

      void enableClockOut(uint8_t freq);
      void enableInterruptControlledClockout(uint8_t freq);
      void disableClockOut();
      bool readClockOutputInterruptFlag();
      void clearClockOutputInterruptFlag();

      uint8_t status(); //Returns the status byte
      void clearInterrupts();

      //Values in RTC are stored in Binary Coded Decimal. These functions convert to/from Decimal
      uint8_t BCDtoDEC(uint8_t val);
      uint8_t DECtoBCD(uint8_t val);

      uint8_t readRegister(uint8_t addr);
      bool writeRegister(uint8_t addr, uint8_t val);
      bool readMultipleRegisters(uint8_t addr, uint8_t * dest, uint8_t len);
      bool writeMultipleRegisters(uint8_t addr, uint8_t * values, uint8_t len);

      bool writeConfigEEPROM_RAMmirror(uint8_t eepromaddr, uint8_t val);
      uint8_t readConfigEEPROM_RAMmirror(uint8_t eepromaddr);
      bool waitforEEPROM();
      void reset();

      void setBit(uint8_t reg_addr, uint8_t bit_num);
      void clearBit(uint8_t reg_addr, uint8_t bit_num);
      bool readBit(uint8_t reg_addr, uint8_t bit_num);
  private:
      uint8_t _time[TIME_ARRAY_LENGTH];
      i2c_inst_t *_i2cPort;
      int8_t address = RV3028_ADDR;

      // From i2cdevice
      int write_bytes(uint8_t reg, uint8_t *buf, int len);
      int read_bytes(uint8_t reg, uint8_t *buf, int len);
      uint8_t get_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
      void set_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
      void clear_bits(uint8_t reg, uint8_t shift, uint8_t mask=0b1);
  };

  //POSSIBLE ENHANCEMENTS :
  //ENHANCEMENT: Battery Interrupt / check battery voltage
  //ENHANCEMENT: External Event Interrupt

}
