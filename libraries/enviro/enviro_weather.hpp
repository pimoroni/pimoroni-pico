#pragma once
#include <string>
#include "enviro.hpp"
#include "bme280.hpp"
#include "ltr559.hpp"
#include "analog.hpp"

using namespace pimoroni;


namespace enviro {

  enum WindSpeedUnits {
    MILES_PER_HOUR,
    KILOMETRES_PER_HOUR,
    METRES_PER_SECOND
  };


  class EnviroWeather : public EnviroBase {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint WIND_DIRECTION_PIN = 26;
    static const uint WIND_SPEED_PIN = 9;
    static const uint RAIN_PIN = 10;


    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum WindDirection {
      WIND_DIRECTION_NORTH      = 0,
      WIND_DIRECTION_NORTHEAST  = 45,
      WIND_DIRECTION_EAST       = 90,
      WIND_DIRECTION_SOUTHEAST  = 135,
      WIND_DIRECTION_SOUTH      = 180,
      WIND_DIRECTION_SOUTHWEST  = 225,
      WIND_DIRECTION_WEST       = 270,
      WIND_DIRECTION_NORTHWEST  = 315
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    BME280 bme280;
    LTR559 ltr559;

  private:
    Analog wind_dir_adc;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    EnviroWeather();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    bool take_reading();
    int wind_direction();
    std::string cardinal_wind_direction();
    uint16_t wind_speed(WindSpeedUnits unit = KILOMETRES_PER_HOUR, uint32_t sample_time_ms = 1000);
  };

}
