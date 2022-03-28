#include <stdio.h>
#include "pico_motor_shim.hpp"

#include "common/pimoroni_common.hpp"
#include "motor.hpp"
#include "button.hpp"

/*
Play a song using a motor! Works by setting the PWM duty cycle to 50% and changing the frequency on the fly.
Plug a motor into connector 1, and press "A" to start the song playing (does not loop). Press the button again will stop the song early.
*/

using namespace pimoroni;

// List frequencies (in hz) to play in sequence here. Use zero for when silence or a pause is wanted
// Song from PicoExplorer noise.py
constexpr float SONG[] = {1397, 1397, 1319, 1397, 698,  0,    698,  0,    1047, 932,
                          880,  1047, 1397, 0,    1397, 0,    1568, 1480, 1568, 784,
                          0,    784,  0,    1568, 1397, 1319, 1175, 1047, 0,    1047,
                          0,    1175, 1319, 1397, 1319, 1175, 1047, 1175, 1047, 932,
                          880,  932,  880,  784,  698,  784,  698,  659,  587,  523,
                          587,  659,  698,  784,  932,  880,  784,  880,  698,  0,    698};
constexpr uint SONG_LENGTH = sizeof(SONG) / sizeof(float);

//The time (in milliseconds) to play each note for. Change this to make the song play faster or slower
static const uint NOTE_DURATION_MS = 150;

//Uncomment this lineto have the song be played without the motor turning
//Note, this will affect the audio quality of the sound produced
//#define STATIONARY_PLAYBACK

//The time (in microseconds) between each direction switch of the motor when using STATIONARY_PLAYBACK
static const uint STATIONARY_TOGGLE_US = 2000;

//Uncomment this line to use the fast decay (coasting) motor mode.
//This seems to produce a louder sound with STATIONARY_PLAYBACK enabled, but will make movement poorer when STATIONARY_PLAYBACK is disabled
//#define USE_FAST_DECAY


Button button_a(pico_motor_shim::BUTTON_A, Polarity::ACTIVE_LOW, 0);
#ifdef USE_FAST_DECAY
  Motor motor_1(pico_motor_shim::MOTOR_1_POS, pico_motor_shim::MOTOR_1_NEG, Motor::DEFAULT_PWM_FREQUENCY, Motor::FAST_DECAY);
  Motor motor_2(pico_motor_shim::MOTOR_2_POS, pico_motor_shim::MOTOR_2_NEG, Motor::DEFAULT_PWM_FREQUENCY, Motor::FAST_DECAY);
#else
  Motor motor_1(pico_motor_shim::MOTOR_1_POS, pico_motor_shim::MOTOR_1_NEG, Motor::DEFAULT_PWM_FREQUENCY, Motor::SLOW_DECAY);
  Motor motor_2(pico_motor_shim::MOTOR_2_POS, pico_motor_shim::MOTOR_2_NEG, Motor::DEFAULT_PWM_FREQUENCY, Motor::SLOW_DECAY);
#endif

static bool button_toggle = false;

/**
 * Checks if the button has been pressed, toggling a value that is also returned.
 */
bool check_button_toggle() {
  bool button_pressed = button_a.read();
  if(button_pressed) {
    button_toggle = !button_toggle;
  }
  return button_toggle;
}

/**
 * The entry point of the program.
 */
int main() {
  stdio_init_all();

  //Initialise the LED. We use this to indicate that the sequence is running.
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  //Initialise the motor
  if(!motor_1.init() || !motor_2.init()) {
    printf("Cannot initialise the motors. Check the provided parameters\n");
    return 0;
  }

  while(true) {
    if(check_button_toggle()) {
      //Turn the Pico's LED on to show that the song has started
      gpio_put(PICO_DEFAULT_LED_PIN, true);

      //Play the song
      for(uint i = 0; i < SONG_LENGTH && check_button_toggle(); i++) {
        if(motor_1.set_frequency(SONG[i]) && motor_2.set_frequency(SONG[i])) {
          #ifdef STATIONARY_PLAYBACK
            //Set the motors to 50% duty cycle to play the note, but alternate
            //the direction so that the motor does not actually spin
            uint t = 0;
            while(t < NOTE_DURATION_MS * 1000) {
              motor_1.set_speed(0.5f);
              motor_2.set_speed(0.5f);
              sleep_us(STATIONARY_TOGGLE_US);
              t += STATIONARY_TOGGLE_US;

              motor_1.set_speed(-0.5f);
              motor_2.set_speed(-0.5f);
              sleep_us(STATIONARY_TOGGLE_US);
              t += STATIONARY_TOGGLE_US;
            }
          #else
            //Set the motors to 50% duty cycle to play the note
            motor_1.set_speed(0.5f);
            motor_2.set_speed(0.5f);
            sleep_ms(NOTE_DURATION_MS);
          #endif
        }
        else {
          //The frequency was invalid, so we are treating that to mean this is a pause note
          motor_1.stop();
          motor_2.stop();
          sleep_ms(NOTE_DURATION_MS);
        }
      }
      button_toggle = false;

      //The song has finished, so turn off the Pico's LED and disable the motors
      gpio_put(PICO_DEFAULT_LED_PIN, false);
      motor_1.disable();
      motor_2.disable();
    }

    sleep_ms(10);
  }
  return 0;
}
