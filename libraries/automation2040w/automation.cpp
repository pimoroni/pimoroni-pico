#include <math.h>

#include "automation.hpp"

namespace automation {

const uint Automation2040W::ADC_LED_PINS[] = {6, 7, 8};
const uint Automation2040W::RELAY_PINS[] = {9, 10, 11};
const uint Automation2040W::USER_SW_PINS[] = {12, 13};
const uint Automation2040W::USER_LED_PINS[] = {14, 15};
const uint Automation2040W::OUTPUT_PINS[] = {16, 17, 18};
const uint Automation2040W::IN_BUFFERED_PINS[] = {19, 20, 21, 22};
const uint Automation2040W::ADC_PINS[] = {26, 27, 28};


Automation2040W::Automation2040W()
: i2c(I2C_SDA_PIN, I2C_SCL_PIN, 100000)
, analogs{Analog(ADC_PINS[0], VOLTAGE_GAIN, 0.0f, VOLTAGE_OFFSET),
          Analog(ADC_PINS[1], VOLTAGE_GAIN, 0.0f, VOLTAGE_OFFSET),
          Analog(ADC_PINS[2], VOLTAGE_GAIN, 0.0f, VOLTAGE_OFFSET)} {
}

bool Automation2040W::init() {
  // Set up the relay pins
  for(auto i = 0u; i < NUM_RELAYS; i++) {
    gpio_set_function(RELAY_PINS[i], GPIO_FUNC_SIO);
    gpio_set_dir(RELAY_PINS[i], GPIO_OUT);
    release_relay(i);
  }

  // Set up the output pins
  for(auto i = 0u; i < NUM_OUTPUTS; i++) {
    gpio_set_function(OUTPUT_PINS[i], GPIO_FUNC_SIO);
    gpio_set_dir(OUTPUT_PINS[i], GPIO_OUT);
    output(i, false);
  }

  // Set up the input pins
  for(auto i = 0u; i < NUM_INPUTS; i++) {
    gpio_set_function(IN_BUFFERED_PINS[i], GPIO_FUNC_SIO);
    gpio_set_dir(IN_BUFFERED_PINS[i], GPIO_IN);
  }

  // Set up the adc leds
  for(auto i = 0u; i < NUM_ADCS; i++) {
    gpio_put(ADC_LED_PINS[i], false);
    pwm_config cfg = pwm_get_default_config();
    pwm_set_wrap(pwm_gpio_to_slice_num(ADC_LED_PINS[i]), 65535);
    pwm_init(pwm_gpio_to_slice_num(ADC_LED_PINS[i]), &cfg, true);
    gpio_set_function(ADC_LED_PINS[i], GPIO_FUNC_PWM);
    pwm_set_gpio_level(ADC_LED_PINS[i], 0);
  }

  // Set up the user switches and LEDs
  for(auto i = 0u; i < NUM_SWITCHES; i++) {
    gpio_set_function(USER_SW_PINS[i], GPIO_FUNC_SIO);
    gpio_set_dir(USER_SW_PINS[i], GPIO_IN);
    gpio_pull_up(USER_SW_PINS[i]);

    gpio_put(USER_LED_PINS[i], false);
    pwm_config cfg = pwm_get_default_config();
    pwm_set_wrap(pwm_gpio_to_slice_num(USER_LED_PINS[i]), 65535);
    pwm_init(pwm_gpio_to_slice_num(USER_LED_PINS[i]), &cfg, true);
    gpio_set_function(USER_LED_PINS[i], GPIO_FUNC_PWM);
    pwm_set_gpio_level(USER_LED_PINS[i], 0);
  }

  // Set up the connectivity LED
  gpio_put(CONN_LED_PIN, false);
  pwm_config cfg = pwm_get_default_config();
  pwm_set_wrap(pwm_gpio_to_slice_num(CONN_LED_PIN), 65535);
  pwm_init(pwm_gpio_to_slice_num(CONN_LED_PIN), &cfg, true);
  gpio_set_function(CONN_LED_PIN, GPIO_FUNC_PWM);
  pwm_set_gpio_level(CONN_LED_PIN, 0);

  return true;  // We just return true for now, but could expand in the future
}

void Automation2040W::conn_led(bool on) {
  pwm_set_gpio_level(CONN_LED_PIN, on ? 65535 : 0);
}

void Automation2040W::conn_led(float brightness) {
  brightness = CLAMP(brightness, 0.0f, 100.0f);
  const float gamma = 2.8f;
  uint16_t value = (uint16_t)(powf(brightness / 100.0f, gamma) * 65535.0f + 0.5f);
  pwm_set_gpio_level(CONN_LED_PIN, value);
}

bool Automation2040W::switch_pressed(uint sw) {
  assert(sw < NUM_SWITCHES);
  return !gpio_get(USER_SW_PINS[sw]);
}

void Automation2040W::switch_led(uint sw, bool on) {
  assert(sw < NUM_SWITCHES);
  pwm_set_gpio_level(USER_LED_PINS[sw], on ? 65535 : 0);
}

void Automation2040W::switch_led(uint sw, float brightness) {
  assert(sw < NUM_SWITCHES);

  brightness = CLAMP(brightness, 0.0f, 100.0f);
  const float gamma = 2.8f;

  uint16_t value = (uint16_t)(powf(brightness / 100.0f, gamma) * 65535.0f + 0.5f);
  pwm_set_gpio_level(USER_LED_PINS[sw], value);
}

bool Automation2040W::relay(uint relay) {
  assert(relay < NUM_RELAYS);
  return gpio_get(RELAY_PINS[relay]);
}

void Automation2040W::relay(uint relay, bool actuate) {
  assert(relay < NUM_RELAYS);
  gpio_put(RELAY_PINS[relay], actuate);
}

void Automation2040W::actuate_relay(uint relay) {
  assert(relay < NUM_RELAYS);
  gpio_put(RELAY_PINS[relay], true);
}

void Automation2040W::release_relay(uint relay) {
  assert(relay < NUM_RELAYS);
  gpio_put(RELAY_PINS[relay], false);
}

bool Automation2040W::output(uint output) {
  assert(output < NUM_OUTPUTS);
  return gpio_get(OUTPUT_PINS[output]);
}

void Automation2040W::output(uint output, bool value) {
  assert(output < NUM_OUTPUTS);
  gpio_put(OUTPUT_PINS[output], value);
}

bool Automation2040W::read_input(uint input) {
  assert(input < NUM_INPUTS);
  return gpio_get(IN_BUFFERED_PINS[input]);
}

float Automation2040W::read_adc(uint adc) {
  assert(adc < NUM_ADCS);
  float voltage = analogs[adc].read_voltage();
  const float gamma = 2.8f;
  uint16_t value = (uint16_t)(powf(voltage / MAX_ADC_LED_VOLTAGE, gamma) * 65535.0f + 0.5f);
  pwm_set_gpio_level(ADC_LED_PINS[adc], value);
  return voltage;
}

void Automation2040W::reset() {
  // Reset the relays
  for(auto i = 0u; i < NUM_RELAYS; i++) {
    release_relay(i);
  }

  // Reset the outputs
  for(auto i = 0u; i < NUM_OUTPUTS; i++) {
    output(i, false);
  }

  // Reset the adc leds
  for(auto i = 0u; i < NUM_ADCS; i++) {
    pwm_set_gpio_level(ADC_LED_PINS[i], 0);
  }

  // Reset the switch LEDs
  for(auto i = 0u; i < NUM_SWITCHES; i++) {
    pwm_set_gpio_level(USER_LED_PINS[i], 0);
  }

  // Reset the connectivity LED
  pwm_set_gpio_level(CONN_LED_PIN, 0);
}
}
