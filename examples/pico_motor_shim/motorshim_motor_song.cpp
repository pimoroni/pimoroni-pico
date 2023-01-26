#include <stdio.h>
#include "pico/stdlib.h"

#include "pico_motor_shim.hpp"
#include "button.hpp"

/*
A fun example of how to change a motor's frequency to have it play a song.

Press "A" to start or stop the song.
*/

using namespace pimoroni;
using namespace motor;

// List frequencies (in hz) to play in sequence here. Use zero for when silence or a pause is wanted
// Song from PicoExplorer noise.py
constexpr float SONG[] = {1397, 1397, 1319, 1397, 698,  0,    698,  0,    1047, 932,
                          880,  1047, 1397, 0,    1397, 0,    1568, 1480, 1568, 784,
                          0,    784,  0,    1568, 1397, 1319, 1175, 1047, 0,    1047,
                          0,    1175, 1319, 1397, 1319, 1175, 1047, 1175, 1047, 932,
                          880,  932,  880,  784,  698,  784,  698,  659,  587,  523,
                          587,  659,  698,  784,  932,  880,  784,  880,  698,  0,    698};
constexpr uint SONG_LENGTH = count_of(SONG);

// The time (in seconds) to play each note for. Change this to make the song play faster or slower
const uint NOTE_DURATION_MS = 150;

// The time (in microseconds) between each direction switch of the motor when using STATIONARY_PLAYBACK
const uint STATIONARY_TOGGLE_US = 2000;

// Whether to play the song with or without the motors spinning
const bool STATIONARY_PLAYBACK = false;

// The motor decay mode to use, either FAST_DECAY (0) or SLOW_DECAY (1). Affects the song's quality
const DecayMode DECAY_MODE = SLOW_DECAY;


// Create two motor objects with a given decay mode
Motor motor_1(pico_motor_shim::MOTOR_1);
Motor motor_2(pico_motor_shim::MOTOR_2);

// Create the user button
Button button_a(pico_motor_shim::BUTTON_A, Polarity::ACTIVE_LOW, 0);

// Variable for recording if the button has been toggled
// Starting as true makes the song play automatically
static bool button_toggle = true;

// Checks if the button has been pressed, toggling a value that is also returned.
bool check_button_toggle() {
  if(button_a.read()) {
    button_toggle = !button_toggle;
  }
  return button_toggle;
}


int main() {
  stdio_init_all();

#ifdef PICO_DEFAULT_LED_PIN
  //Initialise the LED. We use this to indicate that the sequence is running.
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif

  // Se the two motor's decay modes
  motor_1.decay_mode(DECAY_MODE);
  motor_2.decay_mode(DECAY_MODE);

  // Initialise the motors
  if(!motor_1.init() || !motor_2.init()) {
    printf("Cannot initialise the motors. Check the provided parameters\n");
    return 0;
  }

  while(true) {
    // Has the button been toggled?
    if(check_button_toggle()) {

#ifdef PICO_DEFAULT_LED_PIN
      // Turn the Pico's LED on to show that the song has started
      gpio_put(PICO_DEFAULT_LED_PIN, true);
#endif

      // Play the song
      for(uint i = 0; i < SONG_LENGTH && check_button_toggle(); i++) {
        // Get the frequency of the note and attempt to set the motors to it
        float freq = SONG[i];
        if(motor_1.frequency(freq) && motor_2.frequency(freq)) {
          if(STATIONARY_PLAYBACK) {
            // Set the motors to 50% duty cycle to play the note, but alternate
            // the direction so that the motor does not actually spin
            uint t = 0;
            while(t < NOTE_DURATION_MS * 1000) {
              motor_1.duty(0.5f);
              motor_2.duty(0.5f);
              sleep_us(STATIONARY_TOGGLE_US);
              t += STATIONARY_TOGGLE_US;

              motor_1.duty(-0.5f);
              motor_2.duty(-0.5f);
              sleep_us(STATIONARY_TOGGLE_US);
              t += STATIONARY_TOGGLE_US;
            }
          }
          else {
            // Set the motors to 50% duty cycle to play the note
            motor_1.duty(0.5f);
            motor_2.duty(0.5f);
            sleep_ms(NOTE_DURATION_MS);
          }
        }
        else {
          // The frequency was invalid, so we are treating that to mean this is a pause note
          motor_1.stop();
          motor_2.stop();
          sleep_ms(NOTE_DURATION_MS);
        }
      }
      button_toggle = false;

      // The song has finished, so turn off the Pico's LED and disable the motors
#ifdef PICO_DEFAULT_LED_PIN
      gpio_put(PICO_DEFAULT_LED_PIN, false);
#endif
      motor_1.disable();
      motor_2.disable();
    }

    sleep_ms(10);
  }
  return 0;
}
