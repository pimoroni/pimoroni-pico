#include <math.h>

#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"

#include <stdexcept>
#include <algorithm>

#include "yukon.hpp"



namespace pimoroni {

  const SLOT Yukon::SLOT1 = {
    1,        // ID
    0,        // FAST1
    1,        // FAST2
    2,        // FAST3
    3,        // FAST4
    {0, 3},   // SLOW1
    {0, 4},   // SLOW2
    {0, 5},   // SLOW3
    0,        // ADC1_ADDR (0b0000)
    3,        // ADC2_TEMP_ADDR (0b0011)
  };

  const SLOT Yukon::SLOT2 = {
    2,        // ID
    4,        // FAST1
    5,        // FAST2
    6,        // FAST3
    7,        // FAST4
    {0, 0},   // SLOW1
    {0, 1},   // SLOW2
    {0, 2},   // SLOW3
    1,        // ADC1_ADDR (0b0001)
    6,        // ADC2_TEMP_ADDR (0b0110)
  };

  const SLOT Yukon::SLOT3 = {
    3,        // ID
    8,        // FAST1
    9,        // FAST2
    10,       // FAST3
    11,       // FAST4
    {0, 8},   // SLOW1
    {0, 9},   // SLOW2
    {0, 10},  // SLOW3
    4,        // ADC1_ADDR (0b0100)
    2,        // ADC2_TEMP_ADDR (0b0010)
  };

  const SLOT Yukon::SLOT4 = {
    4,        // ID
    12,       // FAST1
    13,       // FAST2
    14,       // FAST3
    15,       // FAST4
    {1, 7},   // SLOW1
    {1, 5},   // SLOW2
    {1, 6},   // SLOW3
    5,        // ADC1_ADDR (0b0101)
    7,        // ADC2_TEMP_ADDR (0b0111)
  };

  const SLOT Yukon::SLOT5 = {
    5,        // ID
    16,       // FAST1
    17,       // FAST2
    18,       // FAST3
    19,       // FAST4
    {1, 15},  // SLOW1
    {1, 14},  // SLOW2
    {1, 13},  // SLOW3
    8,        // ADC1_ADDR (0b1000)
    11,       // ADC2_TEMP_ADDR (0b1011)
  };

  const SLOT Yukon::SLOT6 = {
    6,        // ID
    20,       // FAST1
    21,       // FAST2
    22,       // FAST3
    23,       // FAST4
    {1, 10},  // SLOW1
    {1, 12},  // SLOW2
    {1, 11},  // SLOW3
    9,        // ADC1_ADDR (0b1001)
    10,       // ADC2_TEMP_ADDR (0b1010)
  };

  const TCA Yukon::MAIN_EN = {0, 6};
  const TCA Yukon::USER_SW = {0, 7};

  const TCA Yukon::ADC_ADDR_1 = {0, 12};
  const TCA Yukon::ADC_ADDR_2 = {0, 13};
  const TCA Yukon::ADC_ADDR_3 = {0, 14};
  const TCA Yukon::ADC_MUX_EN_1 = {0, 15};
  const TCA Yukon::ADC_MUX_EN_2 = {0, 11};

  const TCA Yukon::SW_A = {1, 1};
  const TCA Yukon::SW_B = {1, 2};
  const TCA Yukon::LED_A = {1, 3};
  const TCA Yukon::LED_B = {1, 4};

  const TCA Yukon::LCD_BL = {1, 0};
  const TCA Yukon::LCD_DC = {1, 8};
  const TCA Yukon::LCD_CS = {1, 9};

  Yukon::Yukon(float voltage_limit,
               float current_limit,
               float temperature_limit,
               uint logging_level) :
    i2c(24, 25),
    tca0(&i2c, 0x20),
    tca1(&i2c, 0x26),
    voltage_limit(MIN(voltage_limit, ABSOLUTE_MAX_VOLTAGE_LIMIT)),
    current_limit(current_limit),
    temperature_limit(temperature_limit),
    allowed_readings(),
    excluded_readings() {
    logging.level = logging_level;

    slot_assignments[SLOT1] = nullptr;
    slot_assignments[SLOT2] = nullptr;
    slot_assignments[SLOT3] = nullptr;
    slot_assignments[SLOT4] = nullptr;
    slot_assignments[SLOT5] = nullptr;
    slot_assignments[SLOT6] = nullptr;
  }

  Yukon::~Yukon() {
  }

  void Yukon::init() {
    tca0.init();
    tca1.init();


    reset();
  }

  void Yukon::reset() {
    // Only disable the output if enabled (avoids duplicate messages)
    if(is_main_output()) {
      disable_main_output();
    }

    // Set the first IO expander's initial state
    tca0.set_output_port(0x0000);
    tca0.set_polarity_port(0x0000);
    tca0.set_config_port(0x07BF);

    // Set the second IO expander's initial state
    tca1.set_output_port(0x0000);
    tca1.set_polarity_port(0x0000);
    tca1.set_config_port(0xFCE6);

    set_slow_output(MAIN_EN, false);
    set_slow_config(MAIN_EN, true);

    set_slow_config(USER_SW, false);

    set_slow_output(ADC_MUX_EN_1, false);
    set_slow_config(ADC_MUX_EN_1, true);

    set_slow_output(ADC_MUX_EN_2, false);
    set_slow_config(ADC_MUX_EN_2, true);

    set_slow_output(ADC_ADDR_1, false);
    set_slow_config(ADC_ADDR_1, true);

    set_slow_output(ADC_ADDR_2, false);
    set_slow_config(ADC_ADDR_2, true);

    set_slow_output(ADC_ADDR_3, false);
    set_slow_config(ADC_ADDR_3, true);

    set_slow_config(SW_A, false);
    set_slow_config(SW_B, false);

    set_slow_output(LED_A, false);
    set_slow_config(LED_A, true);

    set_slow_output(LED_B, false);
    set_slow_config(LED_B, true);

    if (!(adc_hw->cs & ADC_CS_EN_BITS)) adc_init();

    //Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(SHARED_ADC);

    //Select ADC input 0 (GPIO26)
    adc_select_input(SHARED_ADC - 26);

    __clear_readings();

    monitor_action_callback = nullptr;
  }

  TCA9555& Yukon::get_tca_chip(uint chip) {
    assert(chip < NUM_EXPANDERS);
    return (chip == 0) ? tca0 : tca1;
  }

  bool Yukon::get_slow_input(TCA gpio) {
    return get_tca_chip(gpio.CHIP).get_gpio_input(gpio.GPIO);
  }

  bool Yukon::get_slow_output(TCA gpio) {
    return get_tca_chip(gpio.CHIP).get_gpio_output(gpio.GPIO);
  }

  bool Yukon::get_slow_config(TCA gpio) {
    return get_tca_chip(gpio.CHIP).get_gpio_config(gpio.GPIO);
  }

  bool Yukon::get_slow_polarity(TCA gpio) {
    return get_tca_chip(gpio.CHIP).get_gpio_polarity(gpio.GPIO);
  }

  void Yukon::set_slow_output(TCA gpio, bool value) {
    get_tca_chip(gpio.CHIP).set_gpio_output(gpio.GPIO, value);
  }
  void Yukon::set_slow_config(TCA gpio, bool output) {
    get_tca_chip(gpio.CHIP).set_gpio_config(gpio.GPIO, output);
  }

  void Yukon::set_slow_polarity(TCA gpio, bool polarity) {
    get_tca_chip(gpio.CHIP).set_gpio_polarity(gpio.GPIO, polarity);
  }

  void Yukon::change_output_mask(uint8_t chip, uint16_t mask, uint16_t state) {
    get_tca_chip(chip).change_output_mask(mask, state);
  }

  void Yukon::change_logging(uint logging_level) {
    logging.level = logging_level;
  }

  SLOT Yukon::__check_slot(uint slot_id) {
    if(slot_id < 1 || slot_id > NUM_SLOTS) {
      throw std::invalid_argument("slot id out of range. Expected 1 to 6\n");
    }

    auto it = slot_assignments.begin();
    std::advance(it, slot_id - 1);
    return it->first;
  }

  SLOT Yukon::__check_slot(SLOT slot) {
    if(slot_assignments.find(slot) == slot_assignments.end()) {
      throw std::invalid_argument("slot is not a valid slot object\n");
    }
    return slot;
  }

  std::vector<uint> Yukon::find_slots_with_module(const ModuleType& module_type) {
    if(is_main_output()) {
      throw std::runtime_error("Cannot find slots with modules whilst the main output is active\n");
    }

    logging.info("> Finding slots with '" + module_type.NAME + "' module\n");

    std::vector<uint> slot_ids;
    for(auto it = slot_assignments.begin(); it != slot_assignments.end(); it++) {
      SLOT slot = it->first;
      logging.info("[Slot" + std::to_string(slot.ID) + "] ");
      const ModuleType* detected = __detect_module(slot);

      if(detected != nullptr && detected->TYPE == module_type.TYPE) {
        logging.info("Found '" + detected->NAME + "' module\n");
        slot_ids.push_back(slot.ID);
      }
      else {
        logging.info("No '" + module_type.NAME + "' module\n");
      }
    }

    return slot_ids;
  }

  void Yukon::register_with_slot(YukonModule* module, uint slot_id) {
    SLOT slot = __check_slot(slot_id);
    register_with_slot(module, slot);
  }

  void Yukon::register_with_slot(YukonModule* module, SLOT slot) {
    if(is_main_output()) {
      throw std::runtime_error("Cannot register modules with slots whilst the main output is active\n");
    }

    slot = __check_slot(slot);

    if(slot_assignments[slot] == nullptr)
      slot_assignments[slot] = module;
    else
      throw std::invalid_argument("The selected slot is already populated\n");
  }

  void Yukon::deregister_slot(uint slot_id) {
    SLOT slot = __check_slot(slot_id);
    deregister_slot(slot);
  }

  void Yukon::deregister_slot(SLOT slot) {
    if(is_main_output()) {
      throw std::runtime_error("Cannot deregister module slots whilst the main output is active\n");
    }

    slot = __check_slot(slot);

    YukonModule* module = slot_assignments[slot];
    if(module != nullptr) {
      module->deregister();
      slot_assignments[slot] = nullptr;
    }
  }

  const ModuleType* Yukon::__match_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    for(uint i = 0; i < count_of(KNOWN_MODULES); i++) {
      const ModuleType& m = KNOWN_MODULES[i];
      if(m.is_module(adc_level, slow1, slow2, slow3)) {
        return &m;
      }
    }
    return nullptr;
  }

  const ModuleType* Yukon::__detect_module(SLOT slot) {
    set_slow_config(slot.SLOW1, false);
    set_slow_config(slot.SLOW2, false);
    set_slow_config(slot.SLOW3, false);

    get_slow_input(USER_SW);

    __select_address(slot.ADC1_ADDR);
    float adc_val = 0.0f;
    for(uint i = 0; i < DETECTION_SAMPLES; i++) {
      adc_val += __shared_adc_voltage();
    }
    adc_val /= DETECTION_SAMPLES;

    bool slow1 = get_slow_input(slot.SLOW1);
    bool slow2 = get_slow_input(slot.SLOW2);
    bool slow3 = get_slow_input(slot.SLOW3);

    logging.debug("ADC1 = " + std::to_string(adc_val) + ", SLOW1 = " + std::to_string((int)slow1) + ", SLOW2 = " + std::to_string((int)slow2) + ", SLOW3 = " + std::to_string((int)slow3) + ", ");

    uint adc_level = ADC_FLOAT;
    if(adc_val <= DETECTION_ADC_LOW) {
      adc_level = ADC_LOW;
    }
    else if(adc_val >= DETECTION_ADC_HIGH) {
      adc_level = ADC_HIGH;
    }

    const ModuleType* detected = __match_module(adc_level, slow1, slow2, slow3);

    __deselect_address();

    return detected;
  }
  const ModuleType* Yukon::detect_module(uint slot_id) {
    if(is_main_output()) {
      throw std::runtime_error("Cannot detect modules whilst the main output is active\n");
    }

    SLOT slot = __check_slot(slot_id);

    return __detect_module(slot);
  }

  const ModuleType* Yukon::detect_module(SLOT slot) {
    if(is_main_output()) {
      throw std::runtime_error("Cannot detect modules whilst the main output is active\n");
    }

    slot = __check_slot(slot);

    return __detect_module(slot);
  }

  void __expand_slot_list(std::vector<SLOT> slot_list) {

  }
  void Yukon::__verify_modules(bool allow_unregistered, bool allow_undetected, bool allow_discrepencies, bool allow_no_modules) {
    //TODO Take the allowed parameters and expand them into slot lists that are easier to compare against
    //allow_unregistered = self.__expand_slot_list(allow_unregistered)
    //allow_undetected = self.__expand_slot_list(allow_undetected)
    //allow_discrepencies = self.__expand_slot_list(allow_discrepencies)

    bool raise_unregistered = false;
    bool raise_undetected = false;
    bool raise_discrepency = false;
    uint8_t unregistered_slots = 0;

    for(auto it = slot_assignments.begin(); it != slot_assignments.end(); it++) {
      SLOT slot = it->first;
      YukonModule* module = it->second;

      logging.info("[Slot" + std::to_string(slot.ID) + "] ");
      const ModuleType* detected = __detect_module(slot);

      if(detected == nullptr) {
        if(module != nullptr) {
          logging.info("No module detected! Expected a '" + module->name() + "' module.\n");
          if(!allow_undetected) {//if slot not in allow_undetected:
            raise_undetected = true;
          }
        }
        else {
          logging.info("Module slot is empty.\n");
          unregistered_slots += 1;
        }
      }
      else {
        if(module != nullptr) {
          if(std::type_index(typeid(*module)) == detected->TYPE) {
            logging.info("'" + module->name() + "' module detected and registered.\n");
          }
          else {
            logging.info("Module discrepency! Expected a '" + module->name() + "}' module, but detected a '" + detected->NAME + "' module.\n");
            if(!allow_discrepencies) { //if slot not in allow_discrepencies:
              raise_discrepency = true;
            }
          }
        }
        else {
          logging.info("'" + detected->NAME + "' module detected but not registered.\n");
          if(!allow_unregistered) { //if slot not in allow_unregistered:
            raise_unregistered = true;
          }
          unregistered_slots += 1;
        }
      }
    }

    if(!allow_no_modules && unregistered_slots == NUM_SLOTS) {
      throw VerificationError("No modules have been registered with Yukon. At least one module needs to be registered to enable the output\n");
    }

    if(raise_discrepency) {
      throw VerificationError("Detected a different combination of modules than what was registered with Yukon. Please check the modules attached to your board and the program you are running.\n");
    }

    if(raise_undetected) {
      throw VerificationError("Some or all modules registered with Yukon have not been detected. Please check that the modules are correctly attached to your board or disable this warning.");
    }

    if(raise_unregistered) {
      throw VerificationError("Detected modules that have not been registered with Yukon, which could behave unexpectedly when connected to power. Please remove these modules or disable this warning.");
    }

    logging.info("\n");  // New line
  }

  void Yukon::initialise_modules(bool allow_unregistered, bool allow_undetected, bool allow_discrepencies, bool allow_no_modules) {
    if(is_main_output()) {
      throw std::runtime_error("Cannot verify modules whilst the main output is active\n");
    }

    logging.info("> Verifying modules\n");

    __verify_modules(allow_unregistered, allow_undetected, allow_discrepencies, allow_no_modules);

    logging.info("> Initialising modules\n");

    for(auto it = slot_assignments.begin(); it != slot_assignments.end(); it++) {
      SLOT slot = it->first;
      YukonModule* module = it->second;

      if(module != nullptr) {
        logging.info("[Slot" + std::to_string(slot.ID) + " '" + module->name() + "'] Initialising ... ");
        module->initialise(slot, *this);
        logging.info("done\n");
      }
    }

    logging.info("\n");  // New line
  }

  bool Yukon::is_pressed(uint button) {
    if(button > 1) {
      throw std::invalid_argument("switch out of range. Expected SWITCH_A (0) or SWITCH_B (1)\n");
    }

    return !get_slow_input(button ? SW_B : SW_A);
  }

  bool Yukon::is_boot_pressed() {
    return !get_slow_input(USER_SW);
  }

  void Yukon::set_led(uint button, bool value) {
    if(button > 1) {
      throw std::invalid_argument("switch out of range. Expected SWITCH_A (0) or SWITCH_B (1)\n");
    }

    set_slow_output(button ? LED_B : LED_A, value);
  }

  void Yukon::enable_main_output() {
    if(!is_main_output()) {
      absolute_time_t start = get_absolute_time();

      __select_address(VOLTAGE_SENSE_ADDR);

      float old_voltage = ((__shared_adc_voltage() - VOLTAGE_MIN_MEASURE) * VOLTAGE_MAX) / (VOLTAGE_MAX_MEASURE - VOLTAGE_MIN_MEASURE);
      old_voltage = MAX(old_voltage, 0.0f);
      uint64_t first_stable_time = 0;
      float new_voltage = 0.0f;
      const int64_t dur = 100 * 1000;
      const int64_t dur_b = 5 * 1000;

      logging.info("> Enabling output ...\n");
      __enable_main_output();
      while(true) {
        new_voltage = ((__shared_adc_voltage() - VOLTAGE_MIN_MEASURE) * VOLTAGE_MAX) / (VOLTAGE_MAX_MEASURE - VOLTAGE_MIN_MEASURE);
        if(new_voltage > ABSOLUTE_MAX_VOLTAGE_LIMIT) {
          disable_main_output();
          throw OverVoltageError("[Yukon] Input voltage exceeded a safe level! Turning off output\n");
        }

        absolute_time_t new_time = get_absolute_time();
        if(fabsf(new_voltage - old_voltage) < 0.1f) { // Had to increase from 0.05 for some reason
          if(first_stable_time == 0) {
            first_stable_time = new_time;
          }
          else if(absolute_time_diff_us(first_stable_time, new_time) > dur_b) {
            break;
          }
        }
        else {
          first_stable_time = 0;
        }

        if(absolute_time_diff_us(start, new_time) > dur) {
          disable_main_output();
          throw FaultError("[Yukon] Output voltage did not stablise in an acceptable time. Turning off output\n");
        }

        old_voltage = new_voltage;
      }

      if(new_voltage < VOLTAGE_ZERO_LEVEL) {
        disable_main_output();
        throw UnderVoltageError("[Yukon] No input voltage detected! Make sure power is being provided to the XT-30 (yellow) connector\n");
      }

      if(new_voltage < VOLTAGE_LOWER_LIMIT) {
        disable_main_output();
        throw UnderVoltageError("[Yukon] Input voltage below minimum operating level. Turning off output\n");
      }

      clear_readings();

      logging.info("> Output enabled\n");
    } 
  }
  void Yukon::__enable_main_output() {
    set_slow_output(MAIN_EN, true);
  }
  void Yukon::disable_main_output() {
    set_slow_output(MAIN_EN, false);
    logging.info("> Output disabled\n");
  }

  bool Yukon::is_main_output() {
    return get_slow_output(MAIN_EN);
  }

  void Yukon::__deselect_address() {
    set_slow_output(ADC_MUX_EN_1, false);
    set_slow_output(ADC_MUX_EN_2, false);
  }

  void Yukon::__select_address(uint8_t address) {
    if (address < 0)
      return; //raise ValueError("address is less than zero")
    else if(address > 0b1111)
      return; //raise ValueError("address is greater than number of available addresses")
    else {
      uint16_t state = 0x0000;

      if(address & 0b0001) {
        state |= 1 << ADC_ADDR_1.GPIO;
      }

      if(address & 0b0010) {
        state |= 1 << ADC_ADDR_2.GPIO;
      }

      if(address & 0b0100) {
        state |= 1 << ADC_ADDR_3.GPIO;
      }

      if(address & 0b1000) {
        state |= 1 << ADC_MUX_EN_1.GPIO;
      }
      else {
        state |= 1 << ADC_MUX_EN_2.GPIO;
      }

      uint16_t adc_io_mask = (1 << ADC_MUX_EN_1.GPIO) | (1 << ADC_MUX_EN_2.GPIO) | (1 << ADC_ADDR_1.GPIO) | (1 << ADC_ADDR_2.GPIO) | (1 << ADC_ADDR_3.GPIO);
      change_output_mask(ADC_ADDR_1.CHIP, adc_io_mask, state);
    }
    sleep_us(10); // Add a delay to let the pins settle before taking a reading
  }

  float Yukon::__shared_adc_voltage() {
    adc_select_input(SHARED_ADC - 26);
    return ((float)adc_read() * 3.3f) / (1 << 12);
  }

  float Yukon::read_voltage() {
    __select_address(VOLTAGE_SENSE_ADDR);
    // return (shared_adc_voltage() * (100 + 16)) / 16  # Old equation, kept for reference
    float voltage = ((__shared_adc_voltage() - VOLTAGE_MIN_MEASURE) * VOLTAGE_MAX) / (VOLTAGE_MAX_MEASURE - VOLTAGE_MIN_MEASURE);
    return MAX(voltage, 0.0f);
  }

  float Yukon::read_current() {
    __select_address(CURRENT_SENSE_ADDR);
    // return (shared_adc_voltage() - 0.015) / ((1 + (5100 / 27.4)) * 0.0005)  # Old equation, kept for reference
    float current =((__shared_adc_voltage() - CURRENT_MIN_MEASURE) * CURRENT_MAX) / (CURRENT_MAX_MEASURE - CURRENT_MIN_MEASURE);
    return MAX(current, 0.0f);
  }

  float Yukon::read_temperature() {
    __select_address(TEMP_SENSE_ADDR);
    float sense = __shared_adc_voltage();
    float r_thermistor = sense / ((3.3f - sense) / 5100);
    static constexpr float ROOM_TEMP = 273.15f + 25.0f;
    static constexpr float RESISTOR_AT_ROOM_TEMP = 10000.0f;
    static constexpr float BETA = 3435.0f;
    float t_kelvin = (BETA * ROOM_TEMP) / (BETA + (ROOM_TEMP * log(r_thermistor / RESISTOR_AT_ROOM_TEMP)));
    float t_celsius = t_kelvin - 273.15f;

    // https://www.allaboutcircuits.com/projects/measuring-temperature-with-an-ntc-thermistor/
    return t_celsius;
  }

  float Yukon::read_expansion() {
    __select_address(EX_ADC_ADDR);
    return __shared_adc_voltage();
  }

  float Yukon::read_slot_adc1(SLOT slot) {
    __select_address(slot.ADC1_ADDR);
    return __shared_adc_voltage();
  }

  float Yukon::read_slot_adc2(SLOT slot) {
    __select_address(slot.ADC2_TEMP_ADDR);
    return __shared_adc_voltage();
  }
  /*
  void assign_monitor_action(void* callback_function);
*/
  void Yukon::monitor() {
    float voltage = read_voltage();
    if(voltage > voltage_limit) {
      disable_main_output();
      throw OverVoltageError("[Yukon] Voltage of " + std::to_string(voltage) + "V exceeded the user set level of " + std::to_string(voltage_limit) + "V! Turning off output\n");
    }

    if(voltage < VOLTAGE_LOWER_LIMIT) {
      disable_main_output();
      throw UnderVoltageError("[Yukon] Voltage of " + std::to_string(voltage) + "V below minimum operating level. Turning off output\n");
    }

    float current = read_current();
    if(current > current_limit) {
      disable_main_output();
      throw OverCurrentError("[Yukon] Current of " + std::to_string(current) + "A exceeded the user set level of " + std::to_string(current_limit) + "A! Turning off output\n");
    }

    float temperature = read_temperature();
    if(temperature > temperature_limit) {
      disable_main_output();
      throw OverTemperatureError("[Yukon] Temperature of " + std::to_string(temperature) + "°C exceeded the user set level of " + std::to_string(temperature_limit) + "°C! Turning off output\n");
    }

    // Run some user action based on the latest readings
    //if __monitor_action_callback is not None:
    //  __monitor_action_callback(voltage, current, temperature);

    for(auto it = slot_assignments.begin(); it != slot_assignments.end(); it++) {
      YukonModule* module = it->second;
      if(module != nullptr) {
        try {
          module->monitor();
        }
        catch(const std::exception& e) {
          disable_main_output();
          throw;  // Now the output is off, let the exception continue into user code
        }
      }
    }

    readings.max_voltage = MAX(voltage, readings.max_voltage);
    readings.min_voltage = MIN(voltage, readings.min_voltage);
    readings.avg_voltage += voltage;

    readings.max_current = MAX(current, readings.max_current);
    readings.min_current = MIN(current, readings.min_current);
    readings.avg_current += current;

    readings.max_temperature = MAX(temperature, readings.max_temperature);
    readings.min_temperature = MIN(temperature, readings.min_temperature);
    readings.avg_temperature += temperature;

    count_avg += 1;
  }

  void Yukon::monitored_sleep(float seconds) {
    if(seconds < 0.0f) {
      throw std::invalid_argument("sleep length must be non-negative");
    }

    // Convert and handle the sleep as milliseconds
    monitored_sleep_ms((uint32_t)(1000.0f * seconds + 0.5f));
  }

  void Yukon::monitored_sleep_ms(uint32_t ms) {
    // Calculate the time this sleep should end at, and monitor until then
    monitor_until(make_timeout_time_ms(ms));
  }

  void Yukon::monitor_until(absolute_time_t end) {
    // Clear any readings from previous monitoring attempts
    clear_readings();

    // Perform monitor check(s) until the end time is reached
    do {
      monitor();
      tight_loop_contents();
    } while(absolute_time_diff_us(get_absolute_time(), end) > 0);

    // Process any readings that need it (e.g. averages)
    process_readings();

    if(logging.level >= LOG_INFO) {
      print_readings();
    }
  }

  void Yukon::monitor_once() {
    // Clear any readings from previous monitoring attempts
    clear_readings();

    // Perform a single monitor check
    monitor();

    // Process any readings that need it (e.g. averages)
    process_readings();

    if(logging.level >= LOG_INFO) {
      print_readings();
    }
  }

  void Yukon::print_readings() {
    __print_named_readings("[Yukon]", get_readings());

    for(auto it = slot_assignments.begin(); it != slot_assignments.end(); it++) {
      SLOT slot = it->first;
      YukonModule* module = it->second;

      if(module != nullptr) {
        __print_named_readings("[Slot" + std::to_string(slot.ID) + "]", module->get_readings());
      }
    }
    std::cout << std::endl;
    //printf("\n");
  }

  std::vector<std::pair<std::string, float>> Yukon::get_readings() {
    //std::map<std::string, float> values;
    std::vector<std::pair<std::string, float>> values;
    values.push_back(std::pair("V_max", readings.max_voltage));
    values.push_back(std::pair("V_min", readings.min_voltage));
    values.push_back(std::pair("V_avg", readings.avg_voltage));
    values.push_back(std::pair("C_max", readings.max_current));
    values.push_back(std::pair("C_min", readings.min_current));
    values.push_back(std::pair("C_avg", readings.avg_current));
    values.push_back(std::pair("T_max", readings.max_temperature));
    values.push_back(std::pair("T_min", readings.min_temperature));
    values.push_back(std::pair("T_avg", readings.avg_temperature));
    return values;
  }

  void Yukon::process_readings() {
    if(count_avg > 0) {
      readings.avg_voltage /= count_avg;
      readings.avg_current /= count_avg;
      readings.avg_temperature /= count_avg;
    }

    for(auto it = slot_assignments.begin(); it != slot_assignments.end(); it++) {
      YukonModule* module = it->second;
      if(module != nullptr) {
        module->process_readings();
      }
    }
  }

  void Yukon::__clear_readings() {
      readings.max_voltage = -std::numeric_limits<float>::infinity();
      readings.min_voltage = std::numeric_limits<float>::infinity();
      readings.avg_voltage = 0;

      readings.max_current = -std::numeric_limits<float>::infinity();
      readings.min_current = std::numeric_limits<float>::infinity();
      readings.avg_current = 0;

      readings.max_temperature = -std::numeric_limits<float>::infinity();
      readings.min_temperature = std::numeric_limits<float>::infinity();
      readings.avg_temperature = 0;

      count_avg = 0;
  }
  void Yukon::clear_readings() {
    __clear_readings();

    for(auto it = slot_assignments.begin(); it != slot_assignments.end(); it++) {
      YukonModule* module = it->second;
      if(module != nullptr) {
        module->clear_readings();
      }
    }
  }

  void Yukon::allow_reading(std::string name) {
    if(!in_allowed(name) && !in_excluded(name)) {
      allowed_readings.push_back(name);
    }
  }
  void Yukon::allow_readings(std::list<std::string> names) {
    for(auto it = names.begin(); it != names.end(); it++) {
      allow_reading(*it);
    }
  }

  void Yukon::exclude_reading(std::string name) {
    if(!in_allowed(name) && !in_excluded(name)) {
      excluded_readings.push_back(name);
    }
  }
  void Yukon::exclude_readings(std::list<std::string> names) {
    for(auto it = names.begin(); it != names.end(); it++) {
      exclude_reading(*it);
    }
  }

  void Yukon::clear_filters() {
    allowed_readings.clear();
    excluded_readings.clear();
  }

  bool Yukon::in_allowed(std::string name) {
    return std::find(allowed_readings.begin(), allowed_readings.end(), name) != allowed_readings.end();
  }
  bool Yukon::in_excluded(std::string name) {
    return std::find(excluded_readings.begin(), excluded_readings.end(), name) != excluded_readings.end();
  }

  void Yukon::__print_named_readings(std::string section_name, std::vector<std::pair<std::string, float>> named_readings) {
    if(!named_readings.empty()) {
      std::cout << section_name << " ";
      //printf("%s ", section_name.c_str());

      for(auto it = named_readings.begin(); it != named_readings.end(); it++) {
        std::string name = it->first;
        float value = it->second;

        if((allowed_readings.empty() || in_allowed(name)) && !in_excluded(name)) {
          //if type(value) is bool:
          //  print(f"{name} = {int(value)},", end=" ")  # Output 0 or 1 rather than True of False, so bools can appear on plotter charts
          //else:
            //printf("%s = %f, ", name.c_str(), value);
            std::cout << name << " = " << value << ", ";
        }
      }
    }
  }

  //void reset();
}
