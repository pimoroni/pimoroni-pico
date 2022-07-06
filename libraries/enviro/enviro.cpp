#include <math.h>

#include "enviro.hpp"

uint32_t time() {
  absolute_time_t t = get_absolute_time();
  return to_ms_since_boot(t);
}

namespace enviro {

EnviroBase::EnviroBase()
: i2c(I2C_SDA_PIN, I2C_SCL_PIN, 100000)
, rtc(&i2c)
, woken_up_by(REASON_NONE) {
}

bool EnviroBase::init() {
  // Set up and enable vsys hold so we don't go to sleep
  gpio_set_function(HOLD_VSYS_EN_PIN, GPIO_FUNC_SIO);
  gpio_set_dir(HOLD_VSYS_EN_PIN, GPIO_OUT);
  gpio_put(HOLD_VSYS_EN_PIN, true);

  // Set up the activity led
  gpio_put(ACTIVITY_LED_PIN, false);
  pwm_config cfg = pwm_get_default_config();
  pwm_set_wrap(pwm_gpio_to_slice_num(ACTIVITY_LED_PIN), 65535);
  pwm_init(pwm_gpio_to_slice_num(ACTIVITY_LED_PIN), &cfg, true);
  gpio_set_function(ACTIVITY_LED_PIN, GPIO_FUNC_PWM);
  pwm_set_gpio_level(ACTIVITY_LED_PIN, 0);

  // Set up the switch pin
  gpio_set_function(SWITCH_PIN, GPIO_FUNC_SIO);
  gpio_set_dir(SWITCH_PIN, GPIO_IN);
  gpio_pull_down(SWITCH_PIN);

  // Set up the rtc alarm pin
  gpio_set_function(RTC_ALARM_PIN, GPIO_FUNC_SIO);
  gpio_set_dir(RTC_ALARM_PIN, GPIO_IN);
  gpio_pull_down(RTC_ALARM_PIN);

  // Find out what we were woken by (if anything)
  if(gpio_get(SWITCH_PIN))
    woken_up_by = REASON_SWITCH_PRESSED;
  else if(gpio_get(RTC_ALARM_PIN))
    woken_up_by = REASON_RTC_ALARM;

  // TODO: pcf85063a driver should probably return a success flag from init()
  rtc.init();

  // The pcf85063a defaults to 32KHz clock output so
  // we need to explicitly turn that off by default
  warn_led(WARN_LED_OFF);

  return true;
}

void EnviroBase::sleep(int seconds) {
  if(seconds != -1) {
    // Tell the RTC to wake us up once the number of seconds has passed
    rtc.enable_timer_interrupt(true);
    rtc.set_timer(seconds);
  }

  // Disable the vsys hold, causing us to turn off
  gpio_set_dir(HOLD_VSYS_EN_PIN, GPIO_IN);
}

void EnviroBase::activity_led(bool on) {
  pwm_set_gpio_level(ACTIVITY_LED_PIN, on ? 65535 : 0);
}

void EnviroBase::activity_led(float brightness) {
  brightness = CLAMP(brightness, 0.0f, 100.0f);
  const float gamma = 2.8;
  uint value = (uint16_t)(pow((float)(brightness) / 100.0f, gamma) * 65535.0f + 0.5f);
  pwm_set_gpio_level(ACTIVITY_LED_PIN, value);
}

void EnviroBase::warn_led(WarnLEDState wls) {
  switch(wls) {
    case WARN_LED_OFF: { 
      rtc.set_clock_output(PCF85063A::CLOCK_OUT_OFF);
      break;
    }
    case WARN_LED_ON: { 
      rtc.set_clock_output(PCF85063A::CLOCK_OUT_1024HZ);
      break;
    }
    case WARN_LED_BLINK: { 
      rtc.set_clock_output(PCF85063A::CLOCK_OUT_1HZ);
      break;
    }
  }
}

bool EnviroBase::switch_pressed() {
  return gpio_get(SWITCH_PIN);
}

EnviroBase::WakeReason EnviroBase::wake_reason(bool clear) {
  WakeReason result = woken_up_by;
  if(clear) {
    woken_up_by = REASON_NONE;
  }
  return result;
}
}
