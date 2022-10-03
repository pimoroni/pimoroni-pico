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
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_i2c.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

// The 7-bit I2C ADDRESS of the RV3032
#define RV3032_ADDR         0x51


// REGISTERS
// Clock registers
#define RV3032_SECONDS      0x01
#define RV3032_MINUTES      0x02
#define RV3032_HOURS        0x03
// Calendar registers
#define RV3032_WEEKDAY      0x04
#define RV3032_DATE         0x05
#define RV3032_MONTHS       0x06
#define RV3032_YEARS        0x07

// Alarm registers
#define RV3032_MINUTES_ALM  0x08
#define RV3032_HOURS_ALM    0x09
#define RV3032_DATE_ALM     0x0A

// Periodic Countdown Timer registers
#define RV3032_TIMERVAL_0   0x0B
#define RV3032_TIMERVAL_1   0x0C
#define RV3032_STATUS       0x0D

//Temperature Monitoring registers 
#define RV3032_TEMP_LSB     0x0E
#define RV3032_TEMP_MSB     0x0F

// Extra Contol registers in TEMP_LSB
#define RV3032_TEMP_EEF     3
#define RV3032_TEMP_EEBUSY  2
#define RV3032_TEMP_CLKF    1
#define RV3032_TEMP_BSF     0

// Configuration registers
#define RV3032_CTRL1        0x10
#define RV3032_CTRL2        0x11
#define RV3032_CTRL3        0x12
#define RV3032_TIMESTAMP_C  0x13
#define RV3032_INT_MASK     0x14

// Eventcontrol/Timestamp registers
#define RV3032_EVENTCTRL    0x15
#define RV3032_TLOW_THRES   0x16
#define RV3032_THIGH_THRES  0x17
#define RV3032_COUNT_TSL    0x18
#define RV3032_SECONDS_TSL  0x19
#define RV3032_MINUTES_TSL  0x1A
#define RV3032_HOURS_TSL    0x1B
#define RV3032_DATE_TSL     0x1C
#define RV3032_MONTH_TSL    0x1D
#define RV3032_YEAR_TSL     0x1E
#define RV3032_COUNT_TSH    0x1F
#define RV3032_SECONDS_TSH  0x20
#define RV3032_MINUTES_TSH  0x21
#define RV3032_HOURS_TSH    0x22
#define RV3032_DATE_TSH     0x23
#define RV3032_MONTH_TSH    0x24
#define RV3032_YEAR_TSH     0x25
//#define RV3032_YEAR_TSH     0x26
#define RV3032_COUNT_TSEI   0x27
#define RV3032_SECONDS_TSEI 0x28
#define RV3032_MINUTES_TSEI 0x29
#define RV3032_HOURS_TSEI   0x2A
#define RV3032_DATE_TSEI    0x2B
#define RV3032_MONTH_TSEI   0x2C
#define RV3032_YEAR_TSEI    0x2D

// Password registers
#define RV3032_PASSWORD0    0x39
#define RV3032_PASSWORD1    0x3A
#define RV3032_PASSWORD2    0x3B
#define RV3032_PASSWORD3    0x3C

// EEPROM Memory Control registers
#define RV3032_EEPROM_ADDR  0x3D
#define RV3032_EEPROM_DATA  0x3E
#define RV3032_EEPROM_CMD   0x3F

// RAM registers
#define RV3032_USER_RAM_BEG 0x40
#define RV3032_USER_RAM_END 0x4F

// ID register
#define RV3032_ID           0x28
#define RV3032_CHIP_ID      0x30
#define RV3032_VERSION      0x03

// EEPROM Registers
#define RV3032_EEPROM_Clkout_Register   0xC2
#define RV3032_EEOffset_8_1             0xC1 // bits 8 to 1 of EEOffset. Bit 0 is bit 7 of register 0x37 
#define RV3032_EEPROM_Backup_Register   0xC0


// BITS IN IMPORTANT REGISTERS

// Bits in Status Register
#define RV3032_STATUS_THF     7
#define RV3032_STATUS_TLF     6
#define RV3032_STATUS_UF      5
#define RV3032_STATUS_TF      4
#define RV3032_STATUS_AF      3
#define RV3032_STATUS_EVF     2
#define RV3032_STATUS_PORF    1
#define RV3032_STATUS_VLF     0

// Bits in Control1 Register
//#define CTRL1_TRPT        7
//#define CTRL1_WADA        5 // Bit 6 not implemented
#define RV3032_CTRL1_USEL        4
#define RV3032_CTRL1_TE          3
#define RV3032_CTRL1_EERD        2
#define RV3032_CTRL1_TD1         1
#define RV3032_CTRL1_TD0         0

// Bits in Control2 Register
//#define CTRL2_TSE         7
#define RV3032_CTRL2_CLKIE       6
#define RV3032_CTRL2_UIE         5
#define RV3032_CTRL2_TIE         4
#define RV3032_CTRL2_AIE         3
#define RV3032_CTRL2_EIE         2
//#define CTRL2_12_24       1
#define RV3032_CTRL2_RESET       0

// Bits in Hours register
#define RV3032_HOURS_AM_PM       5

// Bits in Alarm registers
#define RV3032_MINUTESALM_AE_M   7
#define RV3032_HOURSALM_AE_H     7
#define RV3032_DATE_AE_WD        7

// Commands for EEPROM Command Register (0x27)
#define RV3032_EEPROMCMD_First          0x00
#define RV3032_EEPROMCMD_Update         0x11
#define RV3032_EEPROMCMD_Refresh        0x12
#define RV3032_EEPROMCMD_WriteSingle    0x21
#define RV3032_EEPROMCMD_ReadSingle     0x22

// Bits in EEPROM Backup Register
#define RV3032_EEPROMBackup_TCE_BIT     5           // Trickle Charge Enable Bit
#define RV3032_EEPROMBackup_FEDE_BIT    4           // Fast Edge Detection Enable Bit (for Backup Switchover Mode)
#define RV3032_EEPROMBackup_BSM_SHIFT   2           // Backup Switchover Mode shift
#define RV3032_EEPROMBackup_TCR_SHIFT   0           // Trickle Charge Resistor shift

#define RV3032_EEPROMBackup_BSM_CLEAR   0b11110011  // Backup Switchover Mode clear
#define RV3032_EEPROMBackup_TCR_CLEAR   0b11111100  // Trickle Charge Resistor clear
#define RV3032_TCR_3K                   0b00        // Trickle Charge Resistor 3kOhm
#define RV3032_TCR_5K                   0b01        // Trickle Charge Resistor 5kOhm
#define RV3032_TCR_9K                   0b10        // Trickle Charge Resistor 9kOhm
#define RV3032_TCR_15K                  0b11        // Trickle Charge Resistor 15kOhm


// Clock output register (0x35)
#define RV3032_EEPROMClkout_CLKOE_BIT   7           // 1 = CLKOUT pin is enabled. – Default value on delivery 
#define RV3032_EEPROMClkout_CLKSY_BIT   6
// Bits 5 and 4 not implemented
#define RV3032_EEPROMClkout_PORIE       3           // 0 = No interrupt, or canceled, signal on INT pin at POR. – Default value on delivery
                                             // 1 = An interrupt signal on INT pin at POR. Retained until the PORF flag is cleared to 0 (no automatic cancellation). 
#define RV3032_EEPROMClkout_FREQ_SHIFT  0           // frequency shift
#define RV3032_FD_CLKOUT_32k            0b000       // 32.768 kHz – Default value on delivery 
#define RV3032_FD_CLKOUT_8192           0b001       // 8192 Hz 
#define RV3032_FD_CLKOUT_1024           0b010       // 1024 Hz
#define RV3032_FD_CLKOUT_64             0b011       // 64 Hz 
#define RV3032_FD_CLKOUT_32             0b100       // 32 Hz
#define RV3032_FD_CLKOUT_1              0b101       // 1 Hz 
#define RV3032_FD_CLKOUT_TIMER          0b110       // Predefined periodic countdown timer interrupt 
#define RV3032_FD_CLKOUT_LOW            0b111       // CLKOUT = LOW 


#define RV3032_IMT_MASK_CEIE            3           // Clock output when Event Interrupt bit. 
#define RV3032_IMT_MASK_CAIE            2           // Clock output when Alarm Interrupt bit.
#define RV3032_IMT_MASK_CTIE            1           // Clock output when Periodic Countdown Timer Interrupt bit.
#define RV3032_IMT_MASK_CUIE            0           // Clock output when Periodic Time Update Interrupt bit.


#define TIME_ARRAY_LENGTH 7 // Total number of writable values in device


namespace pimoroni {
#ifndef TIME_ORDER
#define TIME_ORDER
  enum time_order {
    TIME_SECONDS,    // 0
    TIME_MINUTES,    // 1
    TIME_HOURS,      // 2
    TIME_WEEKDAY,    // 3
    TIME_DATE,       // 4
    TIME_MONTH,      // 5
    TIME_YEAR,       // 6
  };
#endif
  class RV3032 {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_I2C_ADDRESS  = RV3032_ADDR;
    static const uint8_t DEFAULT_INT_PIN      = 22;
    static const uint8_t PIN_UNUSED           = UINT8_MAX;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    I2C *i2c;

    // interface pins with our standard defaults where appropriate
    const int8_t address  = DEFAULT_I2C_ADDRESS;
    uint interrupt  = DEFAULT_INT_PIN;

    uint8_t times[TIME_ARRAY_LENGTH];


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    RV3032() : RV3032(new I2C()) {}

    RV3032(I2C *i2c, uint interrupt = DEFAULT_INT_PIN) :
      i2c(i2c), interrupt(interrupt) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();
    void reset();

    i2c_inst_t* get_i2c() const;
    int get_sda() const;
    int get_scl() const;
    int get_int() const;

    bool setup(bool set_24Hour = true, bool disable_TrickleCharge = true, bool set_LevelSwitchingMode = true);
    bool set_time(uint8_t sec, uint8_t min, uint8_t hour, uint8_t weekday, uint8_t date, uint8_t month, uint16_t year);
    bool set_time(uint8_t *time, uint8_t len);
    bool set_seconds(uint8_t value);
    bool set_minutes(uint8_t value);
    bool set_hours(uint8_t value);
    bool set_weekday(uint8_t value);
    bool set_date(uint8_t value);
    bool set_month(uint8_t value);
    bool set_year(uint16_t value);
    bool set_to_compiler_time();  // Uses the hours, mins, etc from compile time to set RTC

    bool update_time();           // Update the local array with the RTC registers

    char* string_date_usa();      // Return date in mm-dd-yyyy
    char* string_date();          // Return date in dd-mm-yyyy
    char* string_time();          // Return time hh:mm:ss with AM/PM if in 12 hour mode
    char* string_time_stamp();    // Return timeStamp in ISO 8601 format yyyy-mm-ddThh:mm:ss

    uint8_t get_seconds();
    uint8_t get_minutes();
    uint8_t get_hours();
    uint8_t get_weekday();
    uint8_t get_date();
    uint8_t get_month();
    uint16_t get_year();

    bool is_12_hour();            // Returns true if 12hour bit is set
    bool is_pm();                 // Returns true if is12Hour and PM bit is set
    void set_12_hour();
    void set_24_hour();

    bool set_unix(uint32_t value); // Set the UNIX Time (Real Time and UNIX Time are INDEPENDENT!)
    uint32_t get_unix();

    void enable_alarm_interrupt(uint8_t min, uint8_t hour, uint8_t date_or_weekday, bool set_weekday_alarm_not_date, uint8_t mode, bool enable_clock_output = false);
    void enable_alarm_interrupt();
    void disable_alarm_interrupt();
    bool read_alarm_interrupt_flag();
    void clear_alarm_interrupt_flag();

    void set_timer(bool timer_repeat, uint16_t timer_frequency, uint16_t timer_value, bool set_interrupt, bool start_timer, bool enable_clock_output = false);
    uint16_t get_timer_count();
    void enable_timer();
    void disable_timer();
    void enable_timer_interrupt();
    void disable_timer_interrupt();
    bool read_timer_interrupt_flag();
    void clear_timer_interrupt_flag();

    void enable_periodic_update_interrupt(bool every_second, bool enable_clock_output = false);
    void disable_periodic_update_interrupt();
    bool read_periodic_update_interrupt_flag();
    void clear_periodic_update_interrupt_flag();

    void enable_trickle_charge(uint8_t tcr = RV3032_TCR_15K); // Trickle Charge Resistor default 15k
    void disable_trickle_charge();
    bool set_backup_switchover_mode(uint8_t val);

    void enable_clock_out(uint8_t freq);
    void enable_interrupt_controlled_clockout(uint8_t freq);
    void disable_clock_out();
    bool read_clock_output_interrupt_flag();
    void clear_clock_output_interrupt_flag();

    uint8_t status(); // Returns the status byte
    void clear_interrupts();

private:
    // Values in RTC are stored in Binary Coded Decimal. These functions convert to/from Decimal
    uint8_t bcd_to_dec(uint8_t val);
    uint8_t dec_to_bcd(uint8_t val);

    uint8_t read_register(uint8_t addr);
    bool write_register(uint8_t addr, uint8_t val);
    bool read_multiple_registers(uint8_t addr, uint8_t *dest, uint8_t len);
    bool write_multiple_registers(uint8_t addr, uint8_t *values, uint8_t len);

    bool write_config_eeprom_ram_mirror(uint8_t eepromaddr, uint8_t val);
    uint8_t read_config_eeprom_ram_mirror(uint8_t eepromaddr);
    bool wait_for_eeprom();

    void set_bit(uint8_t reg_addr, uint8_t bit_num);
    void clear_bit(uint8_t reg_addr, uint8_t bit_num);
    bool read_bit(uint8_t reg_addr, uint8_t bit_num);
  };

  // POSSIBLE ENHANCEMENTS :
  // ENHANCEMENT: Battery Interrupt / check battery voltage
  // ENHANCEMENT: External Event Interrupt

}
