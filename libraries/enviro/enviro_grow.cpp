#include <math.h>

#include "enviro_grow.hpp"
#include "pwm.hpp"

namespace enviro {

const uint EnviroGrow::SENSOR_PINS[] = {15, 14, 13};
const uint EnviroGrow::PUMP_PINS[] = {12, 11, 10};

EnviroGrow::EnviroGrow()
: EnviroBase()
, bme280(&i2c, 0x77)
, ltr559(&i2c) {
}

bool EnviroGrow::init() {
  // Attempt to initialise our base
  if(!EnviroBase::init())
    return false;

  // Attempt to initialise our i2c sensors
  if(!bme280.init() || !ltr559.init())
    return false;

  // Set up the sensor pins
  for(auto i = 0u; i < NUM_SENSORS; i++) {
    gpio_set_function(SENSOR_PINS[i], GPIO_FUNC_SIO);
    gpio_set_dir(SENSOR_PINS[i], GPIO_IN);
    gpio_pull_down(SENSOR_PINS[i]);
  }

  // Set up the pump pins
  for(auto i = 0u; i < NUM_PUMPS; i++) {
    gpio_set_function(PUMP_PINS[i], GPIO_FUNC_SIO);
    gpio_set_dir(PUMP_PINS[i], GPIO_OUT);
    gpio_put(PUMP_PINS[i], false);
  }

  // Set up the audio pwm channel
  uint16_t period; uint16_t div16;
  if(pimoroni::calculate_pwm_factors(DEFAULT_FREQUENCY, period, div16)) {
    pwm_config pwm_cfg = pwm_get_default_config();

    // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
    pwm_config_set_wrap(&pwm_cfg, period - 1);

    // Apply the divider
    pwm_config_set_clkdiv(&pwm_cfg, (float)div16 / 16.0f); // There's no 'pwm_config_set_clkdiv_int_frac' for some reason...

    // Set up the phase and enable pins
    pwm_init(pwm_gpio_to_slice_num(PIEZO_PIN), &pwm_cfg, true);
    gpio_set_function(PIEZO_PIN, GPIO_FUNC_PWM);
    pwm_set_gpio_level(PIEZO_PIN, 0);

    return true;
  }

  return false;
}

bool EnviroGrow::take_reading() {
  return false;
}

uint EnviroGrow::sensor_ticks(uint sensor, uint32_t sample_time_ms) {
  assert(sensor < NUM_SENSORS);
  uint32_t start = time();

  bool was = gpio_get(SENSOR_PINS[sensor]);
  int ticks = 0;
  while(time() - start < sample_time_ms) {
    bool now = gpio_get(SENSOR_PINS[sensor]);
    if(!was && now) {
      ticks++;
    }
    was = now;
  }

  return ticks;
}

void EnviroGrow::pump(uint pump, bool on) {
  assert(pump < NUM_PUMPS);
  // Only allow one pump to be active at a time, for current reasons
  for(auto i = 0u; i < NUM_PUMPS; i++) {
    if(i != pump)
      gpio_put(PUMP_PINS[i], false);
  }
  gpio_put(PUMP_PINS[pump], on);
}

bool EnviroGrow::play_tone(float frequency) {
  bool success = false;

  // Calculate a suitable pwm wrap period for this frequency
  uint16_t period; uint16_t div16;
  if(pimoroni::calculate_pwm_factors(frequency, period, div16)) {

    uint8_t div = div16 >> 4;
    uint8_t mod = div16 % 16;

    uint pin_slice = pwm_gpio_to_slice_num(PIEZO_PIN);

    // Apply the new divider
    pwm_set_clkdiv_int_frac(pin_slice, div, mod);

    // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
    pwm_set_wrap(pin_slice, period - 1);

    // Update the pwm after setting the new wrap, and unmute
    pwm_set_gpio_level(PIEZO_PIN, period >> 1);

    success = true;
  }
  else {
    stop_playing();
  }
  return success;
}

void EnviroGrow::stop_playing() {
  pwm_set_gpio_level(PIEZO_PIN, 0);
}
}
