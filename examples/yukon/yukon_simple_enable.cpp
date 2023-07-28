#include "pico/stdlib.h"

#include "yukon.hpp"
using namespace pimoroni;
/*
Demonstrates how to create a Servo object and control it.
*/

// How many sweeps of the servo to perform
const uint SWEEPS = 3;

// The number of discrete sweep steps
const uint STEPS = 10;

// The time in milliseconds between each step of the sequence
const uint STEPS_INTERVAL_MS = 500;

// How far from zero to move the servo when sweeping
constexpr float SWEEP_EXTENT = 90.0f;


Yukon y = Yukon();


int main() {
  stdio_init_all();

  // Initialise the servo
  y.init();

  y.set_slow_output(Yukon::MAIN_EN, true);

  while(true) {
    printf("[Yukon] V = %f, C = %f, T = %f, ", y.read_voltage(), y.read_current(), y.read_temperature());
    printf("[Slot1] A1 = %f, A2 = %f, ", y.read_slot_adc1(Yukon::SLOT1), y.read_slot_adc2(Yukon::SLOT1));
    printf("[Slot2] A1 = %f, A2 = %f, ", y.read_slot_adc1(Yukon::SLOT2), y.read_slot_adc2(Yukon::SLOT2));
    printf("[Slot3] A1 = %f, A2 = %f, ", y.read_slot_adc1(Yukon::SLOT3), y.read_slot_adc2(Yukon::SLOT3));
    printf("[Slot4] A1 = %f, A2 = %f, ", y.read_slot_adc1(Yukon::SLOT4), y.read_slot_adc2(Yukon::SLOT4));
    printf("[Slot5] A1 = %f, A2 = %f, ", y.read_slot_adc1(Yukon::SLOT5), y.read_slot_adc2(Yukon::SLOT5));
    printf("[Slot6] A1 = %f, A2 = %f, ", y.read_slot_adc1(Yukon::SLOT6), y.read_slot_adc2(Yukon::SLOT6));
    printf("\n");
    sleep_ms(100);
  }
}
