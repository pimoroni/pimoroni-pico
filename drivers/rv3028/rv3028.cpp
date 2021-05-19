/******************************************************************************
rv3028.cpp
Based on RV-3028-C7 Arduino Library by Constantin Koch, July 25, 2019
https://github.com/constiko/RV-3028_C7-Arduino_Library

This code is released under the [MIT License](http://opensource.org/licenses/MIT).
Please review the LICENSE file included with this example.
Distributed as-is; no warranty is given.
******************************************************************************/

#include "rv3028.hpp"

//****************************************************************************//
//
//  Settings and configuration
//
//****************************************************************************//

// Parse the __DATE__ predefined macro to generate date defaults:
// __Date__ Format: MMM DD YYYY (First D may be a space if <10)
// <MONTH>                                  
#define BUILD_MONTH_JAN ((__DATE__[0] == 'J') && (__DATE__[1] == 'a')) ? 1 : 0
#define BUILD_MONTH_FEB (__DATE__[0] == 'F') ? 2 : 0
#define BUILD_MONTH_MAR ((__DATE__[0] == 'M') && (__DATE__[1] == 'a') && (__DATE__[2] == 'r')) ? 3 : 0
#define BUILD_MONTH_APR ((__DATE__[0] == 'A') && (__DATE__[1] == 'p')) ? 4 : 0
#define BUILD_MONTH_MAY ((__DATE__[0] == 'M') && (__DATE__[1] == 'a') && (__DATE__[2] == 'y')) ? 5 : 0
#define BUILD_MONTH_JUN ((__DATE__[0] == 'J') && (__DATE__[1] == 'u') && (__DATE__[2] == 'n')) ? 6 : 0
#define BUILD_MONTH_JUL ((__DATE__[0] == 'J') && (__DATE__[1] == 'u') && (__DATE__[2] == 'l')) ? 7 : 0
#define BUILD_MONTH_AUG ((__DATE__[0] == 'A') && (__DATE__[1] == 'u')) ? 8 : 0
#define BUILD_MONTH_SEP (__DATE__[0] == 'S') ? 9 : 0
#define BUILD_MONTH_OCT (__DATE__[0] == 'O') ? 10 : 0
#define BUILD_MONTH_NOV (__DATE__[0] == 'N') ? 11 : 0
#define BUILD_MONTH_DEC (__DATE__[0] == 'D') ? 12 : 0
#define BUILD_MONTH BUILD_MONTH_JAN | BUILD_MONTH_FEB | BUILD_MONTH_MAR | \
BUILD_MONTH_APR | BUILD_MONTH_MAY | BUILD_MONTH_JUN | \
BUILD_MONTH_JUL | BUILD_MONTH_AUG | BUILD_MONTH_SEP | \
BUILD_MONTH_OCT | BUILD_MONTH_NOV | BUILD_MONTH_DEC
// <DATE>                                  
#define BUILD_DATE_0 ((__DATE__[4] == ' ') ? 0 : (__DATE__[4] - 0x30))
#define BUILD_DATE_1 (__DATE__[5] - 0x30)
#define BUILD_DATE ((BUILD_DATE_0 * 10) + BUILD_DATE_1)
// <YEAR>                                  
#define BUILD_YEAR (((__DATE__[7] - 0x30) * 1000) + ((__DATE__[8] - 0x30) * 100) + \
((__DATE__[9] - 0x30) * 10)  + ((__DATE__[10] - 0x30) * 1))

// Parse the __TIME__ predefined macro to generate time defaults:
// __TIME__ Format: HH:MM:SS (First number of each is padded by 0 if <10)
// <HOUR>                                  
#define BUILD_HOUR_0 ((__TIME__[0] == ' ') ? 0 : (__TIME__[0] - 0x30))
#define BUILD_HOUR_1 (__TIME__[1] - 0x30)
#define BUILD_HOUR ((BUILD_HOUR_0 * 10) + BUILD_HOUR_1)
// <MINUTE>                                  
#define BUILD_MINUTE_0 ((__TIME__[3] == ' ') ? 0 : (__TIME__[3] - 0x30))
#define BUILD_MINUTE_1 (__TIME__[4] - 0x30)
#define BUILD_MINUTE ((BUILD_MINUTE_0 * 10) + BUILD_MINUTE_1)
// <SECOND>                                  
#define BUILD_SECOND_0 ((__TIME__[6] == ' ') ? 0 : (__TIME__[6] - 0x30))
#define BUILD_SECOND_1 (__TIME__[7] - 0x30)
#define BUILD_SECOND ((BUILD_SECOND_0 * 10) + BUILD_SECOND_1)

namespace pimoroni {

  bool RV3028::init() {
    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_pull_up(interrupt);
    }

    uint8_t chip_id = 0;
    i2c->read_bytes(address, RV3028_ID, &chip_id, 1);
    if(chip_id != (RV3028_CHIP_ID | RV3028_VERSION)) {
      return false;
    }

    return true;
  }

  void RV3028::reset() {
    set_bit(RV3028_CTRL2, CTRL2_RESET);
  }

  i2c_inst_t* RV3028::get_i2c() const {
    return i2c->get_i2c();
  }

  int RV3028::get_sda() const {
    return i2c->get_sda();
  }

  int RV3028::get_scl() const {
    return i2c->get_scl();
  }

  int RV3028::get_int() const {
    return interrupt;
  }

  bool RV3028::setup(bool set_24Hour, bool disable_TrickleCharge, bool set_LevelSwitchingMode) {
    sleep_ms(1000);
    if(set_24Hour) {
      set_24_hour();
      sleep_ms(1000);
    }

    if(disable_TrickleCharge) {
      disable_trickle_charge();
      sleep_ms(1000);
    }

    return ((set_LevelSwitchingMode ? set_backup_switchover_mode(3) : true) && write_register(RV3028_STATUS, 0x00));
  }

  bool RV3028::set_time(uint8_t sec, uint8_t min, uint8_t hour, uint8_t weekday, uint8_t date, uint8_t month, uint16_t year) {
    times[TIME_SECONDS] = dec_to_bcd(sec);
    times[TIME_MINUTES] = dec_to_bcd(min);
    times[TIME_HOURS] = dec_to_bcd(hour);
    times[TIME_WEEKDAY] = dec_to_bcd(weekday);
    times[TIME_DATE] = dec_to_bcd(date);
    times[TIME_MONTH] = dec_to_bcd(month);
    times[TIME_YEAR] = dec_to_bcd(year - 2000);

    bool status = false;

    if(is_12_hour()) {
      set_24_hour();
      status = set_time(times, TIME_ARRAY_LENGTH);
      set_12_hour();
    }
    else {
      status = set_time(times, TIME_ARRAY_LENGTH);
    }
    return status;
  }

  // setTime -- Set time and date/day registers of RV3028 (using data array)
  bool RV3028::set_time(uint8_t * time, uint8_t len) {
    if(len != TIME_ARRAY_LENGTH)
      return false;

    return write_multiple_registers(RV3028_SECONDS, time, len);
  }

  bool RV3028::set_seconds(uint8_t value) {
    times[TIME_SECONDS] = dec_to_bcd(value);
    return set_time(times, TIME_ARRAY_LENGTH);
  }

  bool RV3028::set_minutes(uint8_t value) {
    times[TIME_MINUTES] = dec_to_bcd(value);
    return set_time(times, TIME_ARRAY_LENGTH);
  }

  bool RV3028::set_hours(uint8_t value) {
    times[TIME_HOURS] = dec_to_bcd(value);
    return set_time(times, TIME_ARRAY_LENGTH);
  }

  bool RV3028::set_weekday(uint8_t value) {
    times[TIME_WEEKDAY] = dec_to_bcd(value);
    return set_time(times, TIME_ARRAY_LENGTH);
  }

  bool RV3028::set_date(uint8_t value) {
    times[TIME_DATE] = dec_to_bcd(value);
    return set_time(times, TIME_ARRAY_LENGTH);
  }

  bool RV3028::set_month(uint8_t value) {
    times[TIME_MONTH] = dec_to_bcd(value);
    return set_time(times, TIME_ARRAY_LENGTH);
  }

  bool RV3028::set_year(uint16_t value) {
    times[TIME_YEAR] = dec_to_bcd(value - 2000);
    return set_time(times, TIME_ARRAY_LENGTH);
  }

  // Takes the time from the last build and uses it as the current time
  bool RV3028::set_to_compiler_time() {
    times[TIME_SECONDS] = dec_to_bcd(BUILD_SECOND);
    times[TIME_MINUTES] = dec_to_bcd(BUILD_MINUTE);
    times[TIME_HOURS] = dec_to_bcd(BUILD_HOUR);

    // Build_Hour is 0-23, convert to 1-12 if needed
    if(is_12_hour()) {
      uint8_t hour = BUILD_HOUR;

      bool pm = false;

      if(hour == 0)
        hour += 12;
      else if(hour == 12)
        pm = true;
      else if(hour > 12) {
        hour -= 12;
        pm = true;
      }

      times[TIME_HOURS] = dec_to_bcd(hour); // Load the modified hours

      if(pm == true)
        times[TIME_HOURS] |= (1 << HOURS_AM_PM); // Set AM/PM bit if needed
    }

    // Calculate weekday (from here: http://stackoverflow.com/a/21235587)
    // 0 = Sunday, 6 = Saturday
    uint16_t d = BUILD_DATE;
    uint16_t m = BUILD_MONTH;
    uint16_t y = BUILD_YEAR;
    uint16_t weekday = (d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) % 7 + 1;
    times[TIME_WEEKDAY] = dec_to_bcd(weekday);

    times[TIME_DATE] = dec_to_bcd(BUILD_DATE);
    times[TIME_MONTH] = dec_to_bcd(BUILD_MONTH);
    times[TIME_YEAR] = dec_to_bcd(BUILD_YEAR - 2000); // ! Not Y2K (or Y2.1K)-proof :(

    return set_time(times, TIME_ARRAY_LENGTH);
  }

  // Move the hours, mins, sec, etc registers from RV-3028-C7 into the _time array
  // Needs to be called before printing time or date
  // We do not protect the GPx registers. They will be overwritten. The user has plenty of RAM if they need it.
  bool RV3028::update_time() {
    if(read_multiple_registers(RV3028_SECONDS, times, TIME_ARRAY_LENGTH) == false)
      return false; // Something went wrong

    if(is_12_hour())
      times[TIME_HOURS] &= ~(1 << HOURS_AM_PM); // Remove this bit from value

    return true;
  }

  // Returns a pointer to array of chars that are the date in mm/dd/yyyy format because they're weird
  char* RV3028::string_date_usa() {
    static char date[11 + 3]; // Max of mm/dd/yyyy with \0 terminator (plus extra for worst case conversion)
    sprintf(date, "%02hhu/%02hhu/20%02hhu", bcd_to_dec(times[TIME_MONTH]), bcd_to_dec(times[TIME_DATE]), bcd_to_dec(times[TIME_YEAR]));
    return date;
  }

  // Returns a pointer to array of chars that are the date in dd/mm/yyyy format
  char* RV3028::string_date() {
    static char date[11 + 3]; // Max of dd/mm/yyyy with \0 terminator (plus extra for worst case conversion)
    sprintf(date, "%02hhu/%02hhu/20%02hhu", bcd_to_dec(times[TIME_DATE]), bcd_to_dec(times[TIME_MONTH]), bcd_to_dec(times[TIME_YEAR]));
    return date;
  }

  // Returns a pointer to array of chars that represents the time in hh:mm:ss format
  // Adds AM/PM if in 12 hour mode
  char* RV3028::string_time() {
    static char time[11 + 3]; // Max of hh:mm:ssXM with \0 terminator (plus extra for worst case conversion)

    if(is_12_hour() == true) {
      char half = 'A';
      if(is_pm()) half = 'P';

      sprintf(time, "%02hhu:%02hhu:%02hhu%cM", bcd_to_dec(times[TIME_HOURS]), bcd_to_dec(times[TIME_MINUTES]), bcd_to_dec(times[TIME_SECONDS]), half);
    }
    else
      sprintf(time, "%02hhu:%02hhu:%02hhu", bcd_to_dec(times[TIME_HOURS]), bcd_to_dec(times[TIME_MINUTES]), bcd_to_dec(times[TIME_SECONDS]));

    return time;
  }

  char* RV3028::string_time_stamp() {
    static char time_stamp[25 + 4]; // Max of yyyy-mm-ddThh:mm:ss.ss with \0 terminator (plus extra for worst case conversion)

    if(is_12_hour() == true) {
      char half = 'A';
      if(is_pm()) half = 'P';

      sprintf(time_stamp, "20%02hhu-%02hhu-%02hhu  %02hhu:%02hhu:%02hhu%cM", bcd_to_dec(times[TIME_YEAR]), bcd_to_dec(times[TIME_MONTH]), bcd_to_dec(times[TIME_DATE]), bcd_to_dec(times[TIME_HOURS]), bcd_to_dec(times[TIME_MINUTES]), bcd_to_dec(times[TIME_SECONDS]), half);
    }
    else
      sprintf(time_stamp, "20%02hhu-%02hhu-%02hhu  %02hhu:%02hhu:%02hhu", bcd_to_dec(times[TIME_YEAR]), bcd_to_dec(times[TIME_MONTH]), bcd_to_dec(times[TIME_DATE]), bcd_to_dec(times[TIME_HOURS]), bcd_to_dec(times[TIME_MINUTES]), bcd_to_dec(times[TIME_SECONDS]));

    return time_stamp;
  }

  uint8_t RV3028::get_seconds() {
    return bcd_to_dec(times[TIME_SECONDS]);
  }

  uint8_t RV3028::get_minutes() {
    return bcd_to_dec(times[TIME_MINUTES]);
  }

  uint8_t RV3028::get_hours() {
    return bcd_to_dec(times[TIME_HOURS]);
  }

  uint8_t RV3028::get_weekday() {
    return bcd_to_dec(times[TIME_WEEKDAY]);
  }

  uint8_t RV3028::get_date() {
    return bcd_to_dec(times[TIME_DATE]);
  }

  uint8_t RV3028::get_month() {
    return bcd_to_dec(times[TIME_MONTH]);
  }

  uint16_t RV3028::get_year() {
    return bcd_to_dec(times[TIME_YEAR]) + 2000;
  }

  // Returns true if RTC has been configured for 12 hour mode
  bool RV3028::is_12_hour() {
    uint8_t controlRegister2 = read_register(RV3028_CTRL2);
    return (controlRegister2 & (1 << CTRL2_12_24));
  }

  // Returns true if RTC has PM bit set and 12Hour bit set
  bool RV3028::is_pm() {
    uint8_t hourRegister = read_register(RV3028_HOURS);
    if(is_12_hour() && (hourRegister & (1 << HOURS_AM_PM)))
      return true;
    return false;
  }

  // Configure RTC to output 1-12 hours
  // Converts any current hour setting to 12 hour
  void RV3028::set_12_hour() {
    // Do we need to change anything?
    if(is_12_hour() == false) {
      uint8_t hour = bcd_to_dec(read_register(RV3028_HOURS)); // Get the current hour in the RTC

      // Set the 12/24 hour bit
      uint8_t setting = read_register(RV3028_CTRL2);
      setting |= (1 << CTRL2_12_24);
      write_register(RV3028_CTRL2, setting);

      // Take the current hours and convert to 12, complete with AM/PM bit
      bool pm = false;

      if(hour == 0)
        hour += 12;
      else if(hour == 12)
        pm = true;
      else if(hour > 12) {
        hour -= 12;
        pm = true;
      }

      hour = dec_to_bcd(hour); // Convert to BCD

      if(pm == true) hour |= (1 << HOURS_AM_PM); // Set AM/PM bit if needed

      write_register(RV3028_HOURS, hour); // Record this to hours register
    }
  }

  // Configure RTC to output 0-23 hours
  // Converts any current hour setting to 24 hour
  void RV3028::set_24_hour() {
    // Do we need to change anything?
    if(is_12_hour() == true) {
      // Not sure what changing the CTRL2 register will do to hour register so let's get a copy
      uint8_t hour = read_register(RV3028_HOURS); //Get the current 12 hour formatted time in BCD
      bool pm = false;
      if(hour & (1 << HOURS_AM_PM)) { // Is the AM/PM bit set?
        pm = true;
        hour &= ~(1 << HOURS_AM_PM); // Clear the bit
      }

      // Change to 24 hour mode
      uint8_t setting = read_register(RV3028_CTRL2);
      setting &= ~(1 << CTRL2_12_24); // Clear the 12/24 hr bit
      write_register(RV3028_CTRL2, setting);

      // Given a BCD hour in the 1-12 range, make it 24
      hour = bcd_to_dec(hour); // Convert core of register to DEC

      if(pm == true) hour += 12; // 2PM becomes 14
      if(hour == 12) hour = 0;   // 12AM stays 12, but should really be 0
      if(hour == 24) hour = 12;  // 12PM becomes 24, but should really be 12

      hour = dec_to_bcd(hour);   // Convert to BCD

      write_register(RV3028_HOURS, hour); // Record this to hours register
    }
  }

  // ATTENTION: Real Time and UNIX Time are INDEPENDENT!
  bool RV3028::set_unix(uint32_t value) {
    uint8_t unix_reg[4];
    unix_reg[0] = value;
    unix_reg[1] = value >> 8;
    unix_reg[2] = value >> 16;
    unix_reg[3] = value >> 24;

    return write_multiple_registers(RV3028_UNIX_TIME0, unix_reg, 4);
  }

  // ATTENTION: Real Time and UNIX Time are INDEPENDENT!
  uint32_t RV3028::get_unix() {
    uint8_t unix_reg[4];
    read_multiple_registers(RV3028_UNIX_TIME0, unix_reg, 4);
    return ((uint32_t)unix_reg[3] << 24) | ((uint32_t)unix_reg[2] << 16) | ((uint32_t)unix_reg[1] << 8) | unix_reg[0];
  }

  /*********************************
  Set the alarm mode in the following way:
  0: When minutes, hours and weekday/date match (once per weekday/date)
  1: When hours and weekday/date match (once per weekday/date)
  2: When minutes and weekday/date match (once per hour per weekday/date)
  3: When weekday/date match (once per weekday/date)
  4: When hours and minutes match (once per day)
  5: When hours match (once per day)
  6: When minutes match (once per hour)
  7: All disabled � Default value
  If you want to set a weekday alarm (setWeekdayAlarm_not_Date = true), set 'date_or_weekday' from 0 (Sunday) to 6 (Saturday)
  ********************************/
  void RV3028::enable_alarm_interrupt(uint8_t min, uint8_t hour, uint8_t date_or_weekday, bool set_weekday_alarm_not_date, uint8_t mode, bool enable_clock_output) {
    // disable Alarm Interrupt to prevent accidental interrupts during configuration
    disable_alarm_interrupt();
    clear_alarm_interrupt_flag();

    // ENHANCEMENT: Add Alarm in 12 hour mode
    set_24_hour();

    // Set WADA bit (Weekday/Date Alarm)
    if(set_weekday_alarm_not_date)
      clear_bit(RV3028_CTRL1, CTRL1_WADA);
    else
      set_bit(RV3028_CTRL1, CTRL1_WADA);

    // Write alarm settings in registers 0x07 to 0x09
    uint8_t alarmTime[3];
    alarmTime[0] = dec_to_bcd(min);        //minutes
    alarmTime[1] = dec_to_bcd(hour);        //hours
    alarmTime[2] = dec_to_bcd(date_or_weekday);  //date or weekday
    // shift alarm enable bits
    if(mode > 0b111)
      mode = 0b111; // 0 to 7 is valid

    if(mode & 0b001)
      alarmTime[0] |= 1 << MINUTESALM_AE_M;
    if(mode & 0b010)
      alarmTime[1] |= 1 << HOURSALM_AE_H;
    if(mode & 0b100)
      alarmTime[2] |= 1 << DATE_AE_WD;

    write_multiple_registers(RV3028_MINUTES_ALM, alarmTime, 3);

    enable_alarm_interrupt();

    // Clock output?
    if(enable_clock_output)
      set_bit(RV3028_INT_MASK, IMT_MASK_CAIE);
    else
      clear_bit(RV3028_INT_MASK, IMT_MASK_CAIE);
  }

  void RV3028::enable_alarm_interrupt() {
    set_bit(RV3028_CTRL2, CTRL2_AIE);
  }

  // Only disables the interrupt (not the alarm flag)
  void RV3028::disable_alarm_interrupt() {
    clear_bit(RV3028_CTRL2, CTRL2_AIE);
  }

  bool RV3028::read_alarm_interrupt_flag() {
    return read_bit(RV3028_STATUS, STATUS_AF);
  }

  void RV3028::clear_alarm_interrupt_flag() {
    clear_bit(RV3028_STATUS, STATUS_AF);
  }

  /*********************************
  Countdown Timer Interrupt
  ********************************/
  void RV3028::set_timer(bool timer_repeat, uint16_t timer_frequency, uint16_t timer_value, bool set_interrupt, bool start_timer, bool enable_clock_output) {
    disable_timer();
    disable_timer_interrupt();
    clear_timer_interrupt_flag();

    write_register(RV3028_TIMERVAL_0, timer_value & 0xff);
    write_register(RV3028_TIMERVAL_1, timer_value >> 8);

    uint8_t ctrl1_val = read_register(RV3028_CTRL1);
    if(timer_repeat) {
      ctrl1_val |= 1 << CTRL1_TRPT;
    }
    else {
      ctrl1_val &= ~(1 << CTRL1_TRPT);
    }

    switch(timer_frequency) {
      case 4096:         // 4096Hz (default)    // up to 122us error on first time
        ctrl1_val &= ~3; // Clear both the bits
        break;

      case 64:           // 64Hz          // up to 7.813ms error on first time
        ctrl1_val &= ~3; // Clear both the bits
        ctrl1_val |= 1;
        break;

      case 1:            // 1Hz          // up to 7.813ms error on first time
        ctrl1_val &= ~3; // Clear both the bits
        ctrl1_val |= 2;
        break;

      case 60000:       // 1/60Hz        // up to 7.813ms error on first time
        ctrl1_val |= 3; // Set both bits
        break;
    }

    if(set_interrupt) {
      enable_timer_interrupt();
    }
    if(start_timer) {
      ctrl1_val |= (1 << CTRL1_TE);
    }
    write_register(RV3028_CTRL1, ctrl1_val);

    // Clock output?
    if(enable_clock_output)
      set_bit(RV3028_INT_MASK, IMT_MASK_CTIE);
    else
      clear_bit(RV3028_INT_MASK, IMT_MASK_CTIE);
  }

  uint16_t RV3028::get_timer_count() {
    // Reads the number of remaining timer ticks
    uint8_t r0 = read_register(RV3028_TIMERSTAT_0);
    return (r0 + (read_register(RV3028_TIMERSTAT_1) << 8));
  }

  void RV3028::enable_timer_interrupt() {
    set_bit(RV3028_CTRL2, CTRL2_TIE);
  }

  void RV3028::disable_timer_interrupt() {
    clear_bit(RV3028_CTRL2, CTRL2_TIE);
  }

  bool RV3028::read_timer_interrupt_flag() {
    return read_bit(RV3028_STATUS, STATUS_TF);
  }

  void RV3028::clear_timer_interrupt_flag() {
    clear_bit(RV3028_STATUS, STATUS_TF);
  }

  void RV3028::enable_timer() {
    set_bit(RV3028_CTRL1, CTRL1_TE);
  }

  void RV3028::disable_timer() {
    clear_bit(RV3028_CTRL1, CTRL1_TE);
  }

  /*********************************
  Periodic Time Update Interrupt
  ********************************/
  void RV3028::enable_periodic_update_interrupt(bool every_second, bool enable_clock_output) {
    disable_periodic_update_interrupt();
    clear_periodic_update_interrupt_flag();

    if(every_second)
      clear_bit(RV3028_CTRL1, CTRL1_USEL);
    else  // every minute
      set_bit(RV3028_CTRL1, CTRL1_USEL);

    set_bit(RV3028_CTRL2, CTRL2_UIE);

    // Clock output?
    if(enable_clock_output)
      set_bit(RV3028_INT_MASK, IMT_MASK_CUIE);
    else
      clear_bit(RV3028_INT_MASK, IMT_MASK_CUIE);
  }

  void RV3028::disable_periodic_update_interrupt() {
    clear_bit(RV3028_CTRL2, CTRL2_UIE);
  }

  bool RV3028::read_periodic_update_interrupt_flag() {
    return read_bit(RV3028_STATUS, STATUS_UF);
  }

  void RV3028::clear_periodic_update_interrupt_flag() {
    clear_bit(RV3028_STATUS, STATUS_UF);
  }

  /*********************************
  Enable the Trickle Charger and set the Trickle Charge series resistor (default is 15k)
  TCR_3K  =  3kOhm
  TCR_5K  =  5kOhm
  TCR_9K  =  9kOhm
  TCR_15K = 15kOhm
  *********************************/
  void RV3028::enable_trickle_charge(uint8_t tcr) {
    if(tcr > 3)
      return;

    // Read EEPROM Backup Register (0x37)
    uint8_t eeprom_backup = read_config_eeprom_ram_mirror(EEPROM_Backup_Register);
    // Set TCR Bits (Trickle Charge Resistor)
    eeprom_backup &= EEPROMBackup_TCR_CLEAR;         // Clear TCR Bits
    eeprom_backup |= tcr << EEPROMBackup_TCR_SHIFT;  // Shift values into EEPROM Backup Register
    // Write 1 to TCE Bit
    eeprom_backup |= 1 << EEPROMBackup_TCE_BIT;
    // Write EEPROM Backup Register
    write_config_eeprom_ram_mirror(EEPROM_Backup_Register, eeprom_backup);
  }

  void RV3028::disable_trickle_charge() {
    // Read EEPROM Backup Register (0x37)
    uint8_t eeprom_backup = read_config_eeprom_ram_mirror(EEPROM_Backup_Register);
    // Write 0 to TCE Bit
    eeprom_backup &= ~(1 << EEPROMBackup_TCE_BIT);
    // Write EEPROM Backup Register
    write_config_eeprom_ram_mirror(EEPROM_Backup_Register, eeprom_backup);
  }


  /*********************************
  0 = Switchover disabled
  1 = Direct Switching Mode
  2 = Standby Mode
  3 = Level Switching Mode
  *********************************/
  bool RV3028::set_backup_switchover_mode(uint8_t val) {
    if(val > 3)
      return false;

    bool success = true;

    // Read EEPROM Backup Register (0x37)
    uint8_t eeprom_backup = read_config_eeprom_ram_mirror(EEPROM_Backup_Register);
    if(eeprom_backup == 0xFF)
      success = false;

    // Ensure FEDE Bit is set to 1
    eeprom_backup |= 1 << EEPROMBackup_FEDE_BIT;

    // Set BSM Bits (Backup Switchover Mode)
    eeprom_backup &= EEPROMBackup_BSM_CLEAR;         // Clear BSM Bits of EEPROM Backup Register
    eeprom_backup |= val << EEPROMBackup_BSM_SHIFT;  // Shift values into EEPROM Backup Register

    // Write EEPROM Backup Register
    if(!write_config_eeprom_ram_mirror(EEPROM_Backup_Register, eeprom_backup))
      success = false;

    return success;
  }


  /*********************************
  Clock Out functions
  ********************************/
  void RV3028::enable_clock_out(uint8_t freq) {
    if(freq > 7)
      return; // check out of bounds

    // Read EEPROM CLKOUT Register (0x35)
    uint8_t eeprom_clkout = read_config_eeprom_ram_mirror(EEPROM_Clkout_Register);
    // Ensure CLKOE Bit is set to 1
    eeprom_clkout |= 1 << EEPROMClkout_CLKOE_BIT;
    // Shift values into EEPROM Backup Register
    eeprom_clkout |= freq << EEPROMClkout_FREQ_SHIFT;
    // Write EEPROM Backup Register
    write_config_eeprom_ram_mirror(EEPROM_Clkout_Register, eeprom_clkout);
  }

  void RV3028::enable_interrupt_controlled_clockout(uint8_t freq) {
    if(freq > 7)
      return; // check out of bounds

    // Read EEPROM CLKOUT Register (0x35)
    uint8_t eeprom_clkout = read_config_eeprom_ram_mirror(EEPROM_Clkout_Register);
    // Shift values into EEPROM Backup Register
    eeprom_clkout |= freq << EEPROMClkout_FREQ_SHIFT;
    // Write EEPROM Backup Register
    write_config_eeprom_ram_mirror(EEPROM_Clkout_Register, eeprom_clkout);

    // Set CLKIE Bit
    set_bit(RV3028_CTRL2, CTRL2_CLKIE);
  }

  void RV3028::disable_clock_out() {
    // Read EEPROM CLKOUT Register (0x35)
    uint8_t eeprom_clkout = read_config_eeprom_ram_mirror(EEPROM_Clkout_Register);
    // Clear CLKOE Bit
    eeprom_clkout &= ~(1 << EEPROMClkout_CLKOE_BIT);
    // Write EEPROM CLKOUT Register
    write_config_eeprom_ram_mirror(EEPROM_Clkout_Register, eeprom_clkout);

    // Clear CLKIE Bit
    clear_bit(RV3028_CTRL2, CTRL2_CLKIE);
  }

  bool RV3028::read_clock_output_interrupt_flag() {
    return read_bit(RV3028_STATUS, STATUS_CLKF);
  }

  void RV3028::clear_clock_output_interrupt_flag() {
    clear_bit(RV3028_STATUS, STATUS_CLKF);
  }

  // Returns the status byte
  uint8_t RV3028::status(void) {
    return read_register(RV3028_STATUS);
  }

  void RV3028::clear_interrupts() { // Read the status register to clear the current interrupt flags
    write_register(RV3028_STATUS, 0);
  }

  /*********************************
  FOR INTERNAL USE
  ********************************/
  uint8_t RV3028::bcd_to_dec(uint8_t val) {
    return ((val / 0x10) * 10) + (val % 0x10);
  }

  // BCDtoDEC -- convert decimal to binary-coded decimal (BCD)
  uint8_t RV3028::dec_to_bcd(uint8_t val) {
    return ((val / 10) * 0x10) + (val % 10);
  }

  uint8_t RV3028::read_register(uint8_t addr) {
    uint8_t b1[2];
    if(1 == i2c->read_bytes(address, addr, b1, 1))
      return b1[0];
    else
      return 0xFF; //Error
  }

  bool RV3028::write_register(uint8_t addr, uint8_t val) {
    uint8_t b1[2];
    b1[0] = val;
    b1[1] = 0;
    return i2c->write_bytes(address, addr, b1, 1);
  }

  bool RV3028::read_multiple_registers(uint8_t addr, uint8_t *dest, uint8_t len) {
    return i2c->read_bytes(address, addr, dest, len);
  }

  bool RV3028::write_multiple_registers(uint8_t addr, uint8_t *values, uint8_t len) {
    return i2c->write_bytes(address, addr, values, len);
  }

  bool RV3028::write_config_eeprom_ram_mirror(uint8_t eeprom_addr, uint8_t val) {
    bool success = wait_for_eeprom();

    // Disable auto refresh by writing 1 to EERD control bit in CTRL1 register
    uint8_t ctrl1 = read_register(RV3028_CTRL1);
    ctrl1 |= 1 << CTRL1_EERD;

    if(!write_register(RV3028_CTRL1, ctrl1))
      success = false;
    
    // Write Configuration RAM Register
    write_register(eeprom_addr, val);

    // Update EEPROM (All Configuration RAM -> EEPROM)
    write_register(RV3028_EEPROM_CMD, EEPROMCMD_First);
    write_register(RV3028_EEPROM_CMD, EEPROMCMD_Update);
    
    if(!wait_for_eeprom())
      success = false;
    
    // Reenable auto refresh by writing 0 to EERD control bit in CTRL1 register
    ctrl1 = read_register(RV3028_CTRL1);
    if(ctrl1 == 0x00)
      success = false;
      
    ctrl1 &= ~(1 << CTRL1_EERD);
    write_register(RV3028_CTRL1, ctrl1);

    if(!wait_for_eeprom())
      success = false;

    return success;
  }

  uint8_t RV3028::read_config_eeprom_ram_mirror(uint8_t eeprom_addr) {
    bool success = wait_for_eeprom();

    // Disable auto refresh by writing 1 to EERD control bit in CTRL1 register
    uint8_t ctrl1 = read_register(RV3028_CTRL1);
    ctrl1 |= 1 << CTRL1_EERD;
    if(!write_register(RV3028_CTRL1, ctrl1))
      success = false;

    // Read EEPROM Register
    write_register(RV3028_EEPROM_ADDR, eeprom_addr);
    write_register(RV3028_EEPROM_CMD, EEPROMCMD_First);
    write_register(RV3028_EEPROM_CMD, EEPROMCMD_ReadSingle);

    if(!wait_for_eeprom())
      success = false;

    uint8_t eeprom_data = read_register(RV3028_EEPROM_DATA);
    if(!wait_for_eeprom())
      success = false;

    // Reenable auto refresh by writing 0 to EERD control bit in CTRL1 register
    ctrl1 = read_register(RV3028_CTRL1);
    if(ctrl1 == 0x00)
      success = false;

    ctrl1 &= ~(1 << CTRL1_EERD);
    write_register(RV3028_CTRL1, ctrl1);

    if(!success)
      return 0xFF;
    
    return eeprom_data;
  }
 
  // True if success, false if timeout occured
  bool RV3028::wait_for_eeprom() {
    // Timeout is number of loops round while - don't have access to millisecond counter
    unsigned long timeout = 500;
    while ((read_register(RV3028_STATUS) & 1 << STATUS_EEBUSY) && --timeout > 0);
    return timeout > 0;
 
  }

  void RV3028::set_bit(uint8_t reg_addr, uint8_t bit_num) {
    i2c->set_bits(address, reg_addr, bit_num, 0x01);
  }

  void RV3028::clear_bit(uint8_t reg_addr, uint8_t bit_num) {
    i2c->clear_bits(address, reg_addr, bit_num, 0x01);
  }

  bool RV3028::read_bit(uint8_t reg_addr, uint8_t bit_num) {
    uint8_t value = i2c->get_bits(address, reg_addr, bit_num, 0x01);
    return value;
  }
}