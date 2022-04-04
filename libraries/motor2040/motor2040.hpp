#pragma once
#include "pico/stdlib.h"

#include "ws2812.hpp"
#include "motor2.hpp"
#include "motor_cluster.hpp"

namespace motor {
  namespace motor2040 {
    const uint MOTOR_1P = 4;
    const uint MOTOR_1N = 5;
    const uint MOTOR_2P = 6;
    const uint MOTOR_2N = 7;
    const uint MOTOR_3P = 8;
    const uint MOTOR_3N = 9;
    const uint MOTOR_4P = 10;
    const uint MOTOR_4N = 11;

    const MotorPins MOTOR_1(MOTOR_1P, MOTOR_1N);
    const MotorPins MOTOR_2(MOTOR_2P, MOTOR_2N);
    const MotorPins MOTOR_3(MOTOR_3P, MOTOR_3N);
    const MotorPins MOTOR_4(MOTOR_4P, MOTOR_4N);
    const uint NUM_MOTORS = 4;

    const uint ENCODER_1A = 0;
    const uint ENCODER_1B = 1;
    const uint ENCODER_2A = 2;
    const uint ENCODER_2B = 3;
    const uint ENCODER_3A = 12;
    const uint ENCODER_3B = 13;
    const uint ENCODER_4A = 14;
    const uint ENCODER_4B = 15;

    const EncoderPins ENCODER_1(ENCODER_1A, ENCODER_1B);
    const EncoderPins ENCODER_2(ENCODER_2A, ENCODER_2B);
    const EncoderPins ENCODER_3(ENCODER_3A, ENCODER_3B);
    const EncoderPins ENCODER_4(ENCODER_4A, ENCODER_4B);
    const uint NUM_ENCODERS = 4;

    const uint TX_TRIG = 16;
    const uint RX_ECHO = 17;

    const uint LED_DATA = 18;
    const uint NUM_LEDS = 1;

    const uint USER_SW = 23;

    const uint ADC_ADDR_0 = 22;
    const uint ADC_ADDR_1 = 24;
    const uint ADC_ADDR_2 = 25;

    const uint ADC0 = 26;
    const uint ADC1 = 27;
    const uint ADC2 = 28;
    const uint SHARED_ADC = 29; // The pin used for the board's sensing features

    const uint CURRENT_SENSE_1_ADDR = 0b000;
    const uint CURRENT_SENSE_2_ADDR = 0b001;
    const uint CURRENT_SENSE_3_ADDR = 0b010;
    const uint CURRENT_SENSE_4_ADDR = 0b011;
    const uint VOLTAGE_SENSE_ADDR = 0b100;
    const uint FAULT_SENSE_ADDR = 0b101;

    const uint SENSOR_1_ADDR = 0b110;
    const uint SENSOR_2_ADDR = 0b111;
    const uint NUM_SENSORS = 2;

    constexpr float SHUNT_RESISTOR = 0.47f;
    constexpr float CURRENT_GAIN = 5;
    constexpr float VOLTAGE_GAIN = 3.9f / 13.9f;
    constexpr float CURRENT_OFFSET = -0.02f;
  }
}