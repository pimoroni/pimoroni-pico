#include <cstdio>
#include "pico/stdlib.h"

#include "inventor.hpp"
#include "analog.hpp"

/*
Shows how to initialise and read the 3 ADC headers of Inventor 2040/2350 W.

Press "User" to exit the program.
*/

using namespace inventor;

// The brightness of the LEDs
constexpr float BRIGHTNESS = 0.4f;

// How many times to update LEDs per second
const uint UPDATES = 10;

const char* ADC_NAMES[] = {"A0", "A1", "A2"};


// Create a new Inventor object
Inventor board;

// Create an analog object for each ADC
Analog analogs[] = {Analog(A0), Analog(A1), Analog(A2)};


int main() {
  stdio_init_all();

  // Attempt to initialise the board
  if(board.init()) {

    // Pull each analog pin low. This avoids random numbers
    // when nothing is connected to any of the pins.
    gpio_set_pulls(A0, false, true);
    gpio_set_pulls(A1, false, true);
    gpio_set_pulls(A2, false, true);

    // Read the GPIOs until the user button is pressed
    while(!board.switch_pressed()) {

      // Read each ADC in turn and print its voltage
      for(uint i = 0; i < NUM_ADCS; i++) {
        float voltage = analogs[i].read_voltage();
        printf("%s = %f, ", ADC_NAMES[i], voltage);

        // Set the neighbouring LED to a colour based on the
        // voltage, with Green for high and Blue for low
        float hue = (2.0f - (voltage / 3.3f)) * 0.333f;
        board.leds.set_hsv(i + LED_A0, hue, 1.0, BRIGHTNESS);
      }

      // Print a new line
      printf("\n");

      sleep_ms(1000 / UPDATES);
    }

    // Turn off the LED bars
    board.leds.clear();

    // Sleep a short time so the clear takes effect
    sleep_ms(100);
  }
}