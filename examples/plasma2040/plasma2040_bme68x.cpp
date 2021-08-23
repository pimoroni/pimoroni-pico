#include <stdio.h>
#include <math.h>
#include <cstdint>

#include "pico/stdlib.h"

#include "plasma2040.hpp"

#include "common/pimoroni_common.hpp"
#include "bme68x.hpp"
#include "rgbled.hpp"
#include "button.hpp"

/*
Press "A" to cycle to the next mode.
Press "B" to cycle to the previous mode.
*/

using namespace pimoroni;
using namespace plasma;

// Set how many LEDs you have
const uint N_LEDS = 30;

// How many times the LEDs will be updated per second
const uint UPDATES = 60;

// The temperature range to show (in degrees celsius)
constexpr float TEMPERATURE_C_MIN = 20.0f;
constexpr float TEMPERATURE_C_MAX = 35.0f;

// The pressure range to show (in pascals)
constexpr float PRESSURE_PA_MIN = 87000.0f;
constexpr float PRESSURE_PA_MAX = 108500.0f;

// The humidity range to show (in percent)
constexpr float HUMIDITY_MIN = 0.0f;
constexpr float HUMIDITY_MAX = 100.0f;


// The start and end hues for the temperature range
constexpr float TEMPERATURE_HUE_START = 0.667f;
constexpr float TEMPERATURE_HUE_END = 1.0f;

// The start and end hues for the pressure range
constexpr float PRESSURE_HUE_START = 0.333f;
constexpr float PRESSURE_HUE_END = 0.0f;

// The start and end hues for the humidity range
constexpr float HUMIDITY_HUE_START = 0.333f;
constexpr float HUMIDITY_HUE_END = 0.667f;


// Pick *one* LED type by uncommenting the relevant line below:

// APA102-style LEDs with Data/Clock lines. AKA DotStar
//APA102 led_strip(N_LEDS, pio0, 0, plasma2040::DAT, plasma2040::CLK);

// WS28X-style LEDs with a single signal line. AKA NeoPixel
WS2812 led_strip(N_LEDS, pio0, 0, plasma2040::DAT);



Button button_a(plasma2040::BUTTON_A, ACTIVE_LOW, 0);
Button button_b(plasma2040::BUTTON_B, ACTIVE_LOW, 0);

RGBLED led(plasma2040::LED_R, plasma2040::LED_G, plasma2040::LED_B);

I2C i2c(BOARD::PICO_EXPLORER);
BME68X bme68x(&i2c);

enum DisplayMode {
  ALL,
  TEMPERATURE,
  PRESSURE,
  HUMIDITY
};

// Maps a value from one range to another
float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Sets a section of the led strip to show a hue gradient based on the provided percent
void colour_gauge(float percent, uint start_led, uint end_led, float start_hue, float end_hue) {
  if(end_led > start_led) {
    uint range = end_led - start_led;
    float light_pixels = percent * range;

    for(uint i = 0; i < range; i++) {
      float h = map(i, 0.0f, (float)range - 1, start_hue, end_hue);

      uint i2 = i + 1;
      if(i2 <= light_pixels) {
        led_strip.set_hsv(i + start_led, h, 1.0f, 1.0f);
      }
      else if(i <= light_pixels) {
        float scale = map(light_pixels, (float)i, (float)i2, 0.0f, 1.0f);
        led_strip.set_hsv(i + start_led, h, 1.0f, scale);
      }
      else {
        led_strip.set_hsv(i + start_led, 0.0f, 0.0f, 0.0f);
      }
    }
  }
}

int main() {
  stdio_init_all();

  led_strip.start(UPDATES);

  bool bme_detected = bme68x.init();


  uint first_third = led_strip.num_leds / 3;
  uint second_third = (led_strip.num_leds * 2) / 3;

  float t = 0.0f;
  DisplayMode mode = DisplayMode::ALL;
  while(true) {
    if(bme_detected) {
      bme68x_data data;

      auto result = bme68x.read_forced(&data);
      (void)result;

      printf("%.2fc, %.2fPa, %.2f%%\n", data.temperature, data.pressure, data.humidity);

      switch(mode) {
        case DisplayMode::ALL:
          t = map(data.temperature, TEMPERATURE_C_MIN, TEMPERATURE_C_MAX, 0.0f, 1.0f);
          colour_gauge(t, 0, first_third, TEMPERATURE_HUE_START, TEMPERATURE_HUE_END);

          t = map(data.pressure, PRESSURE_PA_MIN, PRESSURE_PA_MAX, 0.0f, 1.0f);
          colour_gauge(t, first_third, second_third, PRESSURE_HUE_START, PRESSURE_HUE_END);

          t = map(data.humidity, HUMIDITY_MIN, HUMIDITY_MAX, 0.0f, 1.0f);
          colour_gauge(t, second_third, led_strip.num_leds, HUMIDITY_HUE_START, HUMIDITY_HUE_END);
          break;

        case DisplayMode::TEMPERATURE:
          t = map(data.temperature, TEMPERATURE_C_MIN, TEMPERATURE_C_MAX, 0.0f, 1.0f);
          colour_gauge(t, 0, led_strip.num_leds, TEMPERATURE_HUE_START, TEMPERATURE_HUE_END);
          break;

        case DisplayMode::PRESSURE:
          t = map(data.pressure, PRESSURE_PA_MIN, PRESSURE_PA_MAX, 0.0f, 1.0f);
          colour_gauge(t, 0, led_strip.num_leds, PRESSURE_HUE_START, PRESSURE_HUE_END);
          break;

        case DisplayMode::HUMIDITY:
          t = map(data.humidity, HUMIDITY_MIN, HUMIDITY_MAX, 0.0f, 1.0f);
          colour_gauge(t, 0, led_strip.num_leds, HUMIDITY_HUE_START, HUMIDITY_HUE_END);
          break;
      }
    }
    bool a_pressed = button_a.read();
    bool b_pressed = button_b.read();

    switch(mode) {
      case DisplayMode::ALL:
        led.set_rgb(127, 127, 127);
        if(a_pressed) mode = DisplayMode::TEMPERATURE;
        else if(b_pressed) mode = DisplayMode::HUMIDITY;
        break;

      case DisplayMode::TEMPERATURE:
        led.set_rgb(255, 0, 255);
        if(a_pressed) mode = DisplayMode::PRESSURE;
        else if(b_pressed) mode = DisplayMode::ALL;
        break;

      case DisplayMode::PRESSURE:
        led.set_rgb(255, 255, 0);
        if(a_pressed) mode = DisplayMode::HUMIDITY;
        else if(b_pressed) mode = DisplayMode::TEMPERATURE;
        break;

      case DisplayMode::HUMIDITY:
        led.set_rgb(0, 255, 255);
        if(a_pressed) mode = DisplayMode::ALL;
        else if(b_pressed) mode = DisplayMode::PRESSURE;
        break;
    }
  }
}
