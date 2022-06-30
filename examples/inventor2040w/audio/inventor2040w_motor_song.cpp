#include <stdio.h>
#include "pico/stdlib.h"

#include "inventor.hpp"

/*
A fun example of how to change a motor's frequency to have it play a song.

It uses code written by Avram Piltch - check out his Tom's Hardware article!
https://www.tomshardware.com/uk/how-to/buzzer-music-raspberry-pi-pico

Press "User" button to start or stop the song.
*/

using namespace inventor;

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


// Create a new Inventor2040W
Inventor2040W board;


// Variables for recording the button state and if it has been toggled
// Starting as True makes the song play automatically
static bool button_toggle = true;
static bool last_button_state = false;

// Checks if the button has been pressed, toggling a value that is also returned.
bool check_button_toggle() {
  bool button_state = board.switch_pressed();
  if(button_state && !last_button_state) {
    button_toggle = !button_toggle;
  }
  last_button_state = button_state;

  return button_toggle;
}


int main() {
  stdio_init_all();

  // Attempt to initialise the board
  if(board.init()) {

    // Access the motor and encoder from Inventor
    Motor& motor_a = board.motors[MOTOR_A];
    Motor& motor_b = board.motors[MOTOR_B];

    // Set the two motor's decay modes
    motor_a.decay_mode(DECAY_MODE);
    motor_b.decay_mode(DECAY_MODE);

    while(true) {
        // Has the button been toggled?
        if(check_button_toggle()) {

        // Play the song
        for(uint i = 0; i < SONG_LENGTH && check_button_toggle(); i++) {
            // Get the frequency of the note and attempt to set the motors to it
            float freq = SONG[i];
            if(motor_a.frequency(freq) && motor_b.frequency(freq)) {
            if(STATIONARY_PLAYBACK) {
                // Set the motors to 50% duty cycle to play the note, but alternate
                // the direction so that the motor does not actually spin
                uint t = 0;
                while(t < NOTE_DURATION_MS * 1000) {
                motor_a.duty(0.5f);
                motor_b.duty(0.5f);
                sleep_us(STATIONARY_TOGGLE_US);
                t += STATIONARY_TOGGLE_US;

                motor_a.duty(-0.5f);
                motor_b.duty(-0.5f);
                sleep_us(STATIONARY_TOGGLE_US);
                t += STATIONARY_TOGGLE_US;
                }
            }
            else {
                // Set the motors to 50% duty cycle to play the note
                motor_a.duty(0.5f);
                motor_b.duty(0.5f);
                sleep_ms(NOTE_DURATION_MS);
            }
            }
            else {
            // The frequency was invalid, so we are treating that to mean this is a pause note
            motor_a.stop();
            motor_b.stop();
            sleep_ms(NOTE_DURATION_MS);
            }
        }
        button_toggle = false;

        // The song has finished, so disable the motors
        motor_a.disable();
        motor_b.disable();
        }

        sleep_ms(10);
    }
  }
  return 0;
}