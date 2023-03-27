#include "ltr559.hpp"
#include <algorithm>

namespace pimoroni {
  bool LTR559::init() {
    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_pull_up(interrupt);
    }

    reset();
    interrupts(true, true);

    // 50mA, 100% duty cycle, 30Hz, 1 pulse
    proximity_led(50, 100, 30, 1);

    // enabled, gain 4x
    light_control(true, 4);

    // enabled, saturation indicator enabled
    proximity_control(true, true);

    // 100ms measurement rate
    proximity_measurement_rate(100);

    // 50ms integration time and repeat rate
    light_measurement_rate(50, 50);

    light_threshold(0xFFFF, 0x0000);
    proximity_threshold(0x7FFF, 0x7FFF);
    proximity_offset(0);

    return true;
  }

  void LTR559::reset() {
    i2c->set_bits(address, LTR559_ALS_CONTROL, LTR559_ALS_CONTROL_SW_RESET_BIT);

    while(i2c->get_bits(address, LTR559_ALS_CONTROL, LTR559_ALS_CONTROL_SW_RESET_BIT)) {
      sleep_ms(100);
    }
  }

  i2c_inst_t* LTR559::get_i2c() const {
    return i2c->get_i2c();
  }

  int LTR559::get_address() const {
    return address;
  }

  int LTR559::get_sda() const {
    return i2c->get_sda();
  }

  int LTR559::get_scl() const {
    return i2c->get_sda();
  }

  int LTR559::get_int() const {
    return interrupt;
  }


  uint8_t LTR559::part_id() {
    uint8_t part_id;
    i2c->read_bytes(address, LTR559_PART_ID, &part_id, 1);
    return (part_id >> LTR559_PART_ID_PART_NUMBER_SHIFT) & LTR559_PART_ID_PART_NUMBER_MASK;
  }

  uint8_t LTR559::revision_id() {
    uint8_t revision_id;
    i2c->read_bytes(address, LTR559_PART_ID, &revision_id, 1);
    return revision_id & LTR559_PART_ID_REVISION_MASK;
  }

  uint8_t LTR559::manufacturer_id() {
    uint8_t manufacturer;
    i2c->read_bytes(address, LTR559_MANUFACTURER_ID, &manufacturer, 1);
    return manufacturer;
  }

  bool LTR559::get_reading() {
    bool has_updated = false;
    uint8_t status;
    i2c->read_bytes(address, LTR559_ALS_PS_STATUS, &status, 1);
    bool als_int = (status >> LTR559_ALS_PS_STATUS_ALS_INTERRUPT_BIT) & 0b1;
    bool ps_int = (status >> LTR559_ALS_PS_STATUS_PS_INTERRUPT_BIT) & 0b1;
    bool als_data = (status >> LTR559_ALS_PS_STATUS_ALS_DATA_BIT) & 0b1;
    bool ps_data = (status >> LTR559_ALS_PS_STATUS_PS_DATA_BIT) & 0b1;

    if(ps_int || ps_data) {
      has_updated = true;
      uint16_t ps0;
      i2c->read_bytes(address, LTR559_PS_DATA, (uint8_t *)&ps0, 2);
      ps0 &= LTR559_PS_DATA_MASK;

      data.proximity = ps0;
    }

    if(als_int || als_data) {
      has_updated = true;
      uint16_t als[2];
      i2c->read_bytes(address, LTR559_ALS_DATA_CH1, (uint8_t *)&als, 4);
      data.als0 = als[1];
      data.als1 = als[0];
      data.gain = lookup_light_gain[(status >> LTR559_ALS_PS_STATUS_ALS_GAIN_SHIFT) & LTR559_ALS_PS_STATUS_ALS_GAIN_MASK];

      data.ratio = 101.0f;

      if((uint32_t)data.als0 + data.als1 > 0) {
        data.ratio = (float)data.als1 * 100.0f / ((float)data.als1 + data.als0);
      }

      uint8_t ch_idx = 3;
      if(this->data.ratio < 45)
        ch_idx = 0;
      else if(data.ratio < 64)
        ch_idx = 1;
      else if (data.ratio < 85)
        ch_idx = 2;

      float lux = ((int32_t)data.als0 * ch0_c[ch_idx]) - ((int32_t)data.als1 * ch1_c[ch_idx]);
      lux /= (float)this->data.integration_time / 100.0f;
      lux /= (float)this->data.gain;
      data.lux = lux / 10000.0f;
    }

    return has_updated;
  }

  void LTR559::interrupts(bool light, bool proximity) {
    uint8_t buf = 0;
    buf |= 0b1 << LTR559_INTERRUPT_POLARITY_BIT;
    buf |= (uint8_t)light << LTR559_INTERRUPT_ALS_BIT;
    buf |= (uint8_t)proximity << LTR559_INTERRUPT_PS_BIT;
    i2c->write_bytes(address, LTR559_INTERRUPT, &buf, 1);
  }

  void LTR559::proximity_led(uint8_t current, uint8_t duty_cycle, uint8_t pulse_freq, uint8_t num_pulses) {
    current = lookup<lookup_led_current>(current);

    duty_cycle = lookup<lookup_led_duty_cycle>(duty_cycle);
    duty_cycle <<= LTR559_PS_LED_DUTY_CYCLE_SHIFT; 

    pulse_freq = lookup<lookup_led_pulse_freq>(pulse_freq);
    pulse_freq <<= LTR559_PS_LED_PULSE_FREQ_SHIFT;

    uint8_t buf = current | duty_cycle | pulse_freq;
    i2c->write_bytes(address, LTR559_PS_LED, &buf, 1);

    buf = num_pulses & LTR559_PS_N_PULSES_MASK;
    i2c->write_bytes(address, LTR559_PS_N_PULSES, &buf, 1);
  }

  void LTR559::light_control(bool active, uint8_t gain) {
    uint8_t buf = 0;
    gain = lookup<lookup_light_gain>(gain);
    buf |= gain << LTR559_ALS_CONTROL_GAIN_SHIFT;

    if(active)
      buf |= (0b1 << LTR559_ALS_CONTROL_MODE_BIT);
    else
      buf &= ~(0b1 << LTR559_ALS_CONTROL_MODE_BIT);

    i2c->write_bytes(address, LTR559_ALS_CONTROL, &buf, 1);
  }

  void LTR559::proximity_control(bool active, bool saturation_indicator) {
    uint8_t buf = 0;
    i2c->read_bytes(address, LTR559_PS_CONTROL, &buf, 1);
    if(active)
      buf |= LTR559_PS_CONTROL_ACTIVE_MASK;
    else
      buf &= ~LTR559_PS_CONTROL_ACTIVE_MASK;
    
    if(saturation_indicator)
      buf |= 0b1 << LTR559_PS_CONTROL_SATURATION_INDICATOR_ENABLE_BIT;
    else
      buf &= ~(0b1 << LTR559_PS_CONTROL_SATURATION_INDICATOR_ENABLE_BIT);
    
    i2c->write_bytes(address, LTR559_PS_CONTROL, &buf, 1);
  }

  void LTR559::light_threshold(uint16_t lower, uint16_t upper) {
    lower = __builtin_bswap16(lower);
    upper = __builtin_bswap16(upper);
    i2c->write_bytes(address, LTR559_ALS_THRESHOLD_LOWER, (uint8_t *)&lower, 2);
    i2c->write_bytes(address, LTR559_ALS_THRESHOLD_UPPER, (uint8_t *)&upper, 2);
  }

  void LTR559::proximity_threshold(uint16_t lower, uint16_t upper) {
    lower = uint16_to_bit12(lower);
    upper = uint16_to_bit12(upper);
    i2c->write_bytes(address, LTR559_PS_THRESHOLD_LOWER, (uint8_t *)&lower, 2);
    i2c->write_bytes(address, LTR559_PS_THRESHOLD_UPPER, (uint8_t *)&upper, 2); 
  }

  void LTR559::light_measurement_rate(uint16_t integration_time, uint16_t rate) {
    data.integration_time = integration_time;
    integration_time = lookup<lookup_light_integration_time>(integration_time);
    rate = lookup<lookup_light_repeat_rate>(rate);
    uint8_t buf = 0;
    buf |= rate;
    buf |= integration_time << LTR559_ALS_MEAS_RATE_INTEGRATION_TIME_SHIFT;
    i2c->write_bytes(address, LTR559_ALS_MEAS_RATE, &buf, 1);
  }

  void LTR559::proximity_measurement_rate(uint16_t rate) {
    uint8_t buf = lookup<lookup_proximity_meas_rate>(rate);
    i2c->write_bytes(address, LTR559_PS_MEAS_RATE, &buf, 1);
  }

  void LTR559::proximity_offset(uint16_t offset) {
    offset &= LTR559_PS_OFFSET_MASK;
    i2c->write_bytes(address, LTR559_PS_OFFSET, (uint8_t *)&offset, 1);
  }

  uint16_t LTR559::bit12_to_uint16(uint16_t value) {
    return ((value & 0xFF00) >> 8) | ((value & 0x000F) << 8);
  }

  uint16_t LTR559::uint16_to_bit12(uint16_t value) {
    return ((value & 0xFF) << 8) | ((value & 0xF00) >> 8);
  }
}