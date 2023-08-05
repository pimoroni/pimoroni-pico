#pragma once

#include "../common.hpp"
#include "pwm_cluster.hpp"
#include "io.hpp"

namespace pimoroni {

  class BenchPowerModule : public YukonModule {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const std::string NAME;
    static constexpr float VOLTAGE_MAX = 12.3953f;
    static constexpr float VOLTAGE_MID = 6.5052f;
    static constexpr float VOLTAGE_MIN = 0.6713f;
    static constexpr float VOLTAGE_MIN_MEASURE = 0.1477f;
    static constexpr float VOLTAGE_MID_MEASURE = 1.1706f;
    static constexpr float VOLTAGE_MAX_MEASURE = 2.2007f;
    static constexpr float PWM_MIN = 0.3f;
    static constexpr float PWM_MAX = 0.0f;
    static constexpr float TEMPERATURE_THRESHOLD = 50.0f;


    //--------------------------------------------------
    // Statics
    //--------------------------------------------------
    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);
    TYPE_FUNCTION(BenchPowerModule)


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    bool halt_on_not_pgood;

    //--------------------------------------------------
  private:
    bool last_pgood;
    bool power_good_throughout;
    float max_voltage_out;
    float min_voltage_out;
    float avg_voltage_out;
    float max_temperature;
    float min_temperature;
    float avg_temperature;
    float count_avg;
    

    //--------------------------------------------------
  private:
    PWMCluster* voltage_pwm;
    IO* power_en;
    TCA_IO* power_good;
    uint32_t pwm_period;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BenchPowerModule(bool halt_on_not_pgood = false);
    virtual ~BenchPowerModule();


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
  private:
    void set_pwm(float percent);
  public:
    void set_target_voltage(float voltage);
    void set_target(float percent);
    float read_voltage();
    bool read_power_good();
    float read_temperature();

    //--------------------------------------------------
    virtual void monitor();
    virtual std::vector<std::pair<std::string, float>> get_readings();
    virtual void process_readings();
    virtual void clear_readings();
  };

}
