#pragma once

#include "../common.hpp"

namespace pimoroni {

  class ProtoPotModule : public YukonModule {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const std::string NAME;


    //--------------------------------------------------
    // Statics
    //--------------------------------------------------
    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);
    TYPE_FUNCTION(ProtoPotModule)


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    ProtoPotModule();
    virtual ~ProtoPotModule();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    virtual std::string name();

    //--------------------------------------------------
    float read();
  };

  class ProtoPotModule2 : public YukonModule {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const std::string NAME;
  private:
    static constexpr float PULLUP = 5100.0f;


    //--------------------------------------------------
    // Statics
    //--------------------------------------------------
    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);
    TYPE_FUNCTION(ProtoPotModule2)


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    float sum;
    float const_a;
    float const_b;
    float const_c;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    ProtoPotModule2(float pot_resistance);
    virtual ~ProtoPotModule2();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    virtual std::string name();

    //--------------------------------------------------
    float read();
  };

}
