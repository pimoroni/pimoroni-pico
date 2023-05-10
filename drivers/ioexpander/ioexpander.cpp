#include <cstdio>
#include <cstdlib>
#include <math.h>
#include <map>
#include <vector>

#include "ioexpander.hpp"

namespace pimoroni {

  enum reg {
    CHIP_ID_L = 0xfa,
    CHIP_ID_H = 0xfb,
    VERSION = 0xfc,

    // Rotary encoder
    ENC_EN = 0x04,
    // BIT_ENC_EN_1 = 0
    // BIT_ENC_MICROSTEP_1 = 1
    // BIT_ENC_EN_2 = 2
    // BIT_ENC_MICROSTEP_2 = 3
    // BIT_ENC_EN_3 = 4
    // BIT_ENC_MICROSTEP_3 = 5
    // BIT_ENC_EN_4 = 6
    // BIT_ENC_MICROSTEP_4 = 7

    ENC_1_CFG = 0x05,
    ENC_1_COUNT = 0x06,
    ENC_2_CFG = 0x07,
    ENC_2_COUNT = 0x08,
    ENC_3_CFG = 0x09,
    ENC_3_COUNT = 0x0A,
    ENC_4_CFG = 0x0B,
    ENC_4_COUNT = 0x0C,

    // Cap touch
    CAPTOUCH_EN = 0x0D,
    CAPTOUCH_CFG = 0x0E,
    CAPTOUCH_0 = 0x0F,  // First of 8 bytes from 15-22

    // Switch counters
    SWITCH_EN_P0 = 0x17,
    SWITCH_EN_P1 = 0x18,
    SWITCH_P00 = 0x19,  // First of 8 bytes from 25-40
    SWITCH_P10 = 0x21,  // First of 8 bytes from 33-49

    USER_FLASH = 0xD0,
    FLASH_PAGE = 0xF0,
    DEBUG = 0xF8,

    P0 = 0x40,       // protect_bits 2 # Bit addressing
    SP = 0x41,       // Read only
    DPL = 0x42,      // Read only
    DPH = 0x43,      // Read only
    RCTRIM0 = 0x44,  // Read only
    RCTRIM1 = 0x45,  // Read only
    RWK = 0x46,
    PCON = 0x47,     // Read only
    TCON = 0x48,
    TMOD = 0x49,
    TL0 = 0x4a,
    TL1 = 0x4b,
    TH0 = 0x4c,
    TH1 = 0x4d,
    CKCON = 0x4e,
    WKCON = 0x4f,    // Read only
    P1 = 0x50,       // protect_bits 3 6 # Bit addressing
    SFRS = 0x51,     // TA protected # Read only
    CAPCON0 = 0x52,
    CAPCON1 = 0x53,
    CAPCON2 = 0x54,
    CKDIV = 0x55,
    CKSWT = 0x56,    // TA protected # Read only
    CKEN = 0x57,     // TA protected # Read only
    SCON = 0x58,
    SBUF = 0x59,
    SBUF_1 = 0x5a,
    EIE = 0x5b,      // Read only
    EIE1 = 0x5c,     // Read only
    CHPCON = 0x5f,   // TA protected # Read only
    P2 = 0x60,       // Bit addressing
    AUXR1 = 0x62,
    BODCON0 = 0x63,  // TA protected
    IAPTRG = 0x64,   // TA protected # Read only
    IAPUEN = 0x65,   // TA protected # Read only
    IAPAL = 0x66,    // Read only
    IAPAH = 0x67,    // Read only
    IE = 0x68,       // Read only
    SADDR = 0x69,
    WDCON = 0x6a,    // TA protected
    BODCON1 = 0x6b,  // TA protected
    P3M1 = 0x6c,
    P3S = 0xc0,      // Page 1 # Reassigned from 0x6c to avoid collision
    P3M2 = 0x6d,
    P3SR = 0xc1,     // Page 1 # Reassigned from 0x6d to avoid collision
    IAPFD = 0x6e,    // Read only
    IAPCN = 0x6f,    // Read only
    P3 = 0x70,       // Bit addressing
    P0M1 = 0x71,     // protect_bits  2
    P0S = 0xc2,      // Page 1 # Reassigned from 0x71 to avoid collision
    P0M2 = 0x72,     // protect_bits  2
    P0SR = 0xc3,     // Page 1 # Reassigned from 0x72 to avoid collision
    P1M1 = 0x73,     // protect_bits  3 6
    P1S = 0xc4,      // Page 1 # Reassigned from 0x73 to avoid collision
    P1M2 = 0x74,     // protect_bits  3 6
    P1SR = 0xc5,     // Page 1 # Reassigned from 0x74 to avoid collision
    P2S = 0x75,
    IPH = 0x77,      // Read only
    PWMINTC = 0xc6,  // Page 1 # Read only # Reassigned from 0x77 to avoid collision
    IP = 0x78,       // Read only
    SADEN = 0x79,
    SADEN_1 = 0x7a,
    SADDR_1 = 0x7b,
    I2DAT = 0x7c,    // Read only
    I2STAT = 0x7d,   // Read only
    I2CLK = 0x7e,    // Read only
    I2TOC = 0x7f,    // Read only
    I2CON = 0x80,    // Read only
    I2ADDR = 0x81,   // Read only
    ADCRL = 0x82,
    ADCRH = 0x83,
    T3CON = 0x84,
    PWM4H = 0xc7,    // Page 1 # Reassigned from 0x84 to avoid collision
    RL3 = 0x85,
    PWM5H = 0xc8,    // Page 1 # Reassigned from 0x85 to avoid collision
    RH3 = 0x86,
    PIOCON1 = 0xc9,  // Page 1 # Reassigned from 0x86 to avoid collision
    TA = 0x87,       // Read only
    T2CON = 0x88,
    T2MOD = 0x89,
    RCMP2L = 0x8a,
    RCMP2H = 0x8b,
    TL2 = 0x8c,
    PWM4L = 0xca,    // Page 1 # Reassigned from 0x8c to avoid collision
    TH2 = 0x8d,
    PWM5L = 0xcb,    // Page 1 # Reassigned from 0x8d to avoid collision
    ADCMPL = 0x8e,
    ADCMPH = 0x8f,
    PSW = 0x90,      // Read only
    PWMPH = 0x91,
    PWM0H = 0x92,
    PWM1H = 0x93,
    PWM2H = 0x94,
    PWM3H = 0x95,
    PNP = 0x96,
    FBD = 0x97,
    PWMCON0 = 0x98,
    PWMPL = 0x99,
    PWM0L = 0x9a,
    PWM1L = 0x9b,
    PWM2L = 0x9c,
    PWM3L = 0x9d,
    PIOCON0 = 0x9e,
    PWMCON1 = 0x9f,
    ACC = 0xa0,      // Read only
    ADCCON1 = 0xa1,
    ADCCON2 = 0xa2,
    ADCDLY = 0xa3,
    C0L = 0xa4,
    C0H = 0xa5,
    C1L = 0xa6,
    C1H = 0xa7,
    ADCCON0 = 0xa8,
    PICON = 0xa9,    // Read only
    PINEN = 0xaa,    // Read only
    PIPEN = 0xab,    // Read only
    PIF = 0xac,      // Read only
    C2L = 0xad,
    C2H = 0xae,
    EIP = 0xaf,      // Read only
    B = 0xb0,        // Read only
    CAPCON3 = 0xb1,
    CAPCON4 = 0xb2,
    SPCR = 0xb3,
    SPCR2 = 0xcc,    // Page 1 # Reassigned from 0xb3 to avoid collision
    SPSR = 0xb4,
    SPDR = 0xb5,
    AINDIDS = 0xb6,
    EIPH = 0xb7,     // Read only
    SCON_1 = 0xb8,
    PDTEN = 0xb9,    // TA protected
    PDTCNT = 0xba,   // TA protected
    PMEN = 0xbb,
    PMD = 0xbc,
    EIP1 = 0xbe,     // Read only
    EIPH1 = 0xbf,    // Read only

    INT = 0xf9,


    INT_MASK_P0 = 0x00,
    INT_MASK_P1 = 0x01,
    INT_MASK_P3 = 0x03,

    ADDR = 0xfd,

    CTRL = 0xfe,     // 0 = Sleep, 1 = Reset, 2 = Read Flash, 3 = Write Flash, 4 = Addr Unlock
  };

  enum int_mask {
    TRIG = 0x1,
    OUT = 0x2,
  };

  enum int_bit {
    TRIGD = 0,
    OUT_EN = 1,
    PIN_SWAP = 2,  // 0 = P1.3, 1 = P0.0
  };

  enum ctrl_mask {
    SLEEP = 0x1,
    RESET = 0x2,
    FREAD = 0x4,
    FWRITE = 0x8,
    ADDRWR = 0x10,
  };

  static const uint8_t NUM_BIT_ADDRESSED_REGISTERS = 4;
  static const uint8_t BIT_ADDRESSED_REGS[NUM_BIT_ADDRESSED_REGISTERS] = {reg::P0, reg::P1, reg::P2, reg::P3};
  static const uint8_t ENC_CFG[4] = {reg::ENC_1_CFG, reg::ENC_2_CFG, reg::ENC_3_CFG, reg::ENC_4_CFG};
  static const uint8_t ENC_COUNT[4] = {reg::ENC_1_COUNT, reg::ENC_2_COUNT, reg::ENC_3_COUNT, reg::ENC_4_COUNT};

  const uint8_t IOExpander::Pin::PxM1[4] = {reg::P0M1, reg::P1M1, (uint8_t)-1, reg::P3M1};
  const uint8_t IOExpander::Pin::PxM2[4] = {reg::P0M2, reg::P1M2, (uint8_t)-1, reg::P3M2};
  const uint8_t IOExpander::Pin::Px[4] = {reg::P0, reg::P1, (uint8_t)-1, reg::P3};

  const uint8_t IOExpander::Pin::PxS[4] = {reg::P0S, reg::P1S, (uint8_t)-1, reg::P3S};
  const uint8_t IOExpander::Pin::MASK_P[4] = {reg::INT_MASK_P0, reg::INT_MASK_P1, (uint8_t)-1, reg::INT_MASK_P3};

  const uint8_t IOExpander::Pin::PWML[6] = {reg::PWM0L, reg::PWM1L, reg::PWM2L, reg::PWM3L, reg::PWM4L, reg::PWM5L};
  const uint8_t IOExpander::Pin::PWMH[6] = {reg::PWM0H, reg::PWM1H, reg::PWM2H, reg::PWM3H, reg::PWM4H, reg::PWM5H};

  static const char* MODE_NAMES[3] = {"IO", "PWM", "ADC"};
  static const char* GPIO_NAMES[4] = {"QB", "PP", "IN", "OD"};
  static const char* STATE_NAMES[2] = {"LOW", "HIGH"};

  IOExpander::Pin::Pin(uint8_t port, uint8_t pin) :
    type(TYPE_IO), mode(0), port(port), pin(pin), adc_channel(0), pwm_channel(0),
    reg_m1(PxM1[port]), reg_m2(PxM2[port]), reg_p(Px[port]), reg_ps(PxS[port]), reg_int_mask_p(MASK_P[port]),
    reg_io_pwm(0), reg_pwml(0), reg_pwmh(0) {
  }

  IOExpander::Pin::Pin(uint8_t port, uint8_t pin, uint8_t pwm_channel, uint8_t reg_io_pwm) :
    type(TYPE_PWM), mode(0), port(port), pin(pin), adc_channel(0), pwm_channel(pwm_channel),
    reg_m1(PxM1[port]), reg_m2(PxM2[port]), reg_p(Px[port]), reg_ps(PxS[port]), reg_int_mask_p(MASK_P[port]),
    reg_io_pwm(reg_io_pwm), reg_pwml(PWML[pwm_channel]), reg_pwmh(PWMH[pwm_channel]) {
  }

  IOExpander::Pin::Pin(uint8_t port, uint8_t pin, uint8_t adc_channel) :
    type(TYPE_ADC), mode(0), port(port), pin(pin), adc_channel(adc_channel), pwm_channel(0),
    reg_m1(PxM1[port]), reg_m2(PxM2[port]), reg_p(Px[port]), reg_ps(PxS[port]), reg_int_mask_p(MASK_P[port]),
    reg_io_pwm(0), reg_pwml(0), reg_pwmh(0) {
  }

  IOExpander::Pin::Pin(uint8_t port, uint8_t pin, uint8_t adc_channel, uint8_t pwm_channel, uint8_t reg_io_pwm) :
    type(TYPE_ADC_OR_PWM), mode(0), port(port), pin(pin), adc_channel(adc_channel), pwm_channel(pwm_channel),
    reg_m1(PxM1[port]), reg_m2(PxM2[port]), reg_p(Px[port]), reg_ps(PxS[port]), reg_int_mask_p(MASK_P[port]),
    reg_io_pwm(reg_io_pwm), reg_pwml(PWML[pwm_channel]), reg_pwmh(PWMH[pwm_channel]) {
  }

  IOExpander::Pin IOExpander::Pin::io(uint8_t port, uint8_t pin) {
    return Pin(port, pin);
  }

  IOExpander::Pin IOExpander::Pin::pwm(uint8_t port, uint8_t pin, uint8_t channel, uint8_t reg_iopwm) {
    return Pin(port, pin, channel, reg_iopwm);
  }

  IOExpander::Pin IOExpander::Pin::adc(uint8_t port, uint8_t pin, uint8_t channel) {
    return Pin(port, pin, channel);
  }

  IOExpander::Pin IOExpander::Pin::adc_or_pwm(uint8_t port, uint8_t pin, uint8_t adc_channel, uint8_t pwm_channel, uint8_t reg_iopwm) {
    return Pin(port, pin, adc_channel, pwm_channel, reg_iopwm);
  }

  bool IOExpander::Pin::mode_supported(uint8_t mode) {
    bool supported = false;
    if((type & TYPE_PWM) && (mode == PIN_MODE_PWM)) {
      supported = true;
    }
    else if((type & TYPE_ADC) && (mode == PIN_MODE_ADC)) {
      supported = true;
    }
    return supported;
  }

  IOExpander::Pin::IOType IOExpander::Pin::get_type() {
    return type;
  }

  uint8_t IOExpander::Pin::get_mode() {
    return mode;
  }

  void IOExpander::Pin::set_mode(uint8_t mode) {
    this->mode = mode;
  }

  IOExpander::IOExpander(I2C *i2c, uint8_t address, uint interrupt, uint32_t timeout, bool debug) :
    i2c(i2c),
    address(address), interrupt(interrupt),
    timeout(timeout), debug(debug), vref(3.3f),
    encoder_offset{0,0,0,0},
    encoder_last{0,0,0,0},
    pins{ Pin::pwm(1, 5, 5, reg::PIOCON1),
          Pin::pwm(1, 0, 2, reg::PIOCON0),
          Pin::pwm(1, 2, 0, reg::PIOCON0),
          Pin::pwm(1, 4, 1, reg::PIOCON1),
          Pin::pwm(0, 0, 3, reg::PIOCON0),
          Pin::pwm(0, 1, 4, reg::PIOCON0),
          Pin::adc_or_pwm(1, 1, 7, 1, reg::PIOCON0),
          Pin::adc_or_pwm(0, 3, 6, 5, reg::PIOCON0),
          Pin::adc_or_pwm(0, 4, 5, 3, reg::PIOCON1),
          Pin::adc(3, 0, 1),
          Pin::adc(0, 6, 3),
          Pin::adc_or_pwm(0, 5, 4, 2, reg::PIOCON1),
          Pin::adc(0, 7, 2),
          Pin::adc(1, 7, 0)}
  {}

  bool IOExpander::init(bool skipChipIdCheck, bool perform_reset) {
    if(!skipChipIdCheck) {
      uint16_t chip_id = get_chip_id();
      if(chip_id != CHIP_ID) {
        if(debug) {
          printf("Chip ID invalid: %04x expected: %04x\n", chip_id, CHIP_ID);
        }
        return false;
      }
    }

    // Reset the chip if requested, to put it into a known state
    if(perform_reset && !reset()) {
      return false;
    }

    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_pull_up(interrupt);

      enable_interrupt_out(true);
    }

    return true;
  }

  uint8_t IOExpander::check_reset() {
    uint8_t user_flash_reg = reg::USER_FLASH;
    uint8_t value;
    if(i2c_write_blocking(i2c->get_i2c(), address, &user_flash_reg, 1, false) == PICO_ERROR_GENERIC) {
      return 0x00;
    }
    if(i2c_read_blocking(i2c->get_i2c(), address, (uint8_t *)&value, sizeof(uint8_t), false) == PICO_ERROR_GENERIC) {
      return 0x00;
    }

    return value;
  }

  bool IOExpander::reset() {
    uint32_t start_time = millis();
    set_bits(reg::CTRL, ctrl_mask::RESET);
    // Wait for a register to read its initialised value
    while(check_reset() != 0x78) {
      sleep_ms(1);
      if(millis() - start_time >= RESET_TIMEOUT_MS) {
        if(debug)
          printf("Timed out waiting for Reset!");
        return false;
      }
    }

    return true;
  }

  i2c_inst_t* IOExpander::get_i2c() const {
    return i2c->get_i2c();
  }

  int IOExpander::get_address() const {
    return address;
  }

  int IOExpander::get_sda() const {
    return i2c->get_sda();
  }

  int IOExpander::get_scl() const {
    return i2c->get_scl();
  }

  int IOExpander::get_int() const {
    return interrupt;
  }

  uint16_t IOExpander::get_chip_id() {
      return ((uint16_t)i2c->reg_read_uint8(address, reg::CHIP_ID_H) << 8) | (uint16_t)i2c->reg_read_uint8(address, reg::CHIP_ID_L);
  }

  void IOExpander::set_address(uint8_t address) {
    set_bit(reg::CTRL, 4);
    i2c->reg_write_uint8(this->address, reg::ADDR, address);
    this->address = address;
    sleep_ms(250); //TODO Handle addr change IOError better
    //wait_for_flash()
    clr_bit(reg::CTRL, 4);
  }

  float IOExpander::get_adc_vref() {
    return vref;
  }

  void IOExpander::set_adc_vref(float vref) {
    this->vref = vref;
  }

  void IOExpander::enable_interrupt_out(bool pin_swap) {
    set_bit(reg::INT, int_bit::OUT_EN);
    change_bit(reg::INT, int_bit::PIN_SWAP, pin_swap);
  }

  void IOExpander::disable_interrupt_out() {
    clr_bit(reg::INT, int_bit::OUT_EN);
  }

  bool IOExpander::get_interrupt_flag() {
    if(interrupt != PIN_UNUSED)
      return !gpio_get(interrupt);
    else
      return (get_bit(reg::INT, int_bit::TRIGD) != 0);
  }

  void IOExpander::clear_interrupt_flag() {
    clr_bit(reg::INT, int_bit::TRIGD);
  }

  bool IOExpander::set_pin_interrupt(uint8_t pin, bool enabled) {
    bool succeeded = false;
    if(pin >= 1 && pin <= NUM_PINS) {
      Pin& io_pin = pins[pin - 1];
      change_bit(io_pin.reg_int_mask_p, io_pin.pin, enabled);

      succeeded = true;
    }

    return succeeded;
  }

  void IOExpander::pwm_load(bool wait_for_load) {
    // Load new period and duty registers into buffer
    uint32_t start_time = millis();
    set_bit(reg::PWMCON0, 6);  // Set the "LOAD" bit of PWMCON0

    if(wait_for_load) {
      while(pwm_loading()) {
        sleep_ms(1); // Wait for "LOAD" to complete
        if(millis() - start_time >= timeout) {
          if(debug)
            printf("Timed out waiting for PWM load!");
          return;
        }
      }
    }
  }

  bool IOExpander::pwm_loading() {
    return get_bit(reg::PWMCON0, 6);
  }

  void IOExpander::pwm_clear(bool wait_for_clear) {
    uint32_t start_time = millis();
    set_bit(reg::PWMCON0, 4);  // Set the "CLRPWM" bit of PWMCON0
    if(wait_for_clear) {
      while(pwm_clearing()) {
        sleep_ms(1); // Wait for "CLRPWM" to complete
        if(millis() - start_time >= timeout) {
          if(debug)
            printf("Timed out waiting for PWM clear!");
          return;
        }
      }
    }
  }

  bool IOExpander::pwm_clearing() {
    return get_bit(reg::PWMCON0, 4);
  }

  bool IOExpander::set_pwm_control(uint8_t divider) {
    bool divider_good = true;
    uint8_t pwmdiv2 = 0;
    switch(divider) {
      case 1:   pwmdiv2 = 0b000;    break;
      case 2:   pwmdiv2 = 0b001;    break;
      case 4:   pwmdiv2 = 0b010;    break;
      case 8:   pwmdiv2 = 0b011;    break;
      case 16:  pwmdiv2 = 0b100;    break;
      case 32:  pwmdiv2 = 0b101;    break;
      case 64:  pwmdiv2 = 0b110;    break;
      case 128: pwmdiv2 = 0b111;    break;

      default:
        if(debug) {
          printf("ValueError: A clock divider of %d\n", divider);
        }
        divider_good = false;
        break;
    }

    if(divider_good) {
      //TODO: This currently sets GP, PWMTYP and FBINEN to 0
      // It might be desirable to make these available to the user
      // GP - Group mode enable (changes first three pairs of pAM to PWM01H and PWM01L)
      // PWMTYP - PWM type select: 0 edge-aligned, 1 center-aligned
      // FBINEN - Fault-break input enable

      i2c->reg_write_uint8(address, reg::PWMCON1, pwmdiv2);
    }

    return divider_good;
  }

  void IOExpander::set_pwm_period(uint16_t value, bool load, bool wait_for_load) {
    value &= 0xffff;
    i2c->reg_write_uint8(address, reg::PWMPL, (uint8_t)(value & 0xff));
    i2c->reg_write_uint8(address, reg::PWMPH, (uint8_t)(value >> 8));

    if(load)
      pwm_load(wait_for_load);
  }

  uint16_t IOExpander::set_pwm_frequency(float frequency, bool load, bool wait_for_load) {
    uint32_t period = (uint32_t)(CLOCK_FREQ / frequency);
    if (period / 128 > MAX_PERIOD) {
      return MAX_PERIOD;
    }
    if (period < 2) {
      return 2;
    }

    uint8_t divider = 1;
    while ((period > MAX_PERIOD) && (divider < MAX_DIVIDER)) {
      period >>= 1;
      divider <<= 1;
    }

    period = MIN(period, MAX_PERIOD); // Should be unnecessary because of earlier raised errors, but kept in case
    set_pwm_control(divider);
    set_pwm_period((uint16_t)(period - 1), load, wait_for_load);

    return (uint16_t)period;
  }

  uint8_t IOExpander::get_mode(uint8_t pin) {
    if(pin < 1 || pin > NUM_PINS) {
      printf("ValueError: Pin should be in range 1-14.\n");
      return UINT8_MAX;
    }

    return pins[pin - 1].get_mode();
  }

  void IOExpander::set_mode(uint8_t pin, uint8_t mode, bool schmitt_trigger, bool invert) {
    if(pin < 1 || pin > NUM_PINS) {
      printf("ValueError: Pin should be in range 1-14.\n");
      return;
    }

    Pin& io_pin = pins[pin - 1];

    uint8_t gpio_mode = mode & 0b11;
    uint8_t io_type = (mode >> 2) & 0b11;
    uint8_t initial_state = mode >> 4;

    if(io_pin.get_mode() == mode) {
      if(debug) {
        printf("Mode already is %s\n", MODE_NAMES[io_type]);
      }
      return;
    }

    if((io_type != Pin::TYPE_IO) && !io_pin.mode_supported(mode)) {
      if(debug) {
        printf("Pin %d does not support %s!\n", pin, MODE_NAMES[io_type]);
      }
      return;
    }

    io_pin.set_mode(mode);
    if(debug) {
      printf("Setting pin %d to mode %s %s, state: %s\n", pin, MODE_NAMES[io_type], GPIO_NAMES[gpio_mode], STATE_NAMES[initial_state]);
    }

    if(mode == PIN_MODE_PWM) {
      set_bit(io_pin.reg_io_pwm, io_pin.pwm_channel);
      change_bit(reg::PNP, io_pin.pwm_channel, invert);
      set_bit(reg::PWMCON0, 7);  // Set PWMRUN bit
    }
    else {
      if(io_pin.get_type() & Pin::TYPE_PWM)
        clr_bit(io_pin.reg_io_pwm, io_pin.pwm_channel);
    }

    uint8_t pm1 = i2c->reg_read_uint8(address, io_pin.reg_m1);
    uint8_t pm2 = i2c->reg_read_uint8(address, io_pin.reg_m2);

    // Clear the pm1 and pm2 bits
    pm1 &= 255 - (1 << io_pin.pin);
    pm2 &= 255 - (1 << io_pin.pin);

    // Set the new pm1 and pm2 bits according to our gpio_mode
    pm1 |= (gpio_mode >> 1) << io_pin.pin;
    pm2 |= (gpio_mode & 0b1) << io_pin.pin;

    i2c->reg_write_uint8(address, io_pin.reg_m1, pm1);
    i2c->reg_write_uint8(address, io_pin.reg_m2, pm2);

    // Set up Schmitt trigger mode on inputs
    if(mode == PIN_MODE_PU || mode == PIN_MODE_IN)
      change_bit(io_pin.reg_ps, io_pin.pin, schmitt_trigger);

    // 5th bit of mode encodes default output pin state
    i2c->reg_write_uint8(address, io_pin.reg_p, (initial_state << 3) | io_pin.pin);
  }

  int16_t IOExpander::input(uint8_t pin, uint32_t adc_timeout) {
    if(pin < 1 || pin > NUM_PINS) {
      if(debug)
        printf("ValueError: Pin should be in range 1-14.\n");
      return -1;
    }

    Pin& io_pin = pins[pin - 1];

    if(io_pin.get_mode() == PIN_MODE_ADC) {
      if(debug) {
        printf("Reading ADC from pin %d\n", pin);
      }

      clr_bits(reg::ADCCON0, 0x0f);
      set_bits(reg::ADCCON0, io_pin.adc_channel);
      i2c->reg_write_uint8(address, reg::AINDIDS, 0);
      set_bit(reg::AINDIDS, io_pin.adc_channel);
      set_bit(reg::ADCCON1, 0);

      clr_bit(reg::ADCCON0, 7);  // ADCF - Clear the conversion complete flag
      set_bit(reg::ADCCON0, 6);  // ADCS - Set the ADC conversion start flag

      // Wait for the ADCF conversion complete flag to be set
      unsigned long start_time = millis();
      while(!get_bit(reg::ADCCON0, 7)) {
        sleep_ms(10);
        if(millis() - start_time >= adc_timeout) {
          if(debug)
            printf("Timeout waiting for ADC conversion!");
          return -1;
        }
      }

      uint8_t hi = i2c->reg_read_uint8(address, reg::ADCRH);
      uint8_t lo = i2c->reg_read_uint8(address, reg::ADCRL);
      return (uint16_t)(hi << 4) | (uint16_t)lo;
    }
    else {
      if(debug) {
        printf("Reading IO from pin %d\n", pin);
      }

      uint8_t pv = get_bit(io_pin.reg_p, io_pin.pin);
      return (pv) ? 1 : 0;
    }
  }

  float IOExpander::input_as_voltage(uint8_t pin, uint32_t adc_timeout) {
    if(pin < 1 || pin > NUM_PINS) {
      if(debug)
        printf("ValueError: Pin should be in range 1-14.\n");
      return -1;
    }

    Pin& io_pin = pins[pin - 1];

    if(io_pin.get_mode() == PIN_MODE_ADC) {
      if(debug) {
        printf("Reading ADC from pin %d\n", pin);
      }

      clr_bits(reg::ADCCON0, 0x0f);
      set_bits(reg::ADCCON0, io_pin.adc_channel);
      i2c->reg_write_uint8(address, reg::AINDIDS, 0);
      set_bit(reg::AINDIDS, io_pin.adc_channel);
      set_bit(reg::ADCCON1, 0);


      clr_bit(reg::ADCCON0, 7);  // ADCF - Clear the conversion complete flag
      set_bit(reg::ADCCON0, 6);  // ADCS - Set the ADC conversion start flag

      // Wait for the ADCF conversion complete flag to be set
      unsigned long start_time = millis();
      while(!get_bit(reg::ADCCON0, 7)) {
        sleep_ms(1);
        if(millis() - start_time >= adc_timeout) {
          if(debug)
            printf("Timeout waiting for ADC conversion!\n");
          return -1;
        }
      }

      uint8_t hi = i2c->reg_read_uint8(address, reg::ADCRH);
      uint8_t lo = i2c->reg_read_uint8(address, reg::ADCRL);
      return ((float)((uint16_t)(hi << 4) | (uint16_t)lo) / 4095.0f) * vref;
    }
    else {
      if(debug) {
        printf("Reading IO from pin %d\n", pin);
      }

      uint8_t pv = get_bit(io_pin.reg_p, io_pin.pin);
      return (pv) ? vref : 0.0f;
    }
  }

  void IOExpander::output(uint8_t pin, uint16_t value, bool load, bool wait_for_load) {
    if(pin < 1 || pin > NUM_PINS) {
      printf("Pin should be in range 1-14.");
      return;
    }

    Pin& io_pin = pins[pin - 1];

    if(io_pin.get_mode() == PIN_MODE_PWM) {
      if(debug) {
        printf("Outputting PWM to pin: %d\n", pin);
      }

      i2c->reg_write_uint8(address, io_pin.reg_pwml, (uint8_t)(value & 0xff));
      i2c->reg_write_uint8(address, io_pin.reg_pwmh, (uint8_t)(value >> 8));
      if(load)
        pwm_load(wait_for_load);
    }
    else {
      if(value == LOW) {
        if(debug) {
          printf("Outputting LOW to pin: %d\n", pin);
        }

        clr_bit(io_pin.reg_p, io_pin.pin);
      }
      else if(value == HIGH) {
        if(debug) {
          printf("Outputting HIGH to pin: %d\n", pin);
        }

        set_bit(io_pin.reg_p, io_pin.pin);
      }
    }
  }

  void IOExpander::setup_rotary_encoder(uint8_t channel, uint8_t pin_a, uint8_t pin_b, uint8_t pin_c, bool count_microsteps) {
    channel -= 1;
    set_mode(pin_a, PIN_MODE_PU, true);
    set_mode(pin_b, PIN_MODE_PU, true);

    if(pin_c != 0) {
      set_mode(pin_c, PIN_MODE_OD);
      output(pin_c, 0);
    }

    i2c->reg_write_uint8(address, ENC_CFG[channel], pin_a | (pin_b << 4));
    change_bit(reg::ENC_EN, (channel * 2) + 1, count_microsteps);
    set_bit(reg::ENC_EN, channel * 2);

    // Reset internal encoder count to zero
    uint8_t reg = ENC_COUNT[channel];
    i2c->reg_write_uint8(address, reg, 0x00);
  }

  int16_t IOExpander::read_rotary_encoder(uint8_t channel) {
    channel -= 1;
    int16_t last = encoder_last[channel];
    uint8_t reg = ENC_COUNT[channel];
    int16_t value = (int16_t)i2c->reg_read_uint8(address, reg);

    if(value & 0b10000000)
      value -= 256;

    if(last > 64 && value < -64)
      encoder_offset[channel] += 256;
    if(last < -64 && value > 64)
      encoder_offset[channel] -= 256;

    encoder_last[channel] = value;

    return encoder_offset[channel] + value;
  }

  void IOExpander::clear_rotary_encoder(uint8_t channel) {
    channel -= 1;
    encoder_last[channel] = 0;
    uint8_t reg = ENC_COUNT[channel];
    i2c->reg_write_uint8(address, reg, 0);
  }

  uint8_t IOExpander::get_bit(uint8_t reg, uint8_t bit) {
    // Returns the specified bit (nth position from right) from a register
    return i2c->reg_read_uint8(address, reg) & (1 << bit);
  }

  void IOExpander::set_bits(uint8_t reg, uint8_t bits) {
    if(reg == reg::P0 || reg == reg::P1 || reg == reg::P2 || reg == reg::P3) {
        for(uint8_t bit = 0; bit < 8; bit++) {
          if(bits & (1 << bit)) {
            i2c->reg_write_uint8(address, reg, 0b1000 | (bit & 0b111));
            sleep_us(50);
          }
        }
        return;
    }

    uint8_t value = i2c->reg_read_uint8(address, reg);
    sleep_us(50);
    i2c->reg_write_uint8(address, reg, value | bits);
  }

  void IOExpander::set_bit(uint8_t reg, uint8_t bit) {
    // Set the specified bit (nth position from right) in a register.
    set_bits(reg, (1 << bit));
  }

  void IOExpander::clr_bits(uint8_t reg, uint8_t bits) {
    if(reg == reg::P0 || reg == reg::P1 || reg == reg::P2 || reg == reg::P3) {
      for(uint8_t bit = 0; bit < 8; bit++) {
        if(bits & (1 << bit)) {
          i2c->reg_write_uint8(address, reg, 0b0000 | (bit & 0b111));
          sleep_us(50);
        }
      }
      return;
    }

    // Now deal with any other registers
    uint8_t value = i2c->reg_read_uint8(address, reg);
    sleep_us(50);
    i2c->reg_write_uint8(address, reg, value & ~bits);
  }

  void IOExpander::clr_bit(uint8_t reg, uint8_t bit) {
    // Clear the specified bit (nth position from right) in a register.
    clr_bits(reg, (1 << bit));
  }

  void IOExpander::change_bit(uint8_t reg, uint8_t bit, bool state) {
    // Toggle one register bit on/off.
    if(state)
      set_bit(reg, bit);
    else
      clr_bit(reg, bit);
  }

  void IOExpander::wait_for_flash(void) {
    // Wait for the IOE to finish writing non-volatile memory.
    unsigned long start_time = millis();
    while(get_interrupt_flag()) {
      if(millis() - start_time > timeout) {
        printf("Timed out waiting for interrupt!\n");
        return;
      }
      sleep_ms(1);
    }

    start_time = millis();
    while(!get_interrupt_flag()) {
      if(millis() - start_time > timeout) {
        printf("Timed out waiting for interrupt!\n");
        return;
      }
      sleep_ms(1);
    }
  }

  uint32_t IOExpander::millis() {
    return to_ms_since_boot(get_absolute_time());
  }
}