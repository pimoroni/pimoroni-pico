#include "bsec/inc/bsec_datatypes.h"
#include "bsec/inc/bsec_interface.h"

#include "enviro_indoor.hpp"

namespace enviro {

struct {
  bsec_sensor_configuration_t virtual_sensors[BSEC_NUMBER_OUTPUTS]; 
  bsec_sensor_configuration_t sensor_settings[BSEC_MAX_PHYSICAL_SENSOR];
  bool has_state = false;
  uint8_t state[BSEC_MAX_STATE_BLOB_SIZE];
} bsec;


EnviroIndoor::EnviroIndoor()
: EnviroBase()
, bme688(&i2c, 0x77)
, bh1745(&i2c) {
}

bool EnviroIndoor::init() {
  if(!EnviroBase::init()) return false;
  if(!bme688.init()) return false;
  if(!bh1745.init()) return false;

  return true;
}

bool EnviroIndoor::init_bsec() {
  bsec.virtual_sensors[0].sensor_id = BSEC_OUTPUT_IAQ;
  bsec.virtual_sensors[1].sensor_id = BSEC_OUTPUT_STATIC_IAQ;
  bsec.virtual_sensors[2].sensor_id = BSEC_OUTPUT_CO2_EQUIVALENT;
  bsec.virtual_sensors[3].sensor_id = BSEC_OUTPUT_BREATH_VOC_EQUIVALENT;
  bsec.virtual_sensors[4].sensor_id = BSEC_OUTPUT_RAW_TEMPERATURE;
  bsec.virtual_sensors[5].sensor_id = BSEC_OUTPUT_RAW_PRESSURE;
  bsec.virtual_sensors[6].sensor_id = BSEC_OUTPUT_RAW_HUMIDITY;
  bsec.virtual_sensors[7].sensor_id = BSEC_OUTPUT_RAW_GAS;
  bsec.virtual_sensors[8].sensor_id = BSEC_OUTPUT_STABILIZATION_STATUS;
  bsec.virtual_sensors[9].sensor_id = BSEC_OUTPUT_RUN_IN_STATUS;
  bsec.virtual_sensors[10].sensor_id = BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE;
  bsec.virtual_sensors[11].sensor_id = BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY;
  bsec.virtual_sensors[12].sensor_id = BSEC_OUTPUT_COMPENSATED_GAS;
  bsec.virtual_sensors[13].sensor_id = BSEC_OUTPUT_GAS_PERCENTAGE;

  for(size_t i = 0; i < BSEC_NUMBER_OUTPUTS; i++) {
    bsec.virtual_sensors[i].sample_rate = BSEC_SAMPLE_RATE_LP;  
  }

  // initialise the bosch sensortech library
  printf("bsec_init();\n");
  bsec_init();

  printf("bsec_update_subscription();\n");
  uint8_t sensor_settings_count = BSEC_MAX_PHYSICAL_SENSOR;
  bsec_library_return_t status = bsec_update_subscription(bsec.virtual_sensors, BSEC_NUMBER_OUTPUTS, bsec.sensor_settings, &sensor_settings_count);
  if(status != BSEC_OK) {
    printf("FAIL: bsec_update_subscription();\n");
    return false;
  }
  printf("nSensorSettings = %d\n", sensor_settings_count);

  // TODO: do we need to set a configuration here? it is optional but should check what options there are
  // bsec_set_configuration()

  // TODO: in future state will be stored in flash between sleep states
  if(bsec.has_state) {
    uint8_t _work_buffer[BSEC_MAX_STATE_BLOB_SIZE];
    status = bsec_set_state(bsec.state, BSEC_MAX_STATE_BLOB_SIZE, _work_buffer, BSEC_MAX_STATE_BLOB_SIZE);

    if(status != BSEC_OK) {
      printf("FAIL:  bsec_set_state(state, BSEC_MAX_STATE_BLOB_SIZE, workBuffer, sizeof(workBuffer))\n");
  }

  }

  return true;
}

bool EnviroIndoor::take_reading() {
  uint64_t timestamp_ns = time() * 1000000;
  
  printf("calling bsec_sensor_control with %llu timestamp\n", timestamp_ns);

  bsec_library_return_t status;
  bsec_bme_settings_t sensor_settings;
  status = bsec_sensor_control(timestamp_ns, &sensor_settings);
/*
  if(status == BSEC_W_SC_CALL_TIMING_VIOLATION) {
    printf("FAIL: bsec_sensor_control() TIMING VIOLATION\n");
    return false;
  }else if(status != BSEC_OK) {
    printf("FAIL: bsec_sensor_control() status = %d:\n", status);
    return false;
  }    */

  if(status < 0) {
    return false;
  }

  // read sensor data from bme688
  bme68x_data data;
  if(!bme688.read_forced(&data)) {
    return false;
  }
  temperature_c = data.temperature;
  humidity_pct = data.humidity;
  pressure_hpa = data.pressure / 100.0f;
  gas_resistance_ohm = data.gas_resistance;

  // pass bme688 sensor results to the bsec algorithm to geneate
  // air quality results

  timestamp_ns = time() * 1000000;

  printf("  - sensor settings:\n");
  printf("    - next call in %llums\n", (sensor_settings.next_call - timestamp_ns) / 1000000);

  bsec_input_t inputs[BSEC_MAX_PHYSICAL_SENSOR];

  inputs[0].sensor_id = BSEC_INPUT_TEMPERATURE;
  inputs[0].signal = temperature_c;
  inputs[0].time_stamp = timestamp_ns;

  inputs[1].sensor_id = BSEC_INPUT_HEATSOURCE;
  inputs[1].signal = 0; // temperature offset (due to external heat sources?)
  inputs[1].time_stamp = timestamp_ns;

  inputs[2].sensor_id = BSEC_INPUT_HUMIDITY;
  inputs[2].signal = humidity_pct;
  inputs[2].time_stamp = timestamp_ns;

  inputs[3].sensor_id = BSEC_INPUT_PRESSURE;
  inputs[3].signal = pressure_hpa;
  inputs[3].time_stamp = timestamp_ns;

  inputs[4].sensor_id = BSEC_INPUT_GASRESISTOR;
  inputs[4].signal = gas_resistance_ohm;
  inputs[4].time_stamp = timestamp_ns;

  bsec_output_t outputs[BSEC_NUMBER_OUTPUTS];
  uint8_t output_count = BSEC_NUMBER_OUTPUTS;

  status = bsec_do_steps(inputs, 5, outputs, &output_count);
  if (status != BSEC_OK) {
    printf("FAIL: status = bsec_do_steps(inputs, 5, outputs, &output_count) - status = %d\n", status);
    return false;
  }

  printf("OUTPUT COUNT %d\n", output_count);
  for (uint i = 0; i < output_count; i++) {
    uint8_t sensor_id = outputs[i].sensor_id;
    float signal = outputs[i].signal;
    uint8_t accuracy = outputs[i].accuracy;

    // list of sensor ids and the values they return

    // air quality values range from 0..500 they are based on historical data and
    // can take a few days to fully calibrate. 25 is considered good air and 250 
    // is considered bad air - with values ranging inbetween.
    // - BSEC_OUTPUT_IAQ (signal 0..500 (25 = good, 250 = bad) + accuracy)
    // - BSEC_OUTPUT_STATIC_IAQ (signal 0..500 (25 = good, 250 = bad) + accuracy)

    // gas measurements returned as parts per million (ppm)
    // - BSEC_OUTPUT_CO2_EQUIVALENT (signal = ppm + accuracy)
    // - BSEC_OUTPUT_BREATH_VOC_EQUIVALENT (signal = ppm + accuracy)

    // the raw sensor values (are affected by the heater)
    // - BSEC_OUTPUT_RAW_TEMPERATURE (signal)
    // - BSEC_OUTPUT_RAW_PRESSURE (signal)
    // - BSEC_OUTPUT_RAW_HUMIDITY (signal)
    // - BSEC_OUTPUT_RAW_GAS (signal)

    // status flags
    // - BSEC_OUTPUT_STABILIZATION_STATUS (signal = 1.0f if stabilised)
    // - BSEC_OUTPUT_RUN_IN_STATUS (signal = 1.0f if stabilised)

    // compensated versions of the raw sensor values taking into account the
    // effect of the heater
    // - BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE (signal)
    // - BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY (signal)
    // - BSEC_OUTPUT_COMPENSATED_GAS (signal + accuracy)
    // - BSEC_OUTPUT_GAS_PERCENTAGE (signal + accuracy)


    switch(sensor_id) {
      case BSEC_OUTPUT_IAQ: {
        printf("BSEC_OUTPUT_IAQ = %.2f (%d%%)\n", signal, accuracy);
      } break;

      case BSEC_OUTPUT_STATIC_IAQ: {
        printf("BSEC_OUTPUT_STATIC_IAQ = %.2f (%d%%)\n", signal, accuracy);
      } break;

      case BSEC_OUTPUT_CO2_EQUIVALENT: {
        printf("BSEC_OUTPUT_CO2_EQUIVALENT = %.2f (%d%%)\n", signal, accuracy);
      } break;

      case BSEC_OUTPUT_BREATH_VOC_EQUIVALENT: {
        printf("BSEC_OUTPUT_BREATH_VOC_EQUIVALENT = %.2f (%d%%)\n", signal, accuracy);
      } break;
      
      case BSEC_OUTPUT_STABILIZATION_STATUS: {
        printf("BSEC_OUTPUT_STABILIZATION_STATUS = %s\n", signal == 1.0f ? "STABLE" : "STABILISING");
      } break;

      case BSEC_OUTPUT_RUN_IN_STATUS: {
        printf("BSEC_OUTPUT_RUN_IN_STATUS = %s\n", signal == 1.0f ? "RUN IN" : "RUNNING IN");
      } break;

      case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE: {
        printf("BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE = %.2f (%d%%)\n", signal, accuracy);
      } break;

      case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY: {
        printf("BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY = %.2f (%d%%)\n", signal, accuracy);
      } break;

      case BSEC_OUTPUT_COMPENSATED_GAS: {
        printf("BSEC_OUTPUT_COMPENSATED_GAS = %.2f (%d%%)\n", signal, accuracy);
      } break;

      case BSEC_OUTPUT_GAS_PERCENTAGE: {
        printf("BSEC_OUTPUT_GAS_PERCENTAGE = %.2f (%d%%)\n", signal, accuracy);
      } break;
    };

  }

  // get the current state of the bsec algorithm and save it for reuse later
  uint8_t _work_buffer[BSEC_MAX_STATE_BLOB_SIZE];
  uint32_t serialized_state_size = BSEC_MAX_STATE_BLOB_SIZE;
  status = bsec_get_state(0, bsec.state, BSEC_MAX_STATE_BLOB_SIZE, _work_buffer, BSEC_MAX_STATE_BLOB_SIZE, &serialized_state_size); 
  bsec.has_state = true;

  // TODO: no success return flag from bh1745 driver, not consistent with bme688
  bh1745.set_measurement_time_ms(160);
  light = bh1745.get_rgbc_raw();

  return true;
}

float EnviroIndoor::temperature(TemperatureUnits unit) {
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

float EnviroIndoor::humidity() {
  return humidity_pct;
}

float EnviroIndoor::pressure(PressureUnits unit) {
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
