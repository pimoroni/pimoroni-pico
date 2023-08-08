#pragma once

#include "../common.hpp"
#include "io.hpp"

namespace pimoroni {

  class DualSwitchedModule : public YukonModule {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const std::string NAME;  // Required by all modules. Set in .cpp
    static const uint NUM_SWITCHES = 2;
    static constexpr float TEMPERATURE_THRESHOLD = 50.0f;


    //--------------------------------------------------
    // Statics
    //--------------------------------------------------
    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);
    TYPE_FUNCTION(DualSwitchedModule)


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    bool halt_on_not_pgood;

    //--------------------------------------------------
  private:
    bool last_pgood1;
    bool last_pgood2;
    bool power_good_throughout1;
    bool power_good_throughout2;
    float max_temperature;
    float min_temperature;
    float avg_temperature;
    float count_avg;

    //--------------------------------------------------
  private:
    IO* sw_output[2];
    IO* sw_enable[2];
    TCA_IO* power_good[2];


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    DualSwitchedModule(bool halt_on_not_pgood = false);
    virtual ~DualSwitchedModule();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    virtual std::string name();
    virtual void initialise(const SLOT& slot, SlotAccessor& accessor);
    virtual void reset();

    //--------------------------------------------------
    void enable(uint output);
    void disable(uint output);
    bool is_enabled(uint output);
    void output(uint output, bool val);
    bool read_output(uint output);

    bool read_power_good(uint output);
    float read_temperature();

    //--------------------------------------------------
    virtual void monitor();
    virtual std::vector<std::pair<std::string, float>> get_readings();
    virtual void process_readings();
    virtual void clear_readings();
  };

}
