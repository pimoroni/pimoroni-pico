#pragma once
#include "enviro.hpp"
#include "bme68x.hpp"
#include "bh1745.hpp"

using namespace pimoroni;


namespace enviro {

  class EnviroIndoor : public EnviroBase {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    // Nothing at the moment


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    BME68X bme688;
    BH1745 bh1745;

    bool has_reading = false;

  private:
    float temperature_c; // temperate in celcius
    float humidity_pct; // humidty as a percentage
    float pressure_hpa; // pressure in hectopascals
    float gas_resistance_ohm; // gas resistance reading in ohms
    rgbc_t light; // r, g, b, & c light sensor channels


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    EnviroIndoor();

    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();
    bool init_bsec();

    bool take_reading();
    float temperature(TemperatureUnits unit = CELCIUS);
    float humidity();
    float pressure(PressureUnits unit = HECTOPASCAL);
  };

}
