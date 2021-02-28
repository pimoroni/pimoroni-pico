// Very simple example that turns the pico_rgb_keypad into a MIDI device 
// where each pad sends midi note messages
//
// Based on:
//   https://github.com/pimoroni/pimoroni-pico/blob/main/examples/pico_rgb_keypad
//   https://github.com/hathach/tinyusb/tree/master/examples/device/midi_test


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "bsp/board.h"
#include "tusb.h"

#include "pico_rgb_keypad.hpp"

using namespace pimoroni;

PicoRGBKeypad pico_keypad;

// ButtonNote class contains the button states
// for note ons and note offs
class ButtonNote
{
public:
  typedef enum  
  {
    nsOn,
    nsOff
  } NoteState;

  ButtonNote(uint8_t btn_num, NoteState note_state) : m_btn_num(btn_num), m_btn_state(note_state) {};

  const uint8_t   GetButtonNum(void)   const { return m_btn_num; };
  const uint8_t   GetButtonNote(void)  const { return 60 + m_btn_num; };
  const NoteState GetButtonState(void) const { return m_btn_state; };
  
private:
  uint8_t     m_btn_num;
  NoteState   m_btn_state;
};

// button_notes vector contains the note data to send over MIDI
std::vector<ButtonNote> button_notes;

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);
void midi_task(void);
void keypad_task(void);

/*------------- MAIN -------------*/
int main(void)
{
  board_init();

  tusb_init();

  pico_keypad.init();
 
  pico_keypad.set_brightness(1.0f);

  while (1)
  {
    tud_task(); // tinyusb device task
    led_blinking_task();
    keypad_task();
    midi_task();
  }

  return 0;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

//--------------------------------------------------------------------+
// MIDI Task
//
// Send all note events in button_notes over midi 
// and then clear button_notes.
//--------------------------------------------------------------------+

void midi_task(void)
{
  static uint32_t start_ms = 0;

  uint8_t const cable_num = 0; // MIDI jack associated with USB endpoint
  uint8_t const channel   = 0; // 0 for channel 1

  // The MIDI interface always creates input and output port/jack descriptors
  // regardless of these being used or not. Therefore incoming traffic should be read
  // (possibly just discarded) to avoid the sender blocking in IO
  uint8_t packet[4];
  while(tud_midi_available()) 
    tud_midi_receive(packet);

  // For each of our note events send the relevent note on or note off over MIDI.
  // Note: tudi_midi_write24 is renamed tud_midi_write24 in more recent versions of tinyusb, the pico repo is lagging behind.
  for(ButtonNote note : button_notes)
  {
    if(note.GetButtonState() == ButtonNote::nsOn)
    {
      // Send Note On at full velocity (127) on channel 1.
      tudi_midi_write24(cable_num, 0x90 | channel, note.GetButtonNum(), 127);
    }
    else
    {
      // Send Note Off
      tudi_midi_write24(cable_num, 0x80 | channel, note.GetButtonNum(), 0);
    }
  }

  button_notes.clear();
}

//--------------------------------------------------------------------+
// BLINKING TASK
//
// Blinks the LED dependent on USB state.
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // Blink every interval ms
  if ( board_millis() - start_ms < blink_interval_ms) 
    return; // not enough time

  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}

//--------------------------------------------------------------------+
// Keypad TASK
//
// Check button states, if changed add relevent ButtonNote to button_notes
// and set pad illumination.
//--------------------------------------------------------------------+
void keypad_task(void)
{
  static uint16_t last_button_states = 0;

  uint16_t button_states = pico_keypad.get_button_states();
  
  if(last_button_states != button_states) 
  {
    uint16_t new_button_states = button_states;

    for(uint8_t pad = 0; pad < pico_keypad.NUM_PADS; pad++) 
    {
      bool old_button_state = last_button_states & 0x01;
      bool new_button_state = new_button_states & 0x01;

      if(old_button_state != new_button_state)
      {
        if(new_button_state)
        {
          // note on
          pico_keypad.illuminate(pad, 0xff, 0xff, 0xff);
          button_notes.push_back(ButtonNote(pad, ButtonNote::nsOn));
        }
        else
        {
          // note off
          pico_keypad.illuminate(pad, 0x00, 0x00, 0x00);
          button_notes.push_back(ButtonNote(pad, ButtonNote::nsOff));
        }
      }

      // shift states
      new_button_states >>= 1;
      last_button_states >>= 1;
    }

    last_button_states = button_states;
  }


  pico_keypad.update();
}
