#pragma once

#include "pico_graphics.hpp"
#include "common/pimoroni_common.hpp"
#include "drivers/tca9555/tca9555.hpp"

namespace pimoroni {

  struct TCA {
    uint CHIP;
    uint GPIO;
  };

  struct SLOT {
    uint ID;
    uint FAST1;
    uint FAST2;
    uint FAST3;
    uint FAST4;
    TCA SLOW1;
    TCA SLOW2;
    TCA SLOW3;
    uint ADC1_ADDR;
    uint ADC2_TEMP_ADDR;

    // Needed for use with std::map
    bool operator<(const SLOT& o) const {
      return (ID < o.ID);
    }
  };

  class Module {
    uint type;
  };


  class OverVoltageError : public std::exception {
  private:
    std::string message;

  public:
    OverVoltageError(std::string msg) : message("OverVoltageError: " + msg) {
    }

    virtual const char* what() const noexcept {
      return message.c_str();
    }
  };

  class UnderVoltageError : public std::exception {
  private:
    std::string message;

  public:
    UnderVoltageError(std::string msg) : message("UnderVoltageError: " + msg) {
    }

    virtual const char* what() const noexcept {
      return message.c_str();
    }
  };

  class OverCurrentError : public std::exception {
  private:
    std::string message;

  public:
    OverCurrentError(std::string msg) : message("OverCurrentError: " + msg) {
    }

    virtual const char* what() const noexcept {
      return message.c_str();
    }
  };

  class OverTemperatureError : public std::exception {
  private:
    std::string message;

  public:
    OverTemperatureError(std::string msg) : message("OverTemperatureError: " + msg) {
    }

    virtual const char* what() const noexcept {
      return message.c_str();
    }
  };

  class FaultError : public std::exception {
  private:
    std::string message;

  public:
    FaultError(std::string msg) : message("FaultError: " + msg) {
    }

    virtual const char* what() const noexcept {
      return message.c_str();
    }
  };

  class VerificationError : public std::exception {
  private:
    std::string message;

  public:
    VerificationError(std::string msg) : message(msg) {
    }

    virtual const char* what() const noexcept {
      return ("VerificationError: " + message).c_str();
    }
  };

  class Yukon {
  public:
    static const SLOT SLOT1;
    static const SLOT SLOT2;
    static const SLOT SLOT3;
    static const SLOT SLOT4;
    static const SLOT SLOT5;
    static const SLOT SLOT6;

    static const uint NUM_SLOTS = 6;

    static const uint SDA = 24;
    static const uint SCL = 25;

    static const uint GP26 = 26;
    static const uint GP27 = 27;

    static const uint INT = 28;

    static const uint SHARED_ADC = 29;

    static const TCA MAIN_EN;  // {0, 6}
    static const TCA USER_SW;  // {0, 7}

    static const TCA ADC_ADDR_1;  // {0, 12}
    static const TCA ADC_ADDR_2;  // {0, 13}
    static const TCA ADC_ADDR_3;  // {0, 14}
    static const TCA ADC_MUX_EN_1;  // {0, 15}
    static const TCA ADC_MUX_EN_2;  // {0, 11}

    static const TCA SW_A;  // {1, 1}
    static const TCA SW_B;  // {1, 2}
    static const TCA LED_A;  // {1, 3}
    static const TCA LED_B;  // {1, 4}

    static const TCA LCD_BL;  // {1, 0}
    static const TCA LCD_DC;  // {1, 8}
    static const TCA LCD_CS;  // {1, 9}

    static const uint CURRENT_SENSE_ADDR = 12;  // 0b1100
    static const uint TEMP_SENSE_ADDR = 13;     // 0b1101
    static const uint VOLTAGE_SENSE_ADDR = 14;  // 0b1110
    static const uint EX_ADC_ADDR = 15;         // 0b1111

  private:
    static constexpr float VOLTAGE_MAX = 17.0f;
    static constexpr float VOLTAGE_MIN_MEASURE = 0.030f;
    static constexpr float VOLTAGE_MAX_MEASURE = 2.294f;

    static constexpr float CURRENT_MAX = 10.0f;
    static constexpr float CURRENT_MIN_MEASURE = 0.0147f;
    static constexpr float CURRENT_MAX_MEASURE = 0.9307f;

    static constexpr float DEFAULT_VOLTAGE_LIMIT = 17.2f;
    static constexpr float VOLTAGE_LOWER_LIMIT = 4.8f;
    static constexpr float VOLTAGE_ZERO_LEVEL = 0.05f;
    static constexpr float DEFAULT_CURRENT_LIMIT = 20.0f;
    static constexpr float DEFAULT_TEMPERATURE_LIMIT = 90.0f;
    static constexpr float ABSOLUTE_MAX_VOLTAGE_LIMIT = 18.0f;
  private:
    I2C i2c;
    TCA9555 tca0;
    TCA9555 tca1;

    static const uint NUM_EXPANDERS = 2;

    enum LoggingLevel {
      LOG_INFO = 2
    };

    float voltage_limit;
    float current_limit;
    float temperature_limit;
    uint logging_level;
    std::map<SLOT, Module*> slot_assignments;
    void* monitor_action_callback;

    struct readings {
      float max_voltage;
      float min_voltage;
      float avg_voltage;

      float max_current;
      float min_current;
      float avg_current;

      float max_temperature;
      float min_temperature;
      float avg_temperature;
    } measures;
    float count_avg;

  public:
    Yukon(float voltage_limit = DEFAULT_VOLTAGE_LIMIT,
          float current_limit = DEFAULT_CURRENT_LIMIT,
          float temperature_limit = DEFAULT_TEMPERATURE_LIMIT,
          uint logging_level = LOG_INFO);
    ~Yukon();

    void init();
    void reset();

  private:
    TCA9555& get_tca_chip(uint chip);

  public:
    bool get_slow_input(TCA gpio);
    bool get_slow_output(TCA gpio);
    bool get_slow_config(TCA gpio);
    bool get_slow_polarity(TCA gpio);

    void set_slow_output(TCA gpio, bool value);
    void set_slow_config(TCA gpio, bool output);
    void set_slow_polarity(TCA gpio, bool polarity);

    void change_output_mask(uint8_t chip, uint16_t mask, uint16_t state);

    //--------------------------------------------------

    void change_logging(uint logging_level);

    SLOT __check_slot(uint slot_id);
    SLOT __check_slot(SLOT slot);

    std::vector<uint> find_slots_with_module(std::type_info module_type);

    void register_with_slot(Module* module, uint slot_id);
    void register_with_slot(Module* module, SLOT slot);

    void deregister_slot(uint slot_id);
    void deregister_slot(SLOT slot);

    uint __match_module(uint adc_level, bool slow1, bool slow2, bool slow3);
    std::type_info* __detect_module(uint slot_id);
    std::type_info* __detect_module(SLOT slot);
    uint detect_module(uint slot_id);
    uint detect_module(SLOT slot);

    void __expand_slot_list(std::vector<SLOT> slot_list);
    void __verify_modules(bool allow_unregistered, bool allow_undetected, bool allow_discrepencies, bool allow_no_modules);

    void initialise_modules(bool allow_unregistered = false, bool allow_undetected = false, bool allow_discrepencies = false, bool allow_no_modules = false);

    bool is_pressed(uint button);
    bool is_boot_pressed();
    void set_led(uint button, bool value);

    void enable_main_output();
    void __enable_main_output();
    void disable_main_output();
    bool is_main_output();

    void __deselect_address();
    void __select_address(uint8_t address);
    float __shared_adc_voltage();

    float read_voltage();
    float read_current();
    float read_temperature();
    float read_expansion();
    float read_slot_adc1(SLOT slot);
    float read_slot_adc2(SLOT slot);
    float time();

    void assign_monitor_action(void* callback_function);

    void monitor();
    void monitored_sleep_ms(uint32_t ms, std::vector<std::string> allowed = std::vector<std::string>(), std::vector<std::string> excluded = std::vector<std::string>());
    void monitor_until_ms(uint32_t end_ms, std::vector<std::string> allowed = std::vector<std::string>(), std::vector<std::string> excluded = std::vector<std::string>());

    void __print_readings(std::vector<std::string> allowed, std::vector<std::string> excluded);
    std::vector<std::pair<std::string, float>> get_readings();

    void process_readings();
    void __clear_readings();
    void clear_readings();

    void __print_map(std::string section_name, std::vector<std::pair<std::string, float>> readings, std::vector<std::string> allowed, std::vector<std::string> excluded);
    //void reset();
  };

}