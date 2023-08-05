#pragma once

#include "../common.hpp"
#include "motor_cluster.hpp"

using namespace motor;

namespace pimoroni {

  class DualMotorModule : public YukonModule {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const std::string NAME;
    static const uint NUM_MOTORS = 2;
    static const uint NUM_STEPPERS = 1;
    static constexpr float FAULT_THRESHOLD = 0.1f;
    static constexpr float DEFAULT_FREQUENCY = 25000.0f;
    static constexpr float TEMPERATURE_THRESHOLD = 50.0f;


    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum MotorType {
      DUAL = 0,
      STEPPER = 1
    };


    //--------------------------------------------------
    // Statics
    //--------------------------------------------------
    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);
    TYPE_FUNCTION(DualMotorModule)


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    const MotorType motor_type;
    float frequency;

    //--------------------------------------------------
  private:
    bool fault_triggered;
    float max_temperature;
    float min_temperature;
    float avg_temperature;
    float count_avg;

    //--------------------------------------------------
  private:
    MotorCluster* motors;
    TCA_IO* motors_decay;
    TCA_IO* motors_toff;
    TCA_IO* motors_en;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    DualMotorModule(float frequency = DEFAULT_FREQUENCY);
    virtual ~DualMotorModule();


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
    bool decay();
    void decay(bool val);
    bool toff();
    void toff(bool val);
    bool read_fault();
    float read_temperature();

    //--------------------------------------------------
    virtual void monitor();
    virtual std::vector<std::pair<std::string, float>> get_readings();
    virtual void process_readings();
    virtual void clear_readings();
  };

}
