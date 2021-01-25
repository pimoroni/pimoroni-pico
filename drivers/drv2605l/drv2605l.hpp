#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace pimoroni {

  class DRV2605L {
  public:
    static const uint8_t FB_MODE_ERM = 0x00;
    static const uint8_t FB_MODE_LRA = 0x80;    

    static const uint8_t LIB_EMPTY    = 0;    
    static const uint8_t LIB_TS2200_A = 1;    
    static const uint8_t LIB_TS2200_B = 2;    
    static const uint8_t LIB_TS2200_C = 3;    
    static const uint8_t LIB_TS2200_D = 4;    
    static const uint8_t LIB_TS2200_E = 5;    
    static const uint8_t LIB_LRA      = 6;    
    static const uint8_t LIB_TS2200_F = 7;

    static const uint8_t MODE_INTERNAL_TRIGGER  = 0; //Waveforms are fired by setting the GO bit in register 0x0C
    static const uint8_t MODE_EDGE_TRIGGER      = 1; //A rising edge on INT/TRIG sets the GO bit, a second rising edge cancels the waveform
    static const uint8_t MODE_LEVEL_TRIGGER     = 2; //A rising edge on INT/TRIG sets the GO bit, a falling edge cancels the waveform
    static const uint8_t MODE_PWM_ANALOG_IN     = 3; //A PWM or Analog signal is accepted on INT/TRIG and used as a direct driving source
    static const uint8_t MODE_AUDIO_IN          = 4; //An AC-coupled audio signal is accepted on INT/TRIG and turned into meaningful vibration
                                                     //(AC_COUPLE and N_PWM_ANALOG should also be set)
    static const uint8_t MODE_REALTIME_PLAYBACK = 5; //The contents of the REALTIME_PLAYBACK register are used as a waveform
    static const uint8_t MODE_DIAGNOSTICS       = 6; //Perform a diagnostic test on the actuator- triggered by setting the GO bit
    static const uint8_t MODE_AUTO_CALIBTRATION = 7; //Perform an auto-calibration- triggered by setting the GO bit

    static const uint8_t RT_SIGNED = 0;
    static const uint8_t RT_UNSIGNED = 1;

    static const uint8_t LOOP_GAIN_LOW    = 0 << 2;
    static const uint8_t LOOP_GAIN_MED    = 1 << 2;
    static const uint8_t LOOP_GAIN_HIGH   = 2 << 2;
    static const uint8_t LOOP_GAIN_VHIGH  = 3 << 2;

    static const uint8_t FBF_1  = 0 << 4;
    static const uint8_t FBF_2  = 1 << 4;
    static const uint8_t FBF_3  = 2 << 4;
    static const uint8_t FBF_4  = 3 << 4;
    static const uint8_t FBF_6  = 4 << 4;
    static const uint8_t FBF_8  = 5 << 4;
    static const uint8_t FBF_16 = 6 << 4;
    static const uint8_t FBF_0  = 7 << 4;

    static const uint8_t AUTO_CALIB_TIME_150  = 0 << 4;
    static const uint8_t AUTO_CALIB_TIME_250  = 1 << 4;
    static const uint8_t AUTO_CALIB_TIME_500  = 2 << 4;
    static const uint8_t AUTO_CALIB_TIME_1000 = 3 << 4;

    static const uint8_t ZERO_CROSS_TIME_100 = 0 << 6;
    static const uint8_t ZERO_CROSS_TIME_200 = 1 << 6;
    static const uint8_t ZERO_CROSS_TIME_300 = 2 << 6;
    static const uint8_t ZERO_CROSS_TIME_390 = 3 << 6;

  private:
    i2c_inst_t *i2c = i2c0;

    // interface pins with our standard defaults where appropriate
    int8_t address   = 0x5a;
    int8_t sda       = 20;
    int8_t scl       = 21;
    int8_t interrupt = 22;

  public:
    DRV2605L() {}

    DRV2605L(i2c_inst_t *i2c, uint8_t sda, uint8_t scl, uint8_t interrupt) :
      i2c(i2c), sda(sda), scl(scl), interrupt(interrupt) {}

    void init();
    void reset();

    void set_feedback_mode(uint8_t mode = FB_MODE_LRA);
    void set_library(uint8_t library = LIB_LRA);
    void set_mode(uint8_t mode);
    void auto_calibrate(uint8_t loop_gain = LOOP_GAIN_HIGH,
                       uint8_t feedback_brake_factor = FBF_2,
                       uint8_t auto_calibration_time = AUTO_CALIB_TIME_1000,
                       uint8_t zero_crossing_detection_time = ZERO_CROSS_TIME_100,
                       uint8_t idiss_time = 1);

    void set_realtime_input(uint8_t value);
    void set_realtime_data_format(uint8_t value);
    void set_waveform(uint8_t slot, uint8_t w);

    void go();
    void stop();
    bool busy();

    void i2c_reg_write_uint8(uint8_t reg, uint8_t value);
    uint8_t i2c_reg_read_uint8(uint8_t reg);
    int16_t i2c_reg_read_int16(uint8_t reg);
  };

}
