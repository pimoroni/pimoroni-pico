#include "led_strip.hpp"

namespace pimoroni {

  const std::string LEDStripModule::NAME = "LED Strip";

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | LOW   | 1     | 1     | 1     | LED Strip            |                             |
  bool LEDStripModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return adc_level == ADC_LOW && slow1 == HIGH && slow2 == HIGH && slow3 == HIGH;
  }

  LEDStripModule::LEDStripModule(StripType strip_type, uint num_pixels, float brightness, bool halt_on_not_pgood) :
    YukonModule(),
    strip_type(strip_type),
    num_pixels(num_pixels),
    brightness(brightness),
    halt_on_not_pgood(halt_on_not_pgood),
    last_pgood(false),
    ws_pixels(nullptr),
    apa_pixels(nullptr),
    power_good(nullptr),
    power_en(nullptr) {
  }

  LEDStripModule::~LEDStripModule() {
    if(strip_type == NEOPIXEL) {
      if(ws_pixels != nullptr) {
        ws_pixels->stop();
        delete(ws_pixels);
      }
    }
    else {
      if(apa_pixels != nullptr) {
        apa_pixels->stop();
        delete(apa_pixels);
      }
    }
    delete(power_good);
    delete(power_en);
  }

  std::string LEDStripModule::name() {
    if(strip_type == NEOPIXEL) {
      return LEDStripModule::NAME + " (NeoPixel)";
    }
    else {
      return LEDStripModule::NAME + " (DotStar)";
    }
  }

  void LEDStripModule::initialise(const SLOT& slot, SlotAccessor& accessor) {
    // Create the strip driver object
    if(strip_type == NEOPIXEL) {
      ws_pixels = new WS2812(num_pixels, pio0, 0, slot.FAST4);
      ws_pixels->start(60);
      for(auto i = 0u; i < ws_pixels->num_leds; ++i) {
        float hue = float(i) / ws_pixels->num_leds;
        ws_pixels->set_hsv(i, hue, 1.0f, 1.0f);
      }
    }
    else {
      apa_pixels = new APA102(num_pixels, pio0, 0, slot.FAST4, slot.FAST3);
      apa_pixels->start(60);
      for(auto i = 0u; i < apa_pixels->num_leds; ++i) {
        float hue = float(i) / apa_pixels->num_leds;
        apa_pixels->set_hsv(i, hue, 1.0f, 1.0f);
      }
    }

    // Create the power control pin objects
    power_good = new IO(slot.FAST1);
    power_en = new IO(slot.FAST2);

    // Pass the slot and adc functions up to the parent now that module specific initialisation has finished
    YukonModule::initialise(slot, accessor);
  }

  void LEDStripModule::configure() {
    CHECK_INITIALISED
    power_en->to_output(false);
    power_good->to_input(true, false);
  }

  void LEDStripModule::enable() {
    CHECK_INITIALISED
    power_en->value(true);
  }

  void LEDStripModule::disable() {
    CHECK_INITIALISED
    power_en->value(false);
  }

  bool LEDStripModule::is_enabled() {
    CHECK_INITIALISED
    return power_en->value();
  }

  bool LEDStripModule::read_power_good() {
    CHECK_INITIALISED
    return power_good->value();
  }

  float LEDStripModule::read_temperature() {
    return __read_adc2_as_temp();
  }

  void LEDStripModule::monitor() {
    bool pgood = read_power_good();
    if(!pgood) {
      if(halt_on_not_pgood) {
        throw FaultError(__message_header() + "Power is not good! Turning off output\n");
      }
    }

    float temperature = read_temperature();
    if(temperature > TEMPERATURE_THRESHOLD) {
      throw OverTemperatureError(__message_header() + "Temperature of " + std::to_string(temperature) + "°C exceeded the user set level of " + std::to_string(TEMPERATURE_THRESHOLD) + "°C! Turning off output\n");
    }

    if(last_pgood && !pgood) {
      logging.warn(__message_header() + "Power is not good\n");
    }
    else if(!last_pgood && pgood) {
      logging.warn(__message_header() + "Power is good\n");
    }

    // Run some user action based on the latest readings
    //if self.__monitor_action_callback is not None:
    //  self.__monitor_action_callback(pgood, temperature)

    last_pgood = pgood;
    power_good_throughout = power_good_throughout && pgood;

    max_temperature = MAX(temperature, max_temperature);
    min_temperature = MIN(temperature, min_temperature);
    avg_temperature += temperature;
    count_avg += 1;
  }

  std::vector<std::pair<std::string, float>> LEDStripModule::get_readings() {
    std::vector<std::pair<std::string, float>> values;
    values.push_back(std::pair("PGood", power_good_throughout));
    values.push_back(std::pair("T_max", max_temperature));
    values.push_back(std::pair("T_min", min_temperature));
    values.push_back(std::pair("T_avg", avg_temperature));
    return values;
  }

  void LEDStripModule::process_readings() {
    if(count_avg > 0) {
      avg_temperature /= count_avg;
    }
  }

  void LEDStripModule::clear_readings() {
    power_good_throughout = true;
    max_temperature = -std::numeric_limits<float>::infinity();
    min_temperature = std::numeric_limits<float>::infinity();
    avg_temperature = 0;
    count_avg = 0;
  }

}
