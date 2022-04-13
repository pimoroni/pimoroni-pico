#include <math.h>
#include <climits>
#include "hardware/irq.h"
#include "encoder.hpp"
#include "encoder.pio.h"

#define LAST_STATE(state)  ((state) & 0b0011)
#define CURR_STATE(state)  (((state) & 0b1100) >> 2)

namespace pimoroni {

////////////////////////////////////////////////////////////////////////////////////////////////////
// STATICS
////////////////////////////////////////////////////////////////////////////////////////////////////
Encoder* Encoder::encoders[][NUM_PIO_STATE_MACHINES] = { { nullptr, nullptr, nullptr, nullptr }, { nullptr, nullptr, nullptr, nullptr } };
uint8_t Encoder::claimed_sms[] = { 0x0, 0x0 };
uint Encoder::pio_program_offset[] = { 0, 0 };


Encoder::Encoder(PIO pio, uint sm, const pin_pair &pins, uint pin_c,
                  float counts_per_revolution, bool count_microsteps,
                  uint16_t freq_divider)
: pio(pio)
, sm(sm)
, enc_pins(pins)
, pin_c(pin_c)
, counts_per_revolution(counts_per_revolution)
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

    if(pin_c != PIN_UNUSED) {
      gpio_set_function(pin_c, GPIO_FUNC_NULL);
    }
  }
}

void Encoder::pio_interrupt_handler(uint pio_idx) {
  // Go through each SM on the PIO to see which triggered this interrupt,
  // and if there's an associated encoder, have it update its state
  for(uint8_t sm = 0; sm < NUM_PIO_STATE_MACHINES; sm++) {
    if(encoders[pio_idx][sm] != nullptr) {
      encoders[pio_idx][sm]->check_for_transition(); 
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
      if((pin_c != PIN_UNUSED) && (pin_c < NUM_BANK0_GPIOS)) {
        gpio_init(pin_c);
        gpio_set_dir(pin_c, GPIO_OUT);
        gpio_put(pin_c, false);
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
      //float div = clock_get_hz(clk_sys) / 500000;
      //sm_config_set_clkdiv(&c, div);

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

bool_pair Encoder::state() const {
  return bool_pair(enc_state_a, enc_state_b);
}

int32_t Encoder::count() const {
  return enc_count - count_offset;
}

float Encoder::revolutions() const {
  return (float)count() / counts_per_revolution;
}

float Encoder::angle_degrees() const {
  return revolutions() * 360.0f;
}

float Encoder::angle_radians() const {
  return revolutions() * M_TWOPI;
}

float Encoder::frequency() const {
  return clocks_per_time / (float)time_since;
}

float Encoder::revolutions_per_second() const {
  return frequency() / counts_per_revolution;
}

float Encoder::revolutions_per_minute() const {
  return revolutions_per_second() * 60.0f;
}

float Encoder::degrees_per_second() const {
  return revolutions_per_second() * 360.0f;
}

float Encoder::radians_per_second() const {
  return revolutions_per_second() * M_TWOPI;
}

void Encoder::zero_count() {
  count_offset = enc_count;
}

Capture Encoder::perform_capture() {
  // Capture the current values
  int32_t captured_count = enc_count;
  int32_t captured_cumulative_time = cumulative_time;
  cumulative_time = 0;

  // Determine the change in counts since the last capture was performed
  int32_t count_change = captured_count - last_captured_count;
  last_captured_count = captured_count;

  // Calculate the average frequency of state transitions
  float average_frequency = 0.0f;
  if(count_change != 0 && captured_cumulative_time != INT_MAX) {
    average_frequency = (clocks_per_time * (float)count_change) / (float)captured_cumulative_time;
  }

  return Capture(captured_count, count_change, average_frequency, counts_per_revolution);
}

void Encoder::microstep_up(int32_t time) {
  enc_count++;
  time_since = time;
  microstep_time = 0;

  if(time + cumulative_time < time)  //Check to avoid integer overflow
    cumulative_time = INT_MAX;
  else
    cumulative_time += time;
}

void Encoder::microstep_down(int32_t time) {
  enc_count--;
  time_since = 0 - time;
  microstep_time = 0;

  if(time + cumulative_time < time)  //Check to avoid integer overflow
    cumulative_time = INT_MAX;
  else
    cumulative_time += time;
}

void Encoder::check_for_transition() {
  while(pio->ints1 & (PIO_IRQ1_INTS_SM0_RXNEMPTY_BITS << sm)) {
    uint32_t received = pio_sm_get(pio, sm);

    // Extract the current and last encoder states from the received value
    enc_state_a = (bool)(received & STATE_A_MASK);
    enc_state_b = (bool)(received & STATE_B_MASK);
    uint8_t states = (received & STATES_MASK) >> 28;

    // Extract the time (in cycles) it has been since the last received
    int32_t time_received = (received & TIME_MASK) + ENC_DEBOUNCE_TIME;

    // For rotary encoders, only every fourth transition is cared about, causing an inaccurate time value
    // To address this we accumulate the times received and zero it when a transition is counted
    if(!count_microsteps) {
      if(time_received + microstep_time < time_received)  // Check to avoid integer overflow
        time_received = INT32_MAX;
      else
        time_received += microstep_time;
      microstep_time = time_received;
    }

    // Determine what transition occurred
    switch(LAST_STATE(states)) {
      //--------------------------------------------------
      case MICROSTEP_0:
        switch(CURR_STATE(states)) {
          // A ____|‾‾‾‾
          // B _________
          case MICROSTEP_1:
            if(count_microsteps)
              microstep_up(time_received);
            break;

          // A _________
          // B ____|‾‾‾‾
          case MICROSTEP_3:
            if(count_microsteps)
              microstep_down(time_received);
            break;
        }
        break;

      //--------------------------------------------------
      case MICROSTEP_1:
        switch(CURR_STATE(states)) {
          // A ‾‾‾‾‾‾‾‾‾
          // B ____|‾‾‾‾
          case MICROSTEP_2:
            if(count_microsteps || last_travel_dir == CLOCKWISE)
              microstep_up(time_received);

            last_travel_dir = NO_DIR;  // Finished turning clockwise
            break;

          // A ‾‾‾‾|____
          // B _________
          case MICROSTEP_0:
            if(count_microsteps)
              microstep_down(time_received);
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
              microstep_up(time_received);
            
            last_travel_dir = CLOCKWISE;  // Started turning clockwise
            break;

          // A ‾‾‾‾‾‾‾‾‾
          // B ‾‾‾‾|____
          case MICROSTEP_1:
            if(count_microsteps)
              microstep_down(time_received);
            
            last_travel_dir = COUNTERCLOCK; // Started turning counter-clockwise
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
              microstep_up(time_received);
            break;

          // A ____|‾‾‾‾
          // B ‾‾‾‾‾‾‾‾‾
          case MICROSTEP_2:
            if(count_microsteps || last_travel_dir == COUNTERCLOCK)
              microstep_down(time_received);

            last_travel_dir = NO_DIR;    // Finished turning counter-clockwise
            break;
        }
        break;
    }
  }
}
}