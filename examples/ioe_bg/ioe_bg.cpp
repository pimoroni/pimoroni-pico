#include <sstream>
#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include "pico/stdlib.h"

#include "pico_explorer.hpp"
#include "ms51xb9ae.hpp"

static const uint8_t PIN_RED = 1;
static const uint8_t PIN_GREEN = 7;
static const uint8_t PIN_BLUE = 2;

static const uint8_t PIN_ENC_A = 12;
static const uint8_t PIN_ENC_B = 3;
static const uint8_t PIN_ENC_C = 11;

static const uint8_t ENC_CHANNEL = 1;

static const bool INVERT_OUTPUT = true; //true for common cathode, false for common anode

using namespace pimoroni;

uint16_t buffer[PicoExplorer::WIDTH * PicoExplorer::HEIGHT];
PicoExplorer pico_explorer(buffer);

MS51XB9AE ioe(0x0f);
bool toggle = false;

void HSVtoRGB(float hue, float sat, float val, uint8_t& rOut, uint8_t& gOut, uint8_t& bOut)
{
  ////////////////////////////////
  // Convert HSV values to RGB
  // H(Hue): 0-360 degrees
  // S(Saturation): 0-100 percent
  // V(Value): 0-100 percent
  //
  // RGB out is in range 0->255.
  // This method was found at:
  // https://www.codespeedy.com/hsv-to-rgb-in-cpp/
  ////////////////////////////////
  if(hue > 360.0f || hue < 0.0f || sat > 100.0f || sat < 0.0f || val > 100.0f || val < 0.0f)
  {
    //The given HSV values are not in valid range
    rOut = 0;
    gOut = 0;
    bOut = 0;
    return;
  }
  
  float s = sat / 100.0f;
  float v = val / 100.0f;
  float c = s * v;
  float x = c * (1.0f - fabsf(fmodf(hue / 60.0f, 2.0f) - 1.0f));
  float m = v - c;
  
  float r, g, b;
  if(hue >= 0.0f && hue < 60.0f)
      r = c, g = x, b = 0.0f;
  else if(hue >= 60.0f && hue < 120.0f)
      r = x, g = c, b = 0.0f;
  else if(hue >= 120.0f && hue < 180.0f)
      r = 0.0f, g = c, b = x;
  else if(hue >= 180.0f && hue < 240.0f)
      r = 0.0f, g = x, b = c;
  else if(hue >= 240.0f && hue < 300.0f)
      r = x, g = 0.0f, b = c;
  else
      r = c, g = 0.0f, b = x;

  rOut = (uint8_t)((r + m) * 255.0f);
  gOut = (uint8_t)((g + m) * 255.0f);
  bOut = (uint8_t)((b + m) * 255.0f);
}

int main() {
  pico_explorer.init();
  pico_explorer.update();
  stdio_init_all();

  sleep_ms(5000);

  const uint LED_PIN = 25;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  const uint DBG_PIN = 26;
  gpio_init(DBG_PIN);
  gpio_set_dir(DBG_PIN, GPIO_OUT);

  float brightness = 1.0f;

  int16_t count = 0;
  if(ioe.init()) {
    printf("IOExpander found...\n");

    ioe.setup_rotary_encoder(ENC_CHANNEL, PIN_ENC_A, PIN_ENC_B, PIN_ENC_C);

    //Calculate a period large enough to get 0-255 steps at the desired brightness
    unsigned int period = (unsigned int)(255.0f / brightness);
  
    ioe.set_pwm_period(period);
    ioe.set_pwm_control(2);  //PWM as fast as we can to avoid LED flicker

    ioe.set_mode(PIN_RED, MS51XB9AE::PIN_PWM, false, INVERT_OUTPUT);
    ioe.set_mode(PIN_GREEN, MS51XB9AE::PIN_PWM, false, INVERT_OUTPUT);
    ioe.set_mode(PIN_BLUE, MS51XB9AE::PIN_PWM, false, INVERT_OUTPUT);

    sleep_ms(1000);

    while (true) {
      gpio_put(LED_PIN, toggle);
      toggle = !toggle;

      if(ioe.get_interrupt_flag() > 0) {  //This should work but
        gpio_put(DBG_PIN, 1);
        count = ioe.read_rotary_encoder(ENC_CHANNEL);
        ioe.clear_interrupt_flag();
        gpio_put(DBG_PIN, 0);
      }
  
      while(count < 0)
        count += 24;
  
      float h = ((float)(count % 24) * 360.0f) / 24.0f;
      uint8_t r, g, b;
      HSVtoRGB(h, 100.0f, 100.0f, r, g, b);
      
      ioe.output(PIN_RED, r, false);   //Hold off pwm load until the last
      ioe.output(PIN_GREEN, g, false); //Hold off pwm load until the last
      ioe.output(PIN_BLUE, b);         //Loads all 3 pwms



      pico_explorer.set_pen(0, 0, 0);
      pico_explorer.clear();

      {
        pico_explorer.set_pen(255, 0, 0);
        std::ostringstream ss;
        ss << "R = ";
        ss << (int)r;
        std::string s(ss.str());
        pico_explorer.text(s, Point(10, 10), 220, 6);
      }

      {
        pico_explorer.set_pen(0, 255, 0);
        std::ostringstream ss;
        ss << "G = ";
        ss << (int)g;
        std::string s(ss.str());
        pico_explorer.text(s, Point(10, 70), 220, 6);
      }

      {
        pico_explorer.set_pen(0, 0, 255);
        std::ostringstream ss;
        ss << "B = ";
        ss << (int)b;
        std::string s(ss.str());
        pico_explorer.text(s, Point(10, 130), 220, 6);
      }
      pico_explorer.update();

      sleep_ms(10);
    }  
  }
  else {
    printf("IOExpander not found :'(\n");
    gpio_put(LED_PIN, 1);
  }
  
  return 0;
}
