#include "max30105.hpp"
#include "pico/time.h"

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool MAX30105::init(int leds_enabled, float led_power, enum FIFO_CONFIG sample_average,
    enum SPO2_CONFIG sample_rate, enum SPO2_CONFIG pulse_width, enum SPO2_CONFIG adc_range, int timeout) {

    this->leds_enabled = leds_enabled;
  
    i2c_init(i2c, 400000);

    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(scl);

    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_pull_up(interrupt);
    }

    if(get_chip_id() != 0x15) {
      return false;
    }

    soft_reset(timeout);

    if(leds_enabled == 1) set_mode(MODE::red_only);
    if(leds_enabled == 2) set_mode(MODE::red_ir);
    if(leds_enabled == 3) set_mode(MODE::green_red_ir);
  
    uint8_t spo2_value = sample_rate | pulse_width | adc_range;
    write_bytes(reg::SPO2_CONFIG, &spo2_value, 1);

    change_bits(reg::FIFO_CONFIG, 0b11110000, sample_average | fifo_rollover_en);

    set_slot_mode(1, red);
    set_slot_mode(2, leds_enabled >= 2 ? ir : off);
    set_slot_mode(3, leds_enabled >= 3 ? green : off);
    set_slot_mode(4, off);

    set_led_pulse_amplitude(1, led_power);
    set_led_pulse_amplitude(2, led_power);
    set_led_pulse_amplitude(3, led_power);

    clear_fifo();

    return true;
  }

  void MAX30105::soft_reset(int timeout) {
    set_bits(reg::MODE_CONFIG, 7, 0b1);  // set the reset bit on MODE_CONFIG
    absolute_time_t finished = delayed_by_ms(get_absolute_time(), timeout);
    while (!time_reached(finished) && get_bits(reg::MODE_CONFIG, 7, 0b1)) {
      sleep_ms(1);
    }
  }

  void MAX30105::clear_fifo() {
    clear_bits(reg::FIFO_READ, 0, 0b00011111);
    clear_bits(reg::FIFO_WRITE, 0, 0b00011111);
    clear_bits(reg::FIFO_OVERFLOW, 0, 0b00011111);
  }

  uint8_t MAX30105::get_samples(uint32_t *samples) {
    int ptr_r = get_bits(reg::FIFO_READ, 0, 0b00011111);
    int ptr_w = get_bits(reg::FIFO_WRITE, 0, 0b00011111);

    if (ptr_r == ptr_w) {
      return 0;
    }

    int sample_count = ptr_w - ptr_r;
    if(sample_count < 0) {
      sample_count = 32;
    }

    int byte_count = sample_count * 3 * leds_enabled;

    uint8_t bytes[byte_count];

    read_bytes(reg::FIFO_DATA, bytes, byte_count);

    clear_fifo();

    for(auto x = 0u; x < sample_count * leds_enabled; x++) {
      int offset = x * 3;
      samples[x] = (bytes[offset] << 16) | (bytes[offset + 1] << 8) | bytes[offset + 2];
    }

    return sample_count;
  }

  uint8_t MAX30105::get_chip_id() {
    uint8_t temp = 0;
    read_bytes(reg::PART_ID, &temp, 1);
    return temp;
  }

  float MAX30105::get_temperature(int timeout) {
    set_bits(reg::INT_ENABLE_2, 0, INT_STATUS::die_temp_ready);
    set_bits(reg::DIE_TEMP_CONFIG, 0, 0b1);
  
    absolute_time_t finished = delayed_by_ms(get_absolute_time(), timeout);

    while (!time_reached(finished) && !get_bits(reg::INT_STATUS_2, 0, INT_STATUS::die_temp_ready)) {
      sleep_ms(1);
    }

    if(time_reached(finished)) return -1;

    uint8_t buf[2];
    read_bytes(reg::DIE_TEMP, buf, 2);

    int8_t integer = (int8_t)buf[0];
    uint8_t fractional = (uint8_t)buf[1];

    return (float)integer + ((float)fractional * 0.0625f);
  }

  void MAX30105::set_mode(MODE mode) {
    uint8_t value = mode;
    write_bytes(reg::MODE_CONFIG, &value, 1);
  }

  void MAX30105::set_slot_mode(uint8_t slot, LED_MODE mode) {
    switch(slot) {
      case 1:
        change_bits(LED_MODE_CONTROL_2_1, 0x07, mode);
        break;
      case 2:
        change_bits(LED_MODE_CONTROL_2_1, 0x70, mode << 4);
        break;
      case 3:
        change_bits(LED_MODE_CONTROL_4_3, 0x07, mode);
        break;
      case 4:
        change_bits(LED_MODE_CONTROL_4_3, 0x70, mode << 4);
        break;
    }
  }

  void MAX30105::set_led_pulse_amplitude(uint8_t led, float amplitude) {
    uint8_t led_power = amplitude * 5.0f;
    write_bytes(reg::LED1_PULSE_AMPLITUDE + (led - 1), &led_power, 1);
  }

  void MAX30105::set_fifo_almost_full_count(uint8_t count) {
    count &= 0b1111;
    change_bits(reg::FIFO_CONFIG, 0b00001111, count);
  }

  void MAX30105::set_fifo_almost_full_enable(bool enabled) {
    change_bits(reg::INT_ENABLE_1, INT_STATUS::a_full, enabled ? INT_STATUS::a_full : 0);
  }

  void MAX30105::set_data_ready_enable(bool enabled) {
    change_bits(reg::INT_ENABLE_1, INT_STATUS::data_ready, enabled ? INT_STATUS::data_ready : 0);
  }

  void MAX30105::set_ambient_light_compensation_overflow_enable(bool enabled) {
    change_bits(reg::INT_ENABLE_1, INT_STATUS::alc_overflow, enabled ? INT_STATUS::alc_overflow : 0);
  }

  void MAX30105::set_proximity_enable(bool enabled) {
    change_bits(reg::INT_ENABLE_1, INT_STATUS::prox_int, enabled ? INT_STATUS::prox_int : 0);
  }

  void MAX30105::set_proximity_threshold(uint8_t value) {
    write_bytes(reg::PROX_INT_THRESHOLD, &value, 1);
  }

  bool MAX30105::get_fifo_almost_full_status() {
    return get_bits(reg::INT_STATUS_1, 0, INT_STATUS::a_full);
  }

  bool MAX30105::get_data_ready_status() {
    return get_bits(reg::INT_STATUS_1, 0, INT_STATUS::data_ready);
  }

  bool MAX30105::get_ambient_light_compensation_overflow_status() {
    return get_bits(reg::INT_STATUS_1, 0, INT_STATUS::alc_overflow);
  }

  bool MAX30105::get_proximity_triggered_threshold_status() {
    return get_bits(reg::INT_STATUS_1, 0, INT_STATUS::prox_int);
  }

  bool MAX30105::get_power_ready_status() {
    return get_bits(reg::INT_STATUS_1, 0, INT_STATUS::pwr_ready);
  }

  bool MAX30105::get_die_temp_ready_status() {
    return get_bits(reg::INT_STATUS_2, 0, INT_STATUS::die_temp_ready);
  }

  // i2c functions

  int MAX30105::write_bytes(uint8_t reg, uint8_t *buf, int len) {
    uint8_t buffer[len + 1];
    buffer[0] = reg;
    for(int x = 0; x < len; x++) {
      buffer[x + 1] = buf[x];
    }
    return i2c_write_blocking(i2c, address, buffer, len + 1, false);
  };

  int MAX30105::read_bytes(uint8_t reg, uint8_t *buf, int len) {
    i2c_write_blocking(i2c, address, &reg, 1, true);
    i2c_read_blocking(i2c, address, buf, len, false);
    return len;
  };

  uint8_t MAX30105::get_bits(uint8_t reg, uint8_t shift, uint8_t mask) {
    uint8_t value;
    read_bytes(reg, &value, 1);
    return value & (mask << shift);
  }

  void MAX30105::change_bits(uint8_t reg, uint8_t mask, uint8_t value) {
    uint8_t temp;
    read_bytes(reg, &temp, 1);
    temp &= ~mask;
    temp |= value;
    write_bytes(reg, &temp, 1);
  }

  void MAX30105::set_bits(uint8_t reg, uint8_t shift, uint8_t mask) {
    uint8_t value;
    read_bytes(reg, &value, 1);
    value |= mask << shift;
    write_bytes(reg, &value, 1);
  }

  void MAX30105::clear_bits(uint8_t reg, uint8_t shift, uint8_t mask) {
    uint8_t value;
    read_bytes(reg, &value, 1);
    value &= ~(mask << shift);
    write_bytes(reg, &value, 1);
  }
}