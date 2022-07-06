#pragma once
#include "enviro.hpp"
#include "bme280.hpp"
#include "ltr559.hpp"

using namespace pimoroni;


namespace enviro {
  // Index Constants
  const uint SENSOR_A = 0;
  const uint SENSOR_B = 1;
  const uint SENSOR_C = 2;

  const uint PUMP_A = 0;
  const uint PUMP_B = 1;
  const uint PUMP_C = 2;

  // Count Constants
  const uint NUM_SENSORS = 3;
  const uint NUM_PUMPS = 3;


  class EnviroGrow : public EnviroBase {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint SENSOR_PINS[];
    static const uint PUMP_PINS[];
    static const uint PIEZO_PIN = 28;

    static constexpr float DEFAULT_FREQUENCY = 1000.0f;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    BME280 bme280;
    LTR559 ltr559;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    EnviroGrow();

    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    bool take_reading();
    uint sensor_ticks(uint sensor, uint32_t sample_time_ms = 100);
    void pump(uint pump, bool on);

    bool play_tone(float frequency);
    void stop_playing();
  };

}
