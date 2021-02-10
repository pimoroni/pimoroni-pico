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
  Encoder* Encoder::pio_encoders[][NUM_PIO_STATE_MACHINES] = { { nullptr, nullptr, nullptr, nullptr }, { nullptr, nullptr, nullptr, nullptr } };
  uint8_t Encoder::pio_claimed_sms[] = { 0x0, 0x0 };

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  void Encoder::pio0_interrupt_callback() {
    //Go through each of encoders on this PIO to see which triggered this interrupt
    for(uint8_t sm = 0; sm < NUM_PIO_STATE_MACHINES; sm++) {
      if(pio_encoders[0][sm] != nullptr) { 
        pio_encoders[0][sm]->check_for_transition(); 
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  void Encoder::pio1_interrupt_callback() {
    //Go through each of encoders on this PIO to see which triggered this interrupt
    for(uint8_t sm = 0; sm < NUM_PIO_STATE_MACHINES; sm++) {
      if(pio_encoders[1][sm] != nullptr) { 
        pio_encoders[1][sm]->check_for_transition(); 
      }
    }
  }



  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // CONSTRUCTORS / DESTRUCTOR
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  Encoder::Encoder(PIO pio, uint8_t pinA, uint8_t pinB, uint8_t pinC,
                   float counts_per_revolution, bool count_microsteps, 
                   uint16_t freq_divider) :
    enc_pio(pio), pinA(pinA), pinB(pinB), pinC(pinC),
    counts_per_revolution(counts_per_revolution), count_microsteps(count_microsteps),
    freq_divider(freq_divider), clocks_per_time((float)(clock_get_hz(clk_sys) / (ENC_LOOP_CYCLES * freq_divider))) {
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  Encoder::~Encoder() {
    //Clean up our use of the SM associated with this encoder
    encoder_program_release(enc_pio, enc_sm);
    uint index = pio_get_index(enc_pio);
    pio_encoders[index][enc_sm] = nullptr;
    pio_claimed_sms[index] &= ~(1u << enc_sm);

    //If there are no more SMs using the encoder program, then we can remove it from the PIO
    if(pio_claimed_sms[index] == 0) {
      pio_remove_program(enc_pio, &encoder_program, enc_offset);
    }
  }



  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // METHODS
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  bool Encoder::init() {
    bool initialised = false;

    //Are the pins we want to use actually valid?
    if((pinA < NUM_BANK0_GPIOS) && (pinB < NUM_BANK0_GPIOS)) {

      //If a Pin C was defined, and valid, set it as a GND to pull the other two pins down
      if((pinC != PIN_UNUSED) && (pinC < NUM_BANK0_GPIOS)) {
        gpio_init(pinC);
        gpio_set_dir(pinC, GPIO_OUT);
        gpio_put(pinC, false);
      }

      enc_sm = pio_claim_unused_sm(enc_pio, true);
      uint pio_idx = pio_get_index(enc_pio);

      //Is this the first time using an encoder on this PIO?      
      if(pio_claimed_sms[pio_idx] == 0) {
        //Add the program to the PIO memory and enable the appropriate interrupt
        enc_offset = pio_add_program(enc_pio, &encoder_program);
        encoder_program_init(enc_pio, enc_sm, enc_offset, pinA, pinB, freq_divider);
        hw_set_bits(&enc_pio->inte0, PIO_IRQ0_INTE_SM0_RXNEMPTY_BITS << enc_sm);
        if(pio_idx == 0) {
          irq_set_exclusive_handler(PIO0_IRQ_0, pio0_interrupt_callback);
          irq_set_enabled(PIO0_IRQ_0, true);
        }
        else {
          irq_set_exclusive_handler(PIO1_IRQ_0, pio1_interrupt_callback);
          irq_set_enabled(PIO1_IRQ_0, true);
        }
      }

      //Keep a record of this encoder for the interrupt callback
      pio_encoders[pio_idx][enc_sm] = this;
      pio_claimed_sms[pio_idx] |= 1u << enc_sm;

      //Read the current state of the encoder pins and start the PIO program on the SM
      stateA = gpio_get(pinA);
      stateB = gpio_get(pinB);
      encoder_program_start(enc_pio, enc_sm, stateA, stateB);

      initialised = true;
    }
    return initialised;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  bool Encoder::get_state_a() const {
    return stateA;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  bool Encoder::get_state_b() const {
    return stateB;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  int32_t Encoder::get_count() const {
    return count - count_offset;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  float Encoder::get_revolutions() const {
    return (float)get_count() / counts_per_revolution;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  float Encoder::get_angle_degrees() const {
    return get_revolutions() * 360.0f;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  float Encoder::get_angle_radians() const {
    return get_revolutions() * M_TWOPI;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  float Encoder::get_frequency() const {
    return clocks_per_time / (float)time_since;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  float Encoder::get_revolutions_per_second() const {
    return get_frequency() / counts_per_revolution;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  float Encoder::get_revolutions_per_minute() const {
    return get_revolutions_per_second() * 60.0f;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  float Encoder::get_degrees_per_second() const {
    return get_revolutions_per_second() * 360.0f;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  float Encoder::get_radians_per_second() const {
    return get_revolutions_per_second() * M_TWOPI;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  void Encoder::zero_count() {
    count_offset = count;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  Capture Encoder::perform_capture() {
    //Capture the current values
		int32_t captured_count = count;
		int32_t captured_cumulative_time = cumulative_time;
		cumulative_time = 0;

    //Determine the change in counts since the last capture was performed
		int32_t count_change = captured_count - last_captured_count;
		last_captured_count = captured_count;

    //Calculate the average frequency of state transitions    
    float average_frequency = 0.0f;
		if(count_change != 0 && captured_cumulative_time != INT_MAX) {
      average_frequency = (clocks_per_time * (float)count_change) / (float)captured_cumulative_time;
    }

    return Capture(captured_count, count_change, average_frequency, counts_per_revolution);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  void Encoder::microstep_up(int32_t time) {
    count++;
    time_since = time;
    microstep_time = 0;

    if(time + cumulative_time < time)  //Check to avoid integer overflow
      cumulative_time = INT_MAX;
    else
      cumulative_time += time;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  void Encoder::microstep_down(int32_t time) {
    count--;
    time_since = 0 - time;
    microstep_time = 0;

    if(time + cumulative_time < time)  //Check to avoid integer overflow
      cumulative_time = INT_MAX;
    else
      cumulative_time += time;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  void Encoder::check_for_transition() {
    while(enc_pio->ints0 & (PIO_IRQ0_INTS_SM0_RXNEMPTY_BITS << enc_sm)) {
      uint32_t received = pio_sm_get(enc_pio, enc_sm);

      // Extract the current and last encoder states from the received value
      stateA = (bool)(received & STATE_A_MASK);
      stateB = (bool)(received & STATE_B_MASK);
      uint8_t states = (received & STATES_MASK) >> 28;  

      // Extract the time (in cycles) it has been since the last received
      int32_t time_received = (received & TIME_MASK) + ENC_DEBOUNCE_TIME;

      // For rotary encoders, only every fourth transition is cared about, causing an inaccurate time value
      // To address this we accumulate the times received and zero it when a transition is counted
      if(!count_microsteps) {
        if(time_received + microstep_time < time_received)  //Check to avoid integer overflow
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

              last_travel_dir = NO_DIR;  //Finished turning clockwise
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
              
              last_travel_dir = CLOCKWISE;  //Started turning clockwise
              break;

            // A ‾‾‾‾‾‾‾‾‾
            // B ‾‾‾‾|____
            case MICROSTEP_1:
              if(count_microsteps)
                microstep_down(time_received);
              
              last_travel_dir = COUNTERCLOCK; //Started turning counter-clockwise
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
              
              last_travel_dir = NO_DIR;    //Finished turning counter-clockwise
              break;
          }
          break;
      }
    }
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////
}