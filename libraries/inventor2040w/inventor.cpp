#include <math.h>

#include "inventor.hpp"
#include "pwm.hpp"

namespace inventor {

Inventor2040W::Inventor2040W(float motor_gear_ratio)
: motors{Motor(MOTOR_A_PINS), Motor(MOTOR_B_PINS)}
, encoders{Encoder(pio0, 0, ENCODER_A_PINS, PIN_UNUSED, NORMAL_DIR, MMME_CPR * motor_gear_ratio, true),
           Encoder(pio0, 1, ENCODER_B_PINS, PIN_UNUSED, NORMAL_DIR, MMME_CPR * motor_gear_ratio, true)}
, servos{Servo(SERVO_1_PIN), Servo(SERVO_2_PIN), Servo(SERVO_3_PIN),
         Servo(SERVO_4_PIN), Servo(SERVO_5_PIN), Servo(SERVO_6_PIN)}
, leds(NUM_LEDS, pio0, 2, LED_DATA_PIN)
, i2c(I2C_SDA_PIN, I2C_SCL_PIN, 100000)
, audio_volume(DEFAULT_VOLUME) {
}

bool Inventor2040W::init(bool init_motors, bool init_servos) {
  bool success = true;

  // Set up the motors and encoders, if the user wants them
  if(init_motors) {
    for(uint i = 0; i < NUM_MOTORS && success; i++) {
      success = motors[i].init() && encoders[i].init();
    }
  }

  // Set up the servos, if the user wants them
  if(init_servos) {
    for(uint i = 0; i < NUM_SERVOS && success; i++) {
      success = servos[i].init();
    }
  }

  // Set up the amp enable
  gpio_set_function(AMP_EN_PIN, GPIO_FUNC_SIO);
  gpio_set_dir(AMP_EN_PIN, GPIO_OUT);
  gpio_put(AMP_EN_PIN, false);

  // Set up the audio pwm channel
  uint16_t period; uint16_t div16;
  if(pimoroni::calculate_pwm_factors(SILENCE_FREQUENCY, period, div16)) {
    pwm_config pwm_cfg = pwm_get_default_config();

    // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
    pwm_config_set_wrap(&pwm_cfg, period - 1);

    // Apply the divider
    pwm_config_set_clkdiv(&pwm_cfg, (float)div16 / 16.0f); // There's no 'pwm_config_set_clkdiv_int_frac' for some reason...

    // Set up the phase and enable pins
    pwm_init(pwm_gpio_to_slice_num(PWM_AUDIO_PIN), &pwm_cfg, true);
    gpio_set_function(PWM_AUDIO_PIN, GPIO_FUNC_PWM);
    pwm_set_gpio_level(PWM_AUDIO_PIN, 0);

    success = true;
  }

  // Set up the user switch
  gpio_set_function(USER_SW_PIN, GPIO_FUNC_SIO);
  gpio_set_dir(USER_SW_PIN, GPIO_IN);
  gpio_pull_down(USER_SW_PIN);

  // Start the WS2812 LEDs
  leds.start();

  return success;
}

bool Inventor2040W::switch_pressed() {
  return gpio_get(USER_SW_PIN);
}

bool Inventor2040W::play_tone(float frequency) {
  bool success = false;

  // Calculate a suitable pwm wrap period for this frequency
  uint16_t period; uint16_t div16;
  if(pimoroni::calculate_pwm_factors(frequency, period, div16)) {

    uint8_t div = div16 >> 4;
    uint8_t mod = div16 % 16;

    uint pin_slice = pwm_gpio_to_slice_num(PWM_AUDIO_PIN);

    // Apply the new divider
    pwm_set_clkdiv_int_frac(pin_slice, div, mod);

    float corrected_volume = powf(audio_volume, 4.0f);
    uint16_t level = (uint16_t)((float)(period >> 1) * corrected_volume);

    // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
    pwm_set_wrap(pin_slice, period - 1);

    // Update the pwm after setting the new wrap, and unmute
    pwm_set_gpio_level(PWM_AUDIO_PIN, level);
    unmute_audio();

    success = true;
  }
  else {
    play_silence();
  }
  return success;
}

void Inventor2040W::play_silence() {
  // Calculate a suitable pwm wrap period for the "silence" frequency
  uint16_t period; uint16_t div16;
  if(pimoroni::calculate_pwm_factors(SILENCE_FREQUENCY, period, div16)) {

    uint8_t div = div16 >> 4;
    uint8_t mod = div16 % 16;

    uint pin_slice = pwm_gpio_to_slice_num(PWM_AUDIO_PIN);

    // Apply the new divider
    pwm_set_clkdiv_int_frac(pin_slice, div, mod);

    float corrected_volume = powf(audio_volume, 4.0f);
    uint16_t level = (uint16_t)((float)(period >> 1) * corrected_volume);

    // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
    pwm_set_wrap(pin_slice, period - 1);

    // Update the pwm after setting the new wrap, and unmute
    pwm_set_gpio_level(PWM_AUDIO_PIN, level);
    unmute_audio();
  }
}

void Inventor2040W::stop_playing() {
  pwm_set_gpio_level(PWM_AUDIO_PIN, 0);
  mute_audio();
}

float Inventor2040W::volume() const {
  return audio_volume;
}

void Inventor2040W::volume(float volume) {
  audio_volume = MIN(MAX(volume, 0.0f), 1.0f);
}

void Inventor2040W::mute_audio() {
  gpio_put(AMP_EN_PIN, false);
}

void Inventor2040W::unmute_audio() {
  gpio_put(AMP_EN_PIN, true);
}
}
