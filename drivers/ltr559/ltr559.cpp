#include "ltr559.hpp"
#include <algorithm>

namespace pimoroni {
  lookup::lookup(std::initializer_list<uint16_t> values) : lut(values) {
  }

  uint8_t lookup::index(uint16_t value) {
    auto it = find(this->lut.begin(), this->lut.end(), value);

    if(it == this->lut.end()) return 0;

    return it - this->lut.begin();
  }

  uint16_t lookup::value(uint8_t index) {
    return lut[index];
  }

  pimoroni::lookup LTR559::lookup_led_current({5, 10, 20, 50, 100});
  pimoroni::lookup LTR559::lookup_led_duty_cycle({25, 50, 75, 100});
  pimoroni::lookup LTR559::lookup_led_pulse_freq({30, 40, 50, 60, 70, 80, 90, 100});
  pimoroni::lookup LTR559::lookup_proximity_meas_rate({10, 50, 70, 100, 200, 500, 1000, 2000});
  pimoroni::lookup LTR559::lookup_light_integration_time({100, 50, 200, 400, 150, 250, 300, 350});
  pimoroni::lookup LTR559::lookup_light_repeat_rate({50, 100, 200, 500, 1000, 2000});
  pimoroni::lookup LTR559::lookup_light_gain({1, 2, 4, 8, 0, 0, 48, 96});
  
  int LTR559::init() {
    i2c_init(i2c, 400000);

    gpio_set_function(sda, GPIO_FUNC_I2C); gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C); gpio_pull_up(scl);

    this->reset();
    this->interrupts(true, true);

    // 50mA, 1.0 duty cycle, 30Hz, 1 pulse
    this->proximity_led(50, 1.0, 30, 1);

    // enabled, gain 4x
    this->light_control(true, 4);

    // enabled, saturation indicator enabled
    this->proximity_control(true, true);

    // 100ms measurement rate
    this->proximity_measurement_rate(100);

     // 50ms integration time and repeat rate
    this->light_measurement_rate(50, 50);

    this->light_threshold(0xFFFF, 0x0000);
    this->proximity_threshold(0x7FFF, 0x7FFF);
    this->proximity_offset(0);

    return 0;
  }

  uint8_t LTR559::part_id() {
    uint8_t part_id;
    this->read_bytes(LTR559_PART_ID, &part_id, 1);
    return (part_id >> LTR559_PART_ID_PART_NUMBER_SHIFT) & LTR559_PART_ID_PART_NUMBER_MASK;
  }

  uint8_t LTR559::revision_id() {
    uint8_t revision_id;
    this->read_bytes(LTR559_PART_ID, &revision_id, 1);
    return revision_id & LTR559_PART_ID_REVISION_MASK;
  }

  uint8_t LTR559::manufacturer_id() {
    uint8_t manufacturer;
    this->read_bytes(LTR559_MANUFACTURER_ID, &manufacturer, 1);
    return manufacturer;
  }

  void LTR559::reset() {
    this->set_bits(LTR559_ALS_CONTROL, LTR559_ALS_CONTROL_SW_RESET_BIT);

    while (this->get_bits(LTR559_ALS_CONTROL, LTR559_ALS_CONTROL_SW_RESET_BIT)) {
      sleep_ms(100);
    }
  }

  uint16_t LTR559::bit12_to_uint16(uint16_t value) {
    return ((value & 0xFF00) >> 8) | ((value & 0x000F) << 8);
  }

  uint16_t LTR559::uint16_to_bit12(uint16_t value) {
    return ((value & 0xFF) << 8) | ((value & 0xF00) >> 8);
  }

  bool LTR559::get_reading() {
    bool has_updated = false;
    uint8_t status;
    this->read_bytes(LTR559_ALS_PS_STATUS, &status, 1);
    bool als_int = (status >> LTR559_ALS_PS_STATUS_ALS_INTERRUPT_BIT) & 0b1;
    bool ps_int = (status >> LTR559_ALS_PS_STATUS_PS_INTERRUPT_BIT) & 0b1;
    bool als_data = (status >> LTR559_ALS_PS_STATUS_ALS_DATA_BIT) & 0b1;
    bool ps_data = (status >> LTR559_ALS_PS_STATUS_PS_DATA_BIT) & 0b1;

    if (ps_int || ps_data) {
      has_updated = true;
      uint16_t ps0;
      this->read_bytes(LTR559_PS_DATA, (uint8_t *)&ps0, 2);
      ps0 &= LTR559_PS_DATA_MASK;

      this->data.proximity = ps0;
    }

    if (als_int || als_data) {
      has_updated = true;
      uint16_t als[2];
      this->read_bytes(LTR559_ALS_DATA_CH1, (uint8_t *)&als, 4);
      this->data.als0 = als[1];
      this->data.als1 = als[0];
      this->data.gain = this->lookup_light_gain.value((status >> LTR559_ALS_PS_STATUS_ALS_GAIN_SHIFT) & LTR559_ALS_PS_STATUS_ALS_GAIN_MASK);

      this->data.ratio = 101.0f;

      if ((uint32_t)this->data.als0 + this->data.als1 > 0) {
        this->data.ratio = (float)this->data.als1 * 100.0f / ((float)this->data.als1 + this->data.als0);
      }

      uint8_t ch_idx = 3;
      if (this->data.ratio < 45) {
        ch_idx = 0;
      } else if (this->data.ratio < 64) {
        ch_idx = 1;
      } else if (this->data.ratio < 85) {
        ch_idx = 2;
      }

      float lux = ((int32_t)this->data.als0 * this->ch0_c[ch_idx]) - ((int32_t)this->data.als1 * this->ch1_c[ch_idx]);
      lux /= (float)this->data.integration_time / 100.0f;
      lux /= (float)this->data.gain;
      this->data.lux = (uint16_t)(lux / 10000.0f);
    }

    return has_updated;
  }

  void LTR559::interrupts(bool light, bool proximity) {
    uint8_t buf = 0;
    buf |= 0b1 << LTR559_INTERRUPT_POLARITY_BIT;
    buf |= (uint8_t)light << LTR559_INTERRUPT_ALS_BIT;
    buf |= (uint8_t)proximity << LTR559_INTERRUPT_PS_BIT;
    this->write_bytes(LTR559_INTERRUPT, &buf, 1);
  }

  void LTR559::proximity_led(uint8_t current, uint8_t duty_cycle, uint8_t pulse_freq, uint8_t num_pulses) {
    current = this->lookup_led_current.index(current);

    duty_cycle = this->lookup_led_duty_cycle.index(duty_cycle);
    duty_cycle <<= LTR559_PS_LED_DUTY_CYCLE_SHIFT; 

    pulse_freq = this->lookup_led_pulse_freq.index(pulse_freq);
    pulse_freq <<= LTR559_PS_LED_PULSE_FREQ_SHIFT;

    uint8_t buf = current | duty_cycle | pulse_freq;
    this->write_bytes(LTR559_PS_LED, &buf, 1);

    buf = num_pulses & LTR559_PS_N_PULSES_MASK;
    this->write_bytes(LTR559_PS_N_PULSES, &buf, 1);
  }

  void LTR559::light_control(bool active, uint8_t gain) {
    uint8_t buf = 0;
    gain = this->lookup_light_gain.index(gain);
    buf |= gain << LTR559_ALS_CONTROL_GAIN_SHIFT;

    if(active) {
      buf |= (0b1 << LTR559_ALS_CONTROL_MODE_BIT);
    } else {
      buf &= ~(0b1 << LTR559_ALS_CONTROL_MODE_BIT);
    }

    this->write_bytes(LTR559_ALS_CONTROL, &buf, 1);
  }

  void LTR559::proximity_control(bool active, bool saturation_indicator) {
    uint8_t buf = 0;
    this->read_bytes(LTR559_PS_CONTROL, &buf, 1);
    if(active) {
      buf |= LTR559_PS_CONTROL_ACTIVE_MASK;
    } else {
      buf &= ~LTR559_PS_CONTROL_ACTIVE_MASK;
    }
    if(saturation_indicator) {
      buf |= 0b1 << LTR559_PS_CONTROL_SATURATION_INDICATOR_ENABLE_BIT;
    } else {
      buf &= ~(0b1 << LTR559_PS_CONTROL_SATURATION_INDICATOR_ENABLE_BIT);
    }
    this->write_bytes(LTR559_PS_CONTROL, &buf, 1);
  }

  void LTR559::light_threshold(uint16_t lower, uint16_t upper) {
    lower = __builtin_bswap16(lower);
    upper = __builtin_bswap16(upper);
    this->write_bytes(LTR559_ALS_THRESHOLD_LOWER, (uint8_t *)&lower, 2);
    this->write_bytes(LTR559_ALS_THRESHOLD_UPPER, (uint8_t *)&upper, 2);
  }

  void LTR559::proximity_threshold(uint16_t lower, uint16_t upper) {
    lower = this->uint16_to_bit12(lower);
    upper = this->uint16_to_bit12(upper);
    this->write_bytes(LTR559_PS_THRESHOLD_LOWER, (uint8_t *)&lower, 2);
    this->write_bytes(LTR559_PS_THRESHOLD_UPPER, (uint8_t *)&upper, 2); 
  }

  void LTR559::light_measurement_rate(uint16_t integration_time, uint16_t rate) {
    this->data.integration_time = integration_time;
    integration_time = this->lookup_light_integration_time.index(integration_time);
    rate = this->lookup_light_repeat_rate.index(rate);
    uint8_t buf = 0;
    buf |= rate;
    buf |= integration_time << LTR559_ALS_MEAS_RATE_INTEGRATION_TIME_SHIFT;
    this->write_bytes(LTR559_ALS_MEAS_RATE, &buf, 1);
  }

  void LTR559::proximity_measurement_rate(uint16_t rate) {
    uint8_t buf = this->lookup_proximity_meas_rate.index(rate);
    this->write_bytes(LTR559_PS_MEAS_RATE, &buf, 1);
  }

  void LTR559::proximity_offset(uint16_t offset) {
    offset &= LTR559_PS_OFFSET_MASK;
    this->write_bytes(LTR559_PS_OFFSET, (uint8_t *)&offset, 1);
  }

  // i2c functions

  int LTR559::write_bytes(uint8_t reg, uint8_t *buf, int len) {
    uint8_t buffer[len + 1];
    buffer[0] = reg;
    for(int x = 0; x < len; x++) {
      buffer[x + 1] = buf[x];
    }
    return i2c_write_blocking(this->i2c, this->address, buffer, len + 1, false);
  };

  int LTR559::read_bytes(uint8_t reg, uint8_t *buf, int len) {
    i2c_write_blocking(this->i2c, this->address, &reg, 1, true);
    i2c_read_blocking(this->i2c, this->address, buf, len, false);
    return len;
  };

  uint8_t LTR559::get_bits(uint8_t reg, uint8_t shift, uint8_t mask) {
    uint8_t value;
    this->read_bytes(reg, &value, 1);
    return value & (mask << shift);
  }

  void LTR559::set_bits(uint8_t reg, uint8_t shift, uint8_t mask) {
    uint8_t value;
    this->read_bytes(reg, &value, 1);
    value |= mask << shift;
    this->write_bytes(reg, &value, 1);
  }

  void LTR559::clear_bits(uint8_t reg, uint8_t shift, uint8_t mask) {
    uint8_t value;
    this->read_bytes(reg, &value, 1);
    value &= ~(mask << shift);
    this->write_bytes(reg, &value, 1);
  }
}