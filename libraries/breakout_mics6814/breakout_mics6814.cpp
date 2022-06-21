#include "breakout_mics6814.hpp"
#include <algorithm>

namespace pimoroni {

  bool BreakoutMICS6814::init(bool skip_chip_id_check) {
    bool success = false;
    if(ioe.init(skip_chip_id_check)) {

      ioe.set_mode(MICS_VREF, IOExpander::PIN_ADC);
      ioe.set_mode(MICS_RED, IOExpander::PIN_ADC);
      ioe.set_mode(MICS_NH3, IOExpander::PIN_ADC);
      ioe.set_mode(MICS_OX, IOExpander::PIN_ADC);

      ioe.set_mode(MICS_HEATER_EN, IOExpander::PIN_OD);
      ioe.output(MICS_HEATER_EN, IOExpander::LOW);

      // Calculate a period large enough to get 0-255 steps at the desired brightness
      uint16_t period = (uint16_t)(255.0f / brightness);

      ioe.set_pwm_period(period);
      ioe.set_pwm_control(2);     // PWM as fast as we can to avoid LED flicker

      ioe.set_mode(LED_R, IOExpander::PIN_PWM, false, INVERT_OUTPUT);
      ioe.set_mode(LED_G, IOExpander::PIN_PWM, false, INVERT_OUTPUT);
      ioe.set_mode(LED_B, IOExpander::PIN_PWM, false, INVERT_OUTPUT);

      success = true;
    }

    return success;
  }

  i2c_inst_t* BreakoutMICS6814::get_i2c() const {
    return ioe.get_i2c();
  }

  int BreakoutMICS6814::get_address() const {
    return ioe.get_address();
  }

  int BreakoutMICS6814::get_sda() const {
    return ioe.get_sda();
  }

  int BreakoutMICS6814::get_scl() const {
    return ioe.get_scl();
  }

  int BreakoutMICS6814::get_int() const {
    return ioe.get_int();
  }

  void BreakoutMICS6814::set_address(uint8_t address) {
    ioe.set_address(address);
  }

  float BreakoutMICS6814::get_adc_vref(void) {
    return ioe.get_adc_vref();
  }

  void BreakoutMICS6814::set_adc_vref(float vref) {
    ioe.set_adc_vref(vref);
  }

  void BreakoutMICS6814::set_brightness(float brightness) {
    this->brightness = std::min(std::max(brightness, 0.01f), 1.0f);
    
    // Calculate a period large enough to get 0-255 steps at the desired brightness
    uint16_t period = (uint16_t)(255.0f / this->brightness);
    
    ioe.set_pwm_period(period);
  }

  void BreakoutMICS6814::set_led(uint8_t r, uint8_t g, uint8_t b) {
    ioe.output(LED_R, r, false);    // Hold off pwm load until the last
    ioe.output(LED_G, g, false);    // Hold off pwm load until the last
    ioe.output(LED_B, b);           // Loads all 3 pwms
  }

  void BreakoutMICS6814::set_heater(bool on) {
    ioe.output(MICS_HEATER_EN, on ? IOExpander::LOW : IOExpander::HIGH);
  }

  void BreakoutMICS6814::disable_heater() {
    ioe.output(MICS_HEATER_EN, IOExpander::HIGH);
    ioe.set_mode(MICS_HEATER_EN, IOExpander::PIN_IN);
  }

  float BreakoutMICS6814::get_raw_ref(uint32_t adc_timeout) {
    return ioe.input_as_voltage(MICS_VREF, adc_timeout);
  }

  float BreakoutMICS6814::get_raw_red(uint32_t adc_timeout) {
    return ioe.input_as_voltage(MICS_RED, adc_timeout);
  }

  float BreakoutMICS6814::get_raw_nh3(uint32_t adc_timeout) {
    return ioe.input_as_voltage(MICS_NH3, adc_timeout);
  }

  float BreakoutMICS6814::get_raw_oxd(uint32_t adc_timeout) {
    return ioe.input_as_voltage(MICS_OX, adc_timeout);
  }

  BreakoutMICS6814::Reading BreakoutMICS6814::read_all(uint32_t adc_timeout) {
    BreakoutMICS6814::Reading reading;
    reading.reducing = read_reducing(adc_timeout);
    reading.nh3 = read_nh3(adc_timeout);
    reading.oxidising = read_oxidising(adc_timeout);
    reading.ref = read_ref(adc_timeout);
    return reading;
  }

  float BreakoutMICS6814::read_ref(uint32_t adc_timeout) {
    float ref = get_raw_ref(adc_timeout);
    if(ref == -1)
      ref = 0;

    return ref;
  }

  float BreakoutMICS6814::read_reducing(uint32_t adc_timeout) {
    float vref = ioe.get_adc_vref();

    float red = get_raw_red(adc_timeout);
    if((red != -1) && (vref != red))
      red = (red * 56000.0f) / (vref - red);
    else
      red = 0;

    return red;
  }

  float BreakoutMICS6814::read_nh3(uint32_t adc_timeout) {
    float vref = ioe.get_adc_vref();

    float nh3 = get_raw_nh3(adc_timeout);
    if((nh3 != -1) && (vref != nh3))
      nh3 = (nh3 * 56000.0f) / (vref - nh3);
    else
      nh3 = 0;

    return nh3;
  }

  float BreakoutMICS6814::read_oxidising(uint32_t adc_timeout) {
    float vref = ioe.get_adc_vref();

    float oxd = get_raw_oxd(adc_timeout);
    if((oxd != -1) && (vref != oxd))
      oxd = (oxd * 56000.0f) / (vref - oxd);
    else
      oxd = 0;

    return oxd;
  }
}
