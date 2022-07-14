#include "enviro_urban.hpp"

namespace enviro {

  EnviroUrban::EnviroUrban()
  : EnviroBase()
  , bme280(&i2c, 0x77)
  , pms_i2c(14, 15, 100000) {
  //, pms5003i(&i2c) {
  }

  bool EnviroUrban::init() {
    EnviroBase::init();

    bme280.init();
    //pms5003i.init();

    adc_init();
    adc_gpio_init(MICROPHONE_PIN);

    gpio_set_function(SENSOR_EN, GPIO_FUNC_SIO);
    gpio_set_dir(SENSOR_EN, GPIO_OUT);
    gpio_put(SENSOR_EN, true);

    gpio_set_function(BOOST_EN, GPIO_FUNC_SIO);
    gpio_set_dir(BOOST_EN, GPIO_OUT);
    gpio_put(BOOST_EN, true);

    return true;
  }

  uint EnviroUrban::noise_level() {
    return noise_vpp;
  }

  uint EnviroUrban::particulates(ParticulateMeasure measure) {
    // bit of a fudge to convert decilitres into litres... who uses decilitre?!
    uint multiplier = measure >= PM0_3_PER_LITRE ? 10 : 1;
    return ((particulate_reading[measure * 2] << 8) | particulate_reading[measure * 2 + 1]) * multiplier;
  }

  bool EnviroUrban::take_reading() {
    // get data from particulate sensor
    pms_i2c.read_bytes(0x12, 0, particulate_reading, 32);

    // read sensor data from bme280
    BME280::bme280_reading data = bme280.read_forced();
    temperature_c = data.temperature;
    humidity_pct = data.humidity;
    pressure_hpa = data.pressure / 100.0f;

    // noise
    adc_select_input(0);

    uint32_t sample_time_ms = 100;
    uint32_t start = time();
    uint16_t min_value = 2048, max_value = 2048;
    while(time() - start < sample_time_ms) {
      uint16_t value = adc_read();
      min_value = value < min_value ? value : min_value;
      max_value = value > max_value ? value : max_value;
    }
    noise_vpp = max_value - min_value;

    return true;
  }

  float EnviroUrban::temperature(TemperatureUnits unit) {
    switch(unit) {
      case TemperatureUnits::FAHRENHEIT: {
        return (temperature_c * 9.0f/5.0f) + 32.0f;
      } break;

      case TemperatureUnits::KELVIN: {
        return temperature_c + 273.15f;
      } break;

      default: {
        return temperature_c;
      } break;
    }
  }

  float EnviroUrban::humidity() {
    return humidity_pct;
  }

  float EnviroUrban::pressure(PressureUnits unit) {
    switch(unit) {
      case PressureUnits::BAR: {
        return pressure_hpa / 1000.0f;
      } break;

      case PressureUnits::INHG: {
        return pressure_hpa * 0.029529983071445f;
      } break;

      case PressureUnits::PSI: {
        return pressure_hpa * 0.0145038f;
      } break;

      default: {
        return pressure_hpa;
      } break;
    }
  }
}