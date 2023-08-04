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
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const std::string NAME;  // Required by all modules. Set in .cpp
    static constexpr float TEMPERATURE_THRESHOLD = 50.0f;


    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum StripType {
      NEOPIXEL = 0,
      DOTSTAR = 1
    };


    //--------------------------------------------------
    // Statics
    //--------------------------------------------------
    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);
    TYPE_FUNCTION(LEDStripModule)


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    const StripType strip_type;
    const uint num_pixels;
    const float brightness;
  public:
    bool halt_on_not_pgood;

    //--------------------------------------------------
  private:
    bool last_pgood;
    bool power_good_throughout;
    float max_temperature;
    float min_temperature;
    float avg_temperature;
    float count_avg;

    //--------------------------------------------------
  public:
    WS2812* ws_pixels;
    APA102* apa_pixels;
  private:
    IO* power_good;
    IO* power_en;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    LEDStripModule(StripType strip_type, uint num_pixels, float brightness = 1.0f, bool halt_on_not_pgood = false);
    virtual ~LEDStripModule();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    virtual std::string name();
    virtual void initialise(const SLOT& slot, SlotAccessor& accessor);
    virtual void configure();

    //--------------------------------------------------
    void enable();
    void disable();
    bool is_enabled();
    bool read_power_good();
    float read_temperature();

    //--------------------------------------------------
    virtual void monitor();
    virtual std::vector<std::pair<std::string, float>> get_readings();
    virtual void process_readings();
    virtual void clear_readings();
  };

}
