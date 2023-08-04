#pragma once

#include "../common.hpp"
#include <limits>
#include "../../logging.hpp"
#include "../../errors.hpp"
#include "apa102.hpp"
#include "ws2812.hpp"
#include "io.hpp"

using namespace plasma;

namespace pimoroni {

  class LEDStripModule : public YukonModule {
  public:
    static const std::string NAME;

    enum strip_type {
      NEOPIXEL = 0,
      DOTSTAR = 1
    };

    static constexpr float TEMPERATURE_THRESHOLD = 50.0f;

    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);

    virtual std::string name() {
      return LEDStripModule::NAME + " (NeoPixel)";
    }

    TYPE_FUNCTION(LEDStripModule)

    bool halt_on_not_pgood;
    bool __last_pgood;

    bool __power_good_throughout;
    float __max_temperature;
    float __min_temperature;
    float __avg_temperature;
    float __count_avg;

    WS2812* led_strip;
    APA102* apa_strip;
    IO* __power_good_io;
    IO* __power_en_io;



    LEDStripModule(bool halt_on_not_pgood = false) :
      YukonModule(),
      halt_on_not_pgood(halt_on_not_pgood),
      __last_pgood(false),
      led_strip(nullptr),
      apa_strip(nullptr),
      __power_good_io(nullptr),
      __power_en_io(nullptr) { //TODO strip_type, num_pixels, brightness=1.0, halt_on_not_pgood=False):
      //self.__strip_type = strip_type
      //if self.__strip_type == self.NEOPIXEL:
      //  self.NAME += " (NeoPixel)"
      //else:
      //    self.NAME += " (DotStar)"

      //self.__num_pixels = num_pixels
      //self.__brightness = brightness
    }

    ~LEDStripModule() {
      delete(led_strip);
      delete(apa_strip);
      delete(__power_good_io);
      delete(__power_en_io);
    }

    virtual void initialise(const SLOT& slot, SlotAccessor& accessor) {
      /*
      // Create the strip driver object
      if self.__strip_type == self.NEOPIXEL:
          from neopixel import NeoPixel
          self.pixels = NeoPixel(slot.FAST4, self.__num_pixels, brightness=self.__brightness, auto_write=False)
      else:
          from adafruit_dotstar import DotStar
          self.pixels = DotStar(slot.FAST3, slot.FAST4, self.__num_pixels, brightness=self.__brightness, auto_write=False)
      */
      led_strip = new WS2812(60, pio0, 0, slot.FAST4);
      led_strip->start(60);
      for(auto i = 0u; i < led_strip->num_leds; ++i) {
        float hue = float(i) / led_strip->num_leds;
        led_strip->set_hsv(i, hue, 1.0f, 1.0f);
      }

      // Create the power control pin objects
      __power_good_io = new IO(slot.FAST1);
      __power_en_io = new IO(slot.FAST2);

      // Configure strip and power pins
      configure();

      // Pass the slot and adc functions up to the parent now that module specific initialisation has finished
      YukonModule::initialise(slot, accessor);
    }

    virtual void configure() {
      __power_en_io->to_output(false);
      __power_good_io->to_input(true, false);
    }

    void enable() {
      if(!is_initialised()) {
        throw std::runtime_error("Module not initialised\n");
      }
      __power_en_io->value(true);
    }

    void disable() {
      if(!is_initialised()) {
        throw std::runtime_error("Module not initialised\n");
      }
      __power_en_io->value(false);
    }

    bool is_enabled() {
      if(!is_initialised()) {
        throw std::runtime_error("Module not initialised\n");
      }
      return __power_en_io->value();
    }

    bool read_power_good() {
      if(!is_initialised()) {
        throw std::runtime_error("Module not initialised\n");
      }
      return __power_good_io->value();
    }

    float read_temperature() {
      return __read_adc2_as_temp();
    }

    virtual void monitor() {
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

      if(__last_pgood && !pgood) {
        logging.warn(__message_header() + "Power is not good\n");
      }
      else if(!__last_pgood && pgood) {
        logging.warn(__message_header() + "Power is good\n");
      }

      // Run some user action based on the latest readings
      //if self.__monitor_action_callback is not None:
      //  self.__monitor_action_callback(pgood, temperature)

      __last_pgood = pgood;
      __power_good_throughout = __power_good_throughout && pgood;

      __max_temperature = MAX(temperature, __max_temperature);
      __min_temperature = MIN(temperature, __min_temperature);
      __avg_temperature += temperature;
      __count_avg += 1;
    }

    virtual std::vector<std::pair<std::string, float>> get_readings() {
      std::vector<std::pair<std::string, float>> values;
      values.push_back(std::pair("PGood", __power_good_throughout));
      values.push_back(std::pair("T_max", __max_temperature));
      values.push_back(std::pair("T_min", __min_temperature));
      values.push_back(std::pair("T_avg", __avg_temperature));
      return values;
    }

    virtual void process_readings() {
      if(__count_avg > 0) {
        __avg_temperature /= __count_avg;
      }
    }

    virtual void clear_readings() {
      __power_good_throughout = true;
      __max_temperature = -std::numeric_limits<float>::infinity();
      __min_temperature = std::numeric_limits<float>::infinity();
      __avg_temperature = 0;
      __count_avg = 0;
    }
  };

}
