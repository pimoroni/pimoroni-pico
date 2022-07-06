#include <cfloat>
#include <math.h>

#include "enviro_weather.hpp"

namespace enviro {

EnviroWeather::EnviroWeather()
: EnviroBase()
, bme280(&i2c, 0x77)
, ltr559(&i2c)
, wind_dir_adc(WIND_DIRECTION_PIN) {
}

bool EnviroWeather::init() {
  // Attempt to initialise our base
  if(!EnviroBase::init())
    return false;

  // Attempt to initialise our i2c sensors
  if(!bme280.init() || !ltr559.init())
    return false;

  // Set up the wind speed pin
  gpio_set_function(WIND_SPEED_PIN, GPIO_FUNC_SIO);
  gpio_set_dir(WIND_SPEED_PIN, GPIO_IN);
  gpio_pull_up(WIND_SPEED_PIN);

  // Set up the rain pin
  gpio_set_function(RAIN_PIN, GPIO_FUNC_SIO);
  gpio_set_dir(RAIN_PIN, GPIO_IN);
  gpio_pull_down(RAIN_PIN);

  if(gpio_get(RAIN_PIN))
    woken_up_by = REASON_RAIN_TRIGGER;

  return true;
}

bool EnviroWeather::take_reading() {
  return false;
}

int EnviroWeather::wind_direction() {
  // adc reading voltage to cardinal direction taken from our python
  // library - each array index represents a 45 degree step around
  // the compass (index 0 == 0, 1 == 45, 2 == 90, etc.)
  // we find the closest matching value in the array and use the index
  // to determine the heading
  const float adc_to_degrees[8] = {
    0.9, 2.0, 3.0, 2.8, 2.5, 1.5, 0.3, 0.6
  };

  float value = wind_dir_adc.read_voltage();
  int closest_index = -1;
  float closest_value = FLT_MAX;
  
  for(size_t i = 0; i < 8; i++) {
    float distance = abs(adc_to_degrees[i] - value);
    if(distance < closest_value) {
      closest_value = distance;
      closest_index = i;
    }
  }

  return closest_index * 45;
}

std::string EnviroWeather::cardinal_wind_direction() {
  switch(wind_direction()) {
    case WIND_DIRECTION_NORTH:     {return  "N"; } break;
    case WIND_DIRECTION_NORTHEAST: {return "NE"; } break;
    case WIND_DIRECTION_EAST:      {return  "E"; } break;
    case WIND_DIRECTION_SOUTHEAST: {return "SE"; } break;
    case WIND_DIRECTION_SOUTH:     {return  "S"; } break;
    case WIND_DIRECTION_SOUTHWEST: {return "SW"; } break;
    case WIND_DIRECTION_WEST:      {return  "W"; } break;
    case WIND_DIRECTION_NORTHWEST: {return "NW"; } break;
  };
  return "";
}

uint16_t EnviroWeather::wind_speed(WindSpeedUnits unit, uint32_t sample_time_ms) {
  uint32_t start = time();

  bool was = gpio_get(WIND_SPEED_PIN);
  uint32_t ticks = 0;
  while(time() - start < sample_time_ms) {
    bool now = gpio_get(WIND_SPEED_PIN);
    if(!was && now) {
      ticks++;
    }
    was = now;
  }

  const float radius = 7.0f;
  const float circumference = radius * 2.0f * M_PI;
  const float factor = 0.0218f; // scaling factor for wind speed in m/s
  float tick_hz = (ticks * 1000.0f) / (sample_time_ms * 2.0f); // two ticks per rotation

  // Calculate the wind speed in metres per second
  float wind_m_s = tick_hz * circumference * factor;

  switch(unit) {
    case WindSpeedUnits::MILES_PER_HOUR: {
      return ((wind_m_s * 60 * 60) / 1000.0f) * 0.621371f;
    } break;

    case WindSpeedUnits::KILOMETRES_PER_HOUR: {
      return (wind_m_s * 60 * 60) / 1000.0f;
    } break;

    default: {
      return wind_m_s;
    } break;
  }
}
}
