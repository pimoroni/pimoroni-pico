#pragma once
#include "enviro.hpp"
#include "bme280.hpp"
//#include "pms5003i.hpp"
#include "hardware/adc.h" //TODO replace with analog class

using namespace pimoroni;


namespace enviro {

  enum ParticulateMeasure {
    PM1_UGM3                = 2,
    PM2_5_UGM3              = 3,
    PM10_UGM3               = 4,
    PM1_UGM3_ATHMOSPHERIC   = 5,
    PM2_5_UGM3_ATHMOSPHERIC = 6,
    PM10_UGM3_ATHMOSPHERIC  = 7,
    PM0_3_PER_LITRE         = 8,
    PM0_5_PER_LITRE         = 9,
    PM1_PER_LITRE           = 10,
    PM2_5_PER_LITRE         = 11,
    PM5_PER_LITRE           = 12,
    PM10_PER_LITRE          = 13,
  };


  class EnviroUrban : public EnviroBase {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint MICROPHONE_PIN = 26;
    static const uint SENSOR_RESET = 9;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    BME280 bme280;
    //PMS5003I pms5003i;

  protected:
    float temperature_c; // temperate in celcius
    float humidity_pct; // humidty as a percentage
    float pressure_hpa; // pressure in hectopascals
    uint16_t noise_vpp; // noise adc channel peak to peak
    uint8_t particulate_reading[32];


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    EnviroUrban();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    bool take_reading();
    uint particulates(ParticulateMeasure measure);
    uint noise_level();
    float temperature(TemperatureUnits unit = CELCIUS);
    float humidity();
    float pressure(PressureUnits unit = HECTOPASCAL);
  };

}
