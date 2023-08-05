#pragma once

#include "../common.hpp"
#include "servo_cluster.hpp"
#include "io.hpp"

using namespace servo;

namespace pimoroni {

  class QuadServoRegModule : public YukonModule {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const std::string NAME;
    static const uint NUM_SERVOS = 4;
    static constexpr float TEMPERATURE_THRESHOLD = 50.0f;


    //--------------------------------------------------
    // Statics
    //--------------------------------------------------
    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);
    TYPE_FUNCTION(QuadServoRegModule)


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
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
    ServoCluster* servos;
  private:
    TCA_IO* power_en;
    TCA_IO* power_good;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    QuadServoRegModule(bool halt_on_not_pgood = false);
    virtual ~QuadServoRegModule();


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
