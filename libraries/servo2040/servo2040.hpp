#pragma once
#include "pico/stdlib.h"

#include "ws2812.hpp"
#include "servo.hpp"
#include "servo_cluster.hpp"

namespace servo {
  namespace servo2040 {
    const uint SERVO_1 = 0;
    const uint SERVO_2 = 1;
    const uint SERVO_3 = 2;
    const uint SERVO_4 = 3;
    const uint SERVO_5 = 4;
    const uint SERVO_6 = 5;
    const uint SERVO_7 = 6;
    const uint SERVO_8 = 7;
    const uint SERVO_9 = 8;
    const uint SERVO_10 = 9;
    const uint SERVO_11 = 10;
    const uint SERVO_12 = 11;
    const uint SERVO_13 = 12;
    const uint SERVO_14 = 13;
    const uint SERVO_15 = 14;
    const uint SERVO_16 = 15;
    const uint SERVO_17 = 16;
    const uint SERVO_18 = 17;
    const uint NUM_SERVOS = 18;

    const uint LED_DATA = 18;
    const uint NUM_LEDS = 6;

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

    const uint SENSOR_1_ADDR = 0b000;
    const uint SENSOR_2_ADDR = 0b001;
    const uint SENSOR_3_ADDR = 0b010;
    const uint SENSOR_4_ADDR = 0b011;
    const uint SENSOR_5_ADDR = 0b100;
    const uint SENSOR_6_ADDR = 0b101;
    const uint NUM_SENSORS = 6;

    const uint VOLTAGE_SENSE_ADDR = 0b110;
    const uint CURRENT_SENSE_ADDR = 0b111;

    constexpr float SHUNT_RESISTOR = 0.003f;
    constexpr float CURRENT_GAIN = 69;
    constexpr float VOLTAGE_GAIN = 3.9f / 13.9f;
    constexpr float CURRENT_OFFSET = -0.02f;
  }
}