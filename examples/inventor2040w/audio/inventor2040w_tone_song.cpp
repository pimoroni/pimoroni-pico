#include <stdio.h>
#include "pico/stdlib.h"

#include "inventor.hpp"

/*
This example shows you how you can use Inventor 2040's audio output
with a speaker to play different notes and string them together into a bleepy tune.

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


// Create a new Inventor object
Inventor board;


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

    // Set the volume of the speaker output
    board.volume(0.25f);

    while(true) {
      // Has the button been toggled?
      if(check_button_toggle()) {

        // Play the song
        for(uint i = 0; i < SONG_LENGTH && check_button_toggle(); i++) {
          // Get the frequency of the note
          float freq = SONG[i];
          if(freq == 0) {
            // This is a "pause" note, so stop the motors
            board.play_silence();
          }
          else {
            // Get the frequency of the note and play it
            board.play_tone(freq);
          }
          sleep_ms(NOTE_DURATION_MS);
        }
        button_toggle = false;

        board.stop_playing();
      }

      sleep_ms(10);
    }
  }
  return 0;
}
