#pragma once

#include "../common.hpp"
#include "motor_cluster.hpp"
#include "encoder.hpp"

using namespace motor;
using namespace encoder;

namespace pimoroni {

  class BigMotorModule : public YukonModule {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const std::string NAME;
    static const uint NUM_MOTORS = 2;
    static constexpr float DEFAULT_FREQUENCY = 25000.0f;
    static constexpr float TEMPERATURE_THRESHOLD = 50.0f;
    static constexpr float CURRENT_THRESHOLD = 25.0f;
    static constexpr float SHUNT_RESISTOR = 0.001f;
    static constexpr float GAIN = 80.0f;


    //--------------------------------------------------
    // Statics
    //--------------------------------------------------
    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);
    TYPE_FUNCTION(BigMotorModule)


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    float frequency;

    //--------------------------------------------------
    bool fault_triggered;
    float max_current;
    float min_current;
    float avg_current;
    float max_temperature;
    float min_temperature;
    float avg_temperature;
    float count_avg;

    //--------------------------------------------------
  public:
    MotorCluster* motor;
    Encoder* encoder;
  private:
    TCA_IO* motor_en;
    TCA_IO* motor_nfault;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BigMotorModule(float frequency = DEFAULT_FREQUENCY);
    virtual ~BigMotorModule();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    virtual std::string name();
    virtual void initialise(const SLOT& slot, SlotAccessor& accessor);
    virtual void reset();

    //--------------------------------------------------
    void enable();
    void disable();
    bool is_enabled();
    bool read_fault();
    bool read_current();
    float read_temperature();

    //--------------------------------------------------
    virtual void monitor();
    virtual std::vector<std::pair<std::string, float>> get_readings();
    virtual void process_readings();
    virtual void clear_readings();
  };

}
