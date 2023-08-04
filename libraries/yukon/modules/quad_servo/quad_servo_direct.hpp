#pragma once

#include "../common.hpp"
#include "servo_cluster.hpp"

using namespace servo;
namespace pimoroni {

  class QuadServoDirectModule : public YukonModule {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const std::string NAME;
    static const uint NUM_SERVOS = 4;


    //--------------------------------------------------
    // Statics
    //--------------------------------------------------
    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);
    TYPE_FUNCTION(QuadServoDirectModule)


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    ServoCluster* servos;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    QuadServoDirectModule();
    virtual ~QuadServoDirectModule();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    virtual std::string name();
    virtual void initialise(const SLOT& slot, SlotAccessor& accessor);
    virtual void configure();

    //--------------------------------------------------
    float read_adc1();
    float read_adc2();
  };

}
