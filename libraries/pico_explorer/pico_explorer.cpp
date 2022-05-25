#include <math.h>
#include <string.h>

#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

#include "pico_explorer.hpp"

const uint8_t MOTOR1N = 8;
const uint8_t MOTOR1P = 9;
const uint8_t MOTOR2N = 10;
const uint8_t MOTOR2P = 11;

namespace pimoroni {

  PicoExplorer::PicoExplorer(uint8_t *buf)
    : PicoGraphics(WIDTH, HEIGHT, buf), 
      screen(WIDTH, HEIGHT, false, buf, PIMORONI_SPI_DEFAULT_INSTANCE, screen.get_slot_cs(PICO_EXPLORER_ONBOARD), SPI_DEFAULT_MISO, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, screen.get_slot_bl(PICO_EXPLORER_ONBOARD)) {
        __fb = buf;
  }

  void PicoExplorer::init() {
    // setup button inputs
    gpio_set_function(A, GPIO_FUNC_SIO); gpio_set_dir(A, GPIO_IN); gpio_pull_up(A);
    gpio_set_function(B, GPIO_FUNC_SIO); gpio_set_dir(B, GPIO_IN); gpio_pull_up(B);
    gpio_set_function(X, GPIO_FUNC_SIO); gpio_set_dir(X, GPIO_IN); gpio_pull_up(X);
    gpio_set_function(Y, GPIO_FUNC_SIO); gpio_set_dir(Y, GPIO_IN); gpio_pull_up(Y);

    // setup ADC channels
    adc_init();
    const uint8_t ADC_BASE_PIN = 26;
    adc_gpio_init(ADC0 + ADC_BASE_PIN);
    adc_gpio_init(ADC1 + ADC_BASE_PIN);
    adc_gpio_init(ADC2 + ADC_BASE_PIN);

    // setup motor pins
    pwm_config motor_pwm_cfg = pwm_get_default_config();
    pwm_config_set_wrap(&motor_pwm_cfg, 5500);

    pwm_init(pwm_gpio_to_slice_num(MOTOR1N), &motor_pwm_cfg, true);
    gpio_set_function(MOTOR1N, GPIO_FUNC_PWM);

    pwm_init(pwm_gpio_to_slice_num(MOTOR1P), &motor_pwm_cfg, true);
    gpio_set_function(MOTOR1P, GPIO_FUNC_PWM);

    pwm_init(pwm_gpio_to_slice_num(MOTOR2N), &motor_pwm_cfg, true);
    gpio_set_function(MOTOR2N, GPIO_FUNC_PWM);

    pwm_init(pwm_gpio_to_slice_num(MOTOR2P), &motor_pwm_cfg, true);
    gpio_set_function(MOTOR2P, GPIO_FUNC_PWM);
  }

  void PicoExplorer::update() {
    screen.update(palette);
  }

  bool PicoExplorer::is_pressed(uint8_t button) {
    return !gpio_get(button);
  }

  float PicoExplorer::get_adc(uint8_t channel) {
    adc_select_input(channel);
    // scale raw 12-bit adc value to 0 .. 1 float
    float result = float(adc_read()) / (1 << 12);
    // clamp result to 0 .. 1
    result = std::min(1.0f, std::max(0.0f, result));
    return result;
  }

  void PicoExplorer::set_motor(uint8_t channel, uint8_t action, float speed) {
    uint8_t p = channel == MOTOR1 ? MOTOR1P : MOTOR2P;
    uint8_t n = channel == MOTOR1 ? MOTOR1N : MOTOR2N;

    switch(action) {
      case FORWARD: {
        pwm_set_gpio_level(n, (1 - speed) * 5500);
        pwm_set_gpio_level(p, 5500);
        break;
      }

      case REVERSE: {
        pwm_set_gpio_level(n, 5500);
        pwm_set_gpio_level(p, (1 - speed) * 5500);
        break;
      }

      case STOP: {
        pwm_set_gpio_level(p, 5500);
        pwm_set_gpio_level(n, 5500);
        break;
      }
    }
  }

  void PicoExplorer::set_audio_pin(uint pin) {
    pwm_config tone_pwm_cfg = pwm_get_default_config();

    // calculate the pwm wrap value for this frequency
    // first we set the clock divider to give us exactly
    // ten thousand cycles per second
    pwm_config_set_clkdiv(&tone_pwm_cfg, 255);
    pwm_init(pwm_gpio_to_slice_num(pin), &tone_pwm_cfg, true);
    gpio_set_function(pin, GPIO_FUNC_PWM);
    audio_pin = pin;
  }

  void PicoExplorer::set_tone(uint16_t frequency, float duty) {
    // output a square wave, so 50% duty cycle
    if(audio_pin != -1) {
      uint16_t pwm_wrap = 490196 / frequency;
      pwm_set_wrap(audio_pin, pwm_wrap);
      pwm_set_gpio_level(audio_pin, pwm_wrap * duty);
    }
  }

}
