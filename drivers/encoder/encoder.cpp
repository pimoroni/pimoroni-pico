#include <math.h>
#include <cfloat>
#include <climits>
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "encoder.hpp"

#ifndef NO_QSTR
#include "encoder.pio.h"
#endif

#define LAST_STATE(state)  ((state) & 0b0011)
#define CURR_STATE(state)  (((state) & 0b1100) >> 2)

namespace encoder {

////////////////////////////////////////////////////////////////////////////////////////////////////
// STATICS
////////////////////////////////////////////////////////////////////////////////////////////////////
Encoder* Encoder::encoders[][NUM_PIO_STATE_MACHINES] = { { nullptr, nullptr, nullptr, nullptr }, { nullptr, nullptr, nullptr, nullptr } };
uint8_t Encoder::claimed_sms[] = { 0x0, 0x0 };
uint Encoder::pio_program_offset[] = { 0, 0 };


Encoder::Capture::Capture()
: captured_count(0), captured_delta(0), captured_frequency(0.0f), counts_per_rev(INT32_MAX) {
}

Encoder::Capture::Capture(int32_t count, int32_t delta, float frequency, float counts_per_rev)
: captured_count(count), captured_delta(delta), captured_frequency(frequency)
, counts_per_rev(MAX(counts_per_rev, FLT_EPSILON)) { //Clamp counts_per_rev to avoid potential NaN
}

int32_t Encoder::Capture::count() const {
  return captured_count;
}

int32_t Encoder::Capture::delta() const {
  return captured_delta;
}

float Encoder::Capture::frequency() const {
  return captured_frequency;
}

float Encoder::Capture::revolutions() const {
  return (float)captured_count / counts_per_rev;
}

float Encoder::Capture::degrees() const {
  return revolutions() * 360.0f;
}

float Encoder::Capture::radians() const {
  return revolutions() * M_TWOPI;
}

float Encoder::Capture::revolutions_delta() const {
  return (float)captured_delta / counts_per_rev;
}

float Encoder::Capture::degrees_delta() const {
  return revolutions_delta() * 360.0f;
}

float Encoder::Capture::radians_delta() const {
  return revolutions_delta() * M_TWOPI;
}

float Encoder::Capture::revolutions_per_second() const {
  return captured_frequency / counts_per_rev;
}

float Encoder::Capture::revolutions_per_minute() const {
  return revolutions_per_second() * 60.0f;
}

float Encoder::Capture::degrees_per_second() const {
  return revolutions_per_second() * 360.0f;
}

float Encoder::Capture::radians_per_second() const {
  return revolutions_per_second() * M_TWOPI;
}

Encoder::Encoder(PIO pio, uint sm, const pin_pair &pins, uint common_pin, Direction direction,
                  float counts_per_rev, bool count_microsteps, uint16_t freq_divider)
: pio(pio)
, sm(sm)
, enc_pins(pins)
, enc_common_pin(common_pin)
, enc_direction(direction)
, enc_counts_per_rev(MAX(counts_per_rev, FLT_EPSILON))
, count_microsteps(count_microsteps)
, freq_divider(freq_divider)
, clocks_per_time((float)(clock_get_hz(clk_sys) / (ENC_LOOP_CYCLES * freq_divider))) {
}

Encoder::~Encoder() {
  if(initialised) {
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_unclaim(pio, sm);

    uint pio_idx = pio_get_index(pio);
    encoders[pio_idx][sm] = nullptr;
    claimed_sms[pio_idx] &= ~(1u << sm);

    hw_clear_bits(&pio->inte1, PIO_IRQ1_INTE_SM0_RXNEMPTY_BITS << sm);

    //If there are no more SMs using the encoder program, then we can remove it from the PIO
    if(claimed_sms[pio_idx] == 0) {
      pio_remove_program(pio, &encoder_program, pio_program_offset[pio_idx]);

      if(pio_idx == 0) {
        irq_remove_handler(PIO0_IRQ_1, pio0_interrupt_handler);
      }
      else {
        irq_remove_handler(PIO1_IRQ_1, pio1_interrupt_handler);
      }
    }

    // Reset all the pins this PWM will control back to an unused state
    gpio_set_function(enc_pins.a, GPIO_FUNC_NULL);
    gpio_set_function(enc_pins.b, GPIO_FUNC_NULL);

    if(enc_common_pin != PIN_UNUSED) {
      gpio_set_function(enc_common_pin, GPIO_FUNC_NULL);
    }
  }
}

void Encoder::pio_interrupt_handler(uint pio_idx) {
  // Go through each SM on the PIO to see which triggered this interrupt,
  // and if there's an associated encoder, have it update its state
  for(uint8_t sm = 0; sm < NUM_PIO_STATE_MACHINES; sm++) {
    if(encoders[pio_idx][sm] != nullptr) {
      encoders[pio_idx][sm]->process_steps();
    }
  }
}

void Encoder::pio0_interrupt_handler() {
  pio_interrupt_handler(0);
}

void Encoder::pio1_interrupt_handler() {
  pio_interrupt_handler(1);
}

bool Encoder::init() {
  if(!initialised && !pio_sm_is_claimed(pio, sm)) {
    // Are the pins we want to use actually valid?
    if((enc_pins.a < NUM_BANK0_GPIOS) && (enc_pins.b < NUM_BANK0_GPIOS)) {

      // If a Pin C was defined, and valid, set it as a GND to pull the other two pins down
      if((enc_common_pin != PIN_UNUSED) && (enc_common_pin < NUM_BANK0_GPIOS)) {
        gpio_init(enc_common_pin);
        gpio_set_dir(enc_common_pin, GPIO_OUT);
        gpio_put(enc_common_pin, false);
      }

      pio_sm_claim(pio, sm);
      uint pio_idx = pio_get_index(pio);

      // If this is the first time using an encoder on this PIO, add the program to the PIO memory
      if(claimed_sms[pio_idx] == 0) {
        pio_program_offset[pio_idx] = pio_add_program(pio, &encoder_program);
      }

      // Initialise the A and B pins of this encoder
      pio_gpio_init(pio, enc_pins.a);
      pio_gpio_init(pio, enc_pins.b);
      gpio_pull_up(enc_pins.a);
      gpio_pull_up(enc_pins.b);

      // Set their default direction
      pio_sm_set_consecutive_pindirs(pio, sm, enc_pins.a, 1, false);
      pio_sm_set_consecutive_pindirs(pio, sm, enc_pins.b, 1, false);

      pio_sm_config c = encoder_program_get_default_config(pio_program_offset[pio_idx]);
      sm_config_set_jmp_pin(&c, enc_pins.a);
      sm_config_set_in_pins(&c, enc_pins.b);

      sm_config_set_in_shift(&c, false, false, 1);
      sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);

      sm_config_set_clkdiv_int_frac(&c, freq_divider, 0);

      pio_sm_init(pio, sm, pio_program_offset[pio_idx], &c);

      hw_set_bits(&pio->inte1, PIO_IRQ1_INTE_SM0_RXNEMPTY_BITS << sm);
      if(claimed_sms[pio_idx] == 0) {
        // Configure the processor to run pio_handler() when PIO IRQ 0 is asserted
        if(pio_idx == 0) {
          irq_add_shared_handler(PIO0_IRQ_1, pio0_interrupt_handler, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
          irq_set_enabled(PIO0_IRQ_1, true);
        }
        else {
          irq_add_shared_handler(PIO1_IRQ_1, pio1_interrupt_handler, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
          irq_set_enabled(PIO1_IRQ_1, true);
        }
      }

      //Keep a record of this encoder for the interrupt callback
      encoders[pio_idx][sm] = this;
      claimed_sms[pio_idx] |= 1u << sm;

      enc_state_a = gpio_get(enc_pins.a);
      enc_state_b = gpio_get(enc_pins.b);
      pio_sm_exec(pio, sm, pio_encode_set(pio_x, (uint)enc_state_a << 1 | (uint)enc_state_b));
      pio_sm_set_enabled(pio, sm, true);

      initialised = true;
    }
  }

  return initialised;
}

pin_pair Encoder::pins() const {
  return enc_pins;
}

uint Encoder::common_pin() const {
  return enc_common_pin;
}

bool_pair Encoder::state() const {
  return bool_pair(enc_state_a, enc_state_b);
}

int32_t Encoder::count() const {
  return enc_count;
}

int32_t Encoder::delta() {
  int32_t count = enc_count;  // Store a local copy of enc_count to avoid two reads

  // Determine the change in counts since the last time this function was performed
  int32_t change = count - last_count;
  last_count = count;

  return change;
}

void Encoder::zero() {
  enc_count = 0;
  enc_cumulative_time = 0;
  enc_step = 0;
  enc_turn = 0;

  microstep_time = 0;
  step_dir = NO_DIR; // may not be wanted?

  last_count = 0;
  last_capture_count = 0;
}

int16_t Encoder::step() const {
  return enc_step;
}

int16_t Encoder::turn() const {
  return enc_turn;
}

float Encoder::revolutions() const {
  return (float)count() / enc_counts_per_rev;
}

float Encoder::degrees() const {
  return revolutions() * 360.0f;
}

float Encoder::radians() const {
  return revolutions() * M_TWOPI;
}

Direction Encoder::direction() const {
  return enc_direction;
}

void Encoder::direction(Direction direction) {
  enc_direction = direction;
}

float Encoder::counts_per_rev() const {
  return enc_counts_per_rev;
}

void Encoder::counts_per_rev(float counts_per_rev) {
  enc_counts_per_rev = MAX(counts_per_rev, FLT_EPSILON);
}

Encoder::Capture Encoder::capture() {
  // Take a capture of the current values
  int32_t count = enc_count;
  int32_t cumulative_time = enc_cumulative_time;
  enc_cumulative_time = 0;

  // Determine the change in counts since the last capture was taken
  int32_t change = count - last_capture_count;
  last_capture_count = count;

  // Calculate the average frequency of steps
  float frequency = 0.0f;
  if(change != 0 && cumulative_time != INT32_MAX) {
    frequency = (clocks_per_time * (float)change) / (float)cumulative_time;
  }

  return Capture(count, change, frequency, enc_counts_per_rev);
}

void Encoder::process_steps() {
  while(pio->ints1 & (PIO_IRQ1_INTS_SM0_RXNEMPTY_BITS << sm)) {
    uint32_t received = pio_sm_get(pio, sm);

    // Extract the current and last encoder states from the received value
    enc_state_a = (bool)(received & STATE_A_MASK);
    enc_state_b = (bool)(received & STATE_B_MASK);
    uint8_t states = (received & STATES_MASK) >> 28;

    // Extract the time (in cycles) it has been since the last received
    int32_t time_received = (received & TIME_MASK) + ENC_DEBOUNCE_TIME;

    // For rotary encoders, only every fourth step is cared about, causing an inaccurate time value
    // To address this we accumulate the times received and zero it when a step is counted
    if(!count_microsteps) {
      if(time_received + microstep_time < time_received)  // Check to avoid integer overflow
        time_received = INT32_MAX;
      else
        time_received += microstep_time;
      microstep_time = time_received;
    }

    bool up = (enc_direction == NORMAL_DIR);

    // Determine what step occurred
    switch(LAST_STATE(states)) {
      //--------------------------------------------------
      case MICROSTEP_0:
        switch(CURR_STATE(states)) {
          // A ____|‾‾‾‾
          // B _________
          case MICROSTEP_1:
            if(count_microsteps)
              microstep(time_received, up);
            break;

          // A _________
          // B ____|‾‾‾‾
          case MICROSTEP_3:
            if(count_microsteps)
              microstep(time_received, !up);
            break;
        }
        break;

      //--------------------------------------------------
      case MICROSTEP_1:
        switch(CURR_STATE(states)) {
          // A ‾‾‾‾‾‾‾‾‾
          // B ____|‾‾‾‾
          case MICROSTEP_2:
            if(count_microsteps || step_dir == INCREASING)
              microstep(time_received, up);

            step_dir = NO_DIR;  // Finished increasing
            break;

          // A ‾‾‾‾|____
          // B _________
          case MICROSTEP_0:
            if(count_microsteps)
              microstep(time_received, !up);
            break;
        }
        break;

      //--------------------------------------------------
      case MICROSTEP_2:
        switch(CURR_STATE(states)) {
          // A ‾‾‾‾|____
          // B ‾‾‾‾‾‾‾‾‾
          case MICROSTEP_3:
            if(count_microsteps)
              microstep(time_received, up);

            step_dir = INCREASING;  // Started increasing
            break;

          // A ‾‾‾‾‾‾‾‾‾
          // B ‾‾‾‾|____
          case MICROSTEP_1:
            if(count_microsteps)
              microstep(time_received, !up);

            step_dir = DECREASING;  // Started decreasing
            break;
        }
        break;

      //--------------------------------------------------
      case MICROSTEP_3:
        switch(CURR_STATE(states)) {
          // A _________
          // B ‾‾‾‾|____
          case MICROSTEP_0:
            if(count_microsteps)
              microstep(time_received, up);
            break;

          // A ____|‾‾‾‾
          // B ‾‾‾‾‾‾‾‾‾
          case MICROSTEP_2:
            if(count_microsteps || step_dir == DECREASING)
              microstep(time_received, !up);

            step_dir = NO_DIR;  // Finished decreasing
            break;
        }
        break;
    }
  }
}

void Encoder::microstep(int32_t time, bool up) {
  if(up) {
    enc_count++;
    if(++enc_step >= (int16_t)enc_counts_per_rev) {
      enc_step -= (int16_t)enc_counts_per_rev;
      enc_turn++;
    }
  }
  else {
    enc_count--;
    if(--enc_step < 0) {
      enc_step += (int16_t)enc_counts_per_rev;
      enc_turn--;
    }
  }
  microstep_time = 0;

  if(time + enc_cumulative_time < time)  // Check to avoid integer overflow
    enc_cumulative_time = INT32_MAX;
  else
    enc_cumulative_time += time;
}
}