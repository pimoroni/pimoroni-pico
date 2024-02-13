#pragma once
#include "pico/stdlib.h"

#include "ws2812.hpp"
#include "motor.hpp"
#include "motor_cluster.hpp"
#include "encoder.hpp"

namespace motor {
  namespace motor2040 {
    const uint MOTOR_A_P = 4;
    const uint MOTOR_A_N = 5;
    const uint MOTOR_B_P = 6;
    const uint MOTOR_B_N = 7;
    const uint MOTOR_C_P = 8;
    const uint MOTOR_C_N = 9;
    const uint MOTOR_D_P = 10;
    const uint MOTOR_D_N = 11;

    const pin_pair MOTOR_A(MOTOR_A_P, MOTOR_A_N);
    const pin_pair MOTOR_B(MOTOR_B_P, MOTOR_B_N);
    const pin_pair MOTOR_C(MOTOR_C_P, MOTOR_C_N);
    const pin_pair MOTOR_D(MOTOR_D_P, MOTOR_D_N);
    const uint NUM_MOTORS = 4;

    const uint ENCODER_A_A = 0;
    const uint ENCODER_A_B = 1;
    const uint ENCODER_B_A = 2;
    const uint ENCODER_B_B = 3;
    const uint ENCODER_C_A = 12;
    const uint ENCODER_C_B = 13;
    const uint ENCODER_D_A = 14;
    const uint ENCODER_D_B = 15;

    // Although encoder A and B channels are arbitrary, our MMME Encoders
    // that accompany Motor2040 count down when the motors are diving in a
    // positive direction, so these pin pairs are set as B and A instead
    const pin_pair ENCODER_A(ENCODER_A_B, ENCODER_A_A);
    const pin_pair ENCODER_B(ENCODER_B_B, ENCODER_B_A);
    const pin_pair ENCODER_C(ENCODER_C_B, ENCODER_C_A);
    const pin_pair ENCODER_D(ENCODER_D_B, ENCODER_D_A);
    const uint NUM_ENCODERS = 4;

    const uint TX_TRIG = 16;
    const uint RX_ECHO = 17;

    const uint LED_DATA = 18;
    const uint NUM_LEDS = 1;

    const uint I2C_INT = 19;
    const uint I2C_SDA = 20;
    const uint I2C_SCL = 21;

    const uint USER_SW = 23;

    const uint ADC_ADDR_0 = 22;
    const uint ADC_ADDR_1 = 24;
    const uint ADC_ADDR_2 = 25;

    const uint ADC0 = 26;
    const uint ADC1 = 27;
    const uint ADC2 = 28;
    const uint SHARED_ADC = 29; // The pin used for the board's sensing features

    const uint CURRENT_SENSE_A_ADDR = 0b000;
    const uint CURRENT_SENSE_B_ADDR = 0b001;
    const uint CURRENT_SENSE_C_ADDR = 0b010;
    const uint CURRENT_SENSE_D_ADDR = 0b011;
    const uint VOLTAGE_SENSE_ADDR = 0b100;
    const uint FAULT_SENSE_ADDR = 0b101;

    const uint SENSOR_1_ADDR = 0b110;
    const uint SENSOR_2_ADDR = 0b111;
    const uint NUM_SENSORS = 2;

    constexpr float SHUNT_RESISTOR = 0.47f;
    constexpr float CURRENT_GAIN = 1;
    constexpr float VOLTAGE_GAIN = 3.9f / 13.9f;
    constexpr float CURRENT_OFFSET = -0.005f;
  }
}