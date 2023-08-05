#pragma once

#include "../common.hpp"

namespace pimoroni {

  class AudioAmpModule : public YukonModule {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const std::string NAME;
    static const uint8_t AMP_I2C_ADDRESS = 0x38;
    static constexpr float TEMPERATURE_THRESHOLD = 50.0f;


    //--------------------------------------------------
    // Statics
    //--------------------------------------------------
    static bool is_module(uint adc_level, bool slow1, bool slow2, bool slow3);
    TYPE_FUNCTION(AudioAmpModule)


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    bool halt_on_not_pgood;

    //--------------------------------------------------
  private:
    bool last_pgood1;
    bool last_pgood2;
    bool power_good_throughout1;
    bool power_good_throughout2;
    float max_temperature;
    float min_temperature;
    float avg_temperature;
    float count_avg;

    //--------------------------------------------------
  public:
    uint I2S_DATA;
    uint I2S_CLK;
    uint I2S_FS;
  private:
    TCA_IO* slow_sda;
    TCA_IO* slow_scl;
    TCA_IO* amp_en;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    AudioAmpModule();
    virtual ~AudioAmpModule();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    virtual std::string name();
    virtual void initialise(const SLOT& slot, SlotAccessor& accessor);
    virtual void configure();

    //--------------------------------------------------
    void enable();
    void disable();
    bool is_enabled();

    void exit_soft_shutdown();
    void set_volume(float volume);
    float read_temperature();

    //--------------------------------------------------
    virtual void monitor();
    virtual std::vector<std::pair<std::string, float>> get_readings();
    virtual void process_readings();
    virtual void clear_readings();

    //--------------------------------------------------
  private:
    void start_i2c();
    void end_i2c();
    void write_i2c_byte(uint8_t number);
    uint8_t read_i2c_byte();
    void write_i2c_reg(uint8_t reg, uint8_t data);
    uint8_t read_i2c_reg(uint8_t reg);
    uint8_t detect_i2c();
  };

}
