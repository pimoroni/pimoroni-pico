#pragma once
#include "pico/stdlib.h"

#include "motor.hpp"
#include "encoder.hpp"
#include "servo.hpp"
#include "ws2812.hpp"
#include "pimoroni_i2c.hpp"

using namespace motor;
using namespace encoder;
using namespace servo;
using namespace plasma;
using namespace pimoroni;


namespace inventor {
  // IO Pin Constants
  const uint GP0 = 0;
  const uint GP1 = 1;
  const uint GP2 = 2;

  const uint A0 = 26;
  const uint A1 = 27;
  const uint A2 = 28;

  const uint GPIOS[] = {GP0, GP1, GP2, A0, A1, A2};
  const uint ADCS[] = {A0, A1, A2};

  // Index Constants
  const uint MOTOR_A = 0;
  const uint MOTOR_B = 1;

  const uint SERVO_1 = 0;
  const uint SERVO_2 = 1;
  const uint SERVO_3 = 2;
  const uint SERVO_4 = 3;
  const uint SERVO_5 = 4;
  const uint SERVO_6 = 5;

  const uint LED_GP0 = 0;
  const uint LED_GP1 = 1;
  const uint LED_GP2 = 2;
  const uint LED_A0 = 3;
  const uint LED_A1 = 4;
  const uint LED_A2 = 5;
  const uint LED_SERVO_1 = 6;
  const uint LED_SERVO_2 = 7;
  const uint LED_SERVO_3 = 8;
  const uint LED_SERVO_4 = 9;
  const uint LED_SERVO_5 = 10;
  const uint LED_SERVO_6 = 11;


  // Count Constants
  const uint NUM_GPIOS = 6;
  const uint NUM_ADCS = 3;
  const uint NUM_MOTORS = 2;
  const uint NUM_SERVOS = 6;
  const uint NUM_LEDS = 12;


  class Inventor2040W {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint AMP_EN_PIN = 3;
    static const uint I2C_SDA_PIN = 4;
    static const uint I2C_SCL_PIN = 5;
    static constexpr pin_pair MOTOR_A_PINS = {6, 7};
    static constexpr pin_pair MOTOR_B_PINS = {8, 9};
    static constexpr pin_pair ENCODER_A_PINS = {19, 18};
    static constexpr pin_pair ENCODER_B_PINS = {17, 16};

    static const uint SERVO_1_PIN = 10;
    static const uint SERVO_2_PIN = 11;
    static const uint SERVO_3_PIN = 12;
    static const uint SERVO_4_PIN = 13;
    static const uint SERVO_5_PIN = 14;
    static const uint SERVO_6_PIN = 15;

    static const uint LED_DATA_PIN = 20;
    static const uint PWM_AUDIO_PIN = 21;
    static const uint USER_SW_PIN = 22;

    static constexpr float AMP_CORRECTION = 4.0f;
    static constexpr float DEFAULT_VOLUME = 0.2f;
    static constexpr float SILENCE_FREQUENCY = 44100.0f;
    static constexpr float DEFAULT_GEAR_RATIO = 50.0f;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  public:
    Motor motors[NUM_MOTORS];
    Encoder encoders[NUM_MOTORS];
    Servo servos[NUM_SERVOS];
    WS2812 leds;
    I2C i2c;

  private:
    float audio_volume;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Inventor2040W(float motor_gear_ratio = DEFAULT_GEAR_RATIO);


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init(bool init_motors = true, bool init_servos = true);

    bool switch_pressed();
    bool play_tone(float frequency);
    void play_silence();
    void stop_playing();

    float volume() const;
    void volume(float volume);

    void mute_audio();
    void unmute_audio();
  };

}