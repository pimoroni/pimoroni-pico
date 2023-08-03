#include "pico/stdlib.h"
#include <tusb.h>

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

  y.change_logging(3);

  try {
    // Initialise the servo
    y.init();

    while (!tud_cdc_connected()) {
      sleep_ms(100);
    }
    sleep_ms(1000);
    printf("tud_cdc_connected()\n");

    LEDStripModule strip;
    //y.find_slots_with_module(LEDStripModule::info());
    //y.find_slots_with_module(DualSwitchedModule::info());
    //y.find_slots_with_module(BenchPowerModule::info());
    y.register_with_slot(&strip, 3);
    y.initialise_modules();

    //y.detect_module(Yukon::SLOT1);
    //y.detect_module(Yukon::SLOT2);
    //y.detect_module(Yukon::SLOT3);
    //y.detect_module(Yukon::SLOT4);
    //y.detect_module(Yukon::SLOT5);
    //y.detect_module(Yukon::SLOT6);

    y.enable_main_output();
    strip.enable();

    while(!y.is_boot_pressed()) {
      y.monitored_sleep_ms(100);
      //printf("[Yukon] V = %f, C = %f, T = %f, ", y.read_voltage(), y.read_current(), y.read_temperature());
      printf("[Slot1] A1 = %f, A2 = %f, ", y.read_slot_adc1(Yukon::SLOT1), y.read_slot_adc2(Yukon::SLOT1));
      printf("[Slot2] A1 = %f, A2 = %f, ", y.read_slot_adc1(Yukon::SLOT2), y.read_slot_adc2(Yukon::SLOT2));
      printf("[Slot3] A1 = %f, A2 = %f, ", y.read_slot_adc1(Yukon::SLOT3), y.read_slot_adc2(Yukon::SLOT3));
      printf("[Slot4] A1 = %f, A2 = %f, ", y.read_slot_adc1(Yukon::SLOT4), y.read_slot_adc2(Yukon::SLOT4));
      printf("[Slot5] A1 = %f, A2 = %f, ", y.read_slot_adc1(Yukon::SLOT5), y.read_slot_adc2(Yukon::SLOT5));
      printf("[Slot6] A1 = %f, A2 = %f, ", y.read_slot_adc1(Yukon::SLOT6), y.read_slot_adc2(Yukon::SLOT6));
      printf("\n");
    }
  }
  catch(const std::exception &e) {
    printf(e.what());
  }

  y.reset();
  return 0;
}
