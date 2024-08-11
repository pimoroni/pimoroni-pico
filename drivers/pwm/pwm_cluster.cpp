#include "pwm_cluster.hpp"
#include "hardware/gpio.h"
#include "hardware/clocks.h"

#ifndef NO_QSTR
#include "pwm_cluster.pio.h"
#endif

// Uncomment the below line to enable debugging
//#define DEBUG_MULTI_PWM

namespace pimoroni {

#ifdef DEBUG_MULTI_PWM
  static const uint IRQ_GPIO = 15;
  static const uint WRITE_GPIO = 16;
  static const uint DEBUG_SIDESET = 17;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// STATICS
////////////////////////////////////////////////////////////////////////////////////////////////////
PWMCluster* PWMCluster::clusters[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
uint8_t PWMCluster::claimed_sms[] = { 0x0, 0x0 };
uint PWMCluster::pio_program_offset = 0;


PWMCluster::PWMCluster(PIO pio, uint sm, uint pin_mask, bool loading_zone)
: pio(pio)
, sm(sm)
, pin_mask(pin_mask & ((1u << NUM_BANK0_GPIOS) - 1))
, channel_count(0)
, wrap_level(0)
, loading_zone(loading_zone) {

  // Create the channel mapping
  for(uint pin = 0; pin < NUM_BANK0_GPIOS; pin++) {
    if(bit_in_mask(pin, pin_mask)) {
      channel_to_pin_map[channel_count] = pin;
      channel_count++;
    }
  }

  constructor_common();
}


PWMCluster::PWMCluster(PIO pio, uint sm, uint pin_base, uint pin_count, bool loading_zone)
: pio(pio)
, sm(sm)
, pin_mask(0x00000000)
, channel_count(0)
, wrap_level(0)
, loading_zone(loading_zone) {

  // Create the pin mask and channel mapping
  uint pin_end = MIN(pin_count + pin_base, NUM_BANK0_GPIOS);
  for(uint pin = pin_base; pin < pin_end; pin++) {
    pin_mask |= (1u << pin);
    channel_to_pin_map[channel_count] = pin;
    channel_count++;
  }

  constructor_common();
}

PWMCluster::PWMCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, bool loading_zone)
: pio(pio)
, sm(sm)
, pin_mask(0x00000000)
, channel_count(0)
, wrap_level(0)
, loading_zone(loading_zone) {

  // Create the pin mask and channel mapping
  for(uint i = 0; i < length; i++) {
    uint8_t pin = pins[i];
    if(pin < NUM_BANK0_GPIOS) {
      pin_mask |= (1u << pin);
      channel_to_pin_map[channel_count] = pin;
      channel_count++;
    }
  }

  constructor_common();
}

PWMCluster::PWMCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, bool loading_zone)
: pio(pio)
, sm(sm)
, pin_mask(0x00000000)
, channel_count(0)
, wrap_level(0)
, loading_zone(loading_zone) {

  // Create the pin mask and channel mapping
  for(auto pin : pins) {
    if(pin < NUM_BANK0_GPIOS) {
      pin_mask |= (1u << pin);
      channel_to_pin_map[channel_count] = pin;
      channel_count++;
    }
  }

  constructor_common();
}

PWMCluster::PWMCluster(PIO pio, uint sm, const pin_pair *pin_pairs, uint32_t length, bool loading_zone)
: pio(pio)
, sm(sm)
, pin_mask(0x00000000)
, channel_count(0)
, wrap_level(0)
, loading_zone(loading_zone) {

  // Create the pin mask and channel mapping
  for(uint i = 0; i < length; i++) {
    pin_pair pair = pin_pairs[i];
    if((pair.first < NUM_BANK0_GPIOS) && (pair.second < NUM_BANK0_GPIOS)) {
      pin_mask |= (1u << pair.first);
      channel_to_pin_map[channel_count] = pair.first;
      channel_count++;

      pin_mask |= (1u << pair.second);
      channel_to_pin_map[channel_count] = pair.second;
      channel_count++;
    }
  }

  constructor_common();
}

PWMCluster::PWMCluster(PIO pio, uint sm, std::initializer_list<pin_pair> pin_pairs, bool loading_zone)
: pio(pio)
, sm(sm)
, pin_mask(0x00000000)
, channel_count(0)
, wrap_level(0)
, loading_zone(loading_zone) {

  // Create the pin mask and channel mapping
  for(auto pair : pin_pairs) {
    if((pair.first < NUM_BANK0_GPIOS) && (pair.second < NUM_BANK0_GPIOS)) {
      pin_mask |= (1u << pair.first);
      channel_to_pin_map[channel_count] = pair.first;
      channel_count++;

      pin_mask |= (1u << pair.second);
      channel_to_pin_map[channel_count] = pair.second;
      channel_count++;
    }
  }

  constructor_common();
}

void PWMCluster::constructor_common() {
  // Initialise all the channels this PWM will control
  for(uint i = 0; i < channel_count; i++) {
    channels[i] = ChannelState();
  }

  // Set up the transition buffers
  for(uint i = 0; i < NUM_BUFFERS; i++) {
    // Need to set a delay otherwise a lockup occurs when first changing frequency
    sequences[i].data[0].delay = 10;
    loop_sequences[i].data[0].delay = 10;
  }
}

PWMCluster::~PWMCluster() {
  if(initialised) {
    pio_sm_set_enabled(pio, sm, false);

    // Tear down the DMA channel.
    // This is copied from: https://github.com/raspberrypi/pico-sdk/pull/744/commits/5e0e8004dd790f0155426e6689a66e08a83cd9fc
    uint32_t irq0_save = dma_hw->inte0 & (1u << dma_channel);
    hw_clear_bits(&dma_hw->inte0, irq0_save);

    dma_hw->abort = 1u << dma_channel;

    // To fence off on in-flight transfers, the BUSY bit should be polled
    // rather than the ABORT bit, because the ABORT bit can clear prematurely.
    while (dma_hw->ch[dma_channel].ctrl_trig & DMA_CH0_CTRL_TRIG_BUSY_BITS) tight_loop_contents();

    // Clear the interrupt (if any) and restore the interrupt masks.
    dma_hw->ints0 = 1u << dma_channel;
    hw_set_bits(&dma_hw->inte0, irq0_save);

    dma_channel_unclaim(dma_channel); // This works now the teardown behaves correctly
    clusters[dma_channel] = nullptr;

    pio_sm_unclaim(pio, sm);

    uint pio_idx = pio_get_index(pio);
    claimed_sms[pio_idx] &= ~(1u << sm);

    //If there are no more SMs using the encoder program, then we can remove it from the PIO
    if(claimed_sms[pio_idx] == 0) {
    #ifdef DEBUG_MULTI_PWM
      pio_remove_program(pio, &debug_pwm_cluster_program, pio_program_offset);
    #else
      pio_remove_program(pio, &pwm_cluster_program, pio_program_offset);
    #endif
    }

    if(claimed_sms[0] == 0 && claimed_sms[1] == 0) {
      irq_remove_handler(DMA_IRQ_0, dma_interrupt_handler);
    }

    // Reset all the pins this PWM will control back to an unused state
    for(uint channel = 0; channel < channel_count; channel++) {
      gpio_set_function(channel_to_pin_map[channel], GPIO_FUNC_NULL);
    }
  }
}

void PWMCluster::dma_interrupt_handler() {
  // Go through each dma channel to see which triggered this interrupt,
  // and if there's an associated cluster, have it advance to the next sequence
  for(uint8_t channel = 0; channel < NUM_DMA_CHANNELS; channel++) {
    if(dma_channel_get_irq0_status(channel) && clusters[channel] != nullptr) {
      clusters[channel]->next_dma_sequence();
    }
  }
}

void PWMCluster::next_dma_sequence() {
  #ifdef DEBUG_MULTI_PWM
    gpio_put(IRQ_GPIO, true);
  #endif

  // Clear any interrupt request caused by our channel
  dma_channel_acknowledge_irq0(dma_channel);

  // If new data been written since the last time, switch to reading
  // that sequence, otherwise continue with the looping sequence
  Sequence* seq;
  if(last_written_index != read_index) {
    read_index = last_written_index;
    seq = &sequences[read_index];
  }
  else {
    seq = &loop_sequences[read_index];
  }

  // Let the dma channel know the sequence size and data location
  dma_channel_set_trans_count(dma_channel, seq->size << 1, false);
  dma_channel_set_read_addr(dma_channel, seq->data, true);

  #ifdef DEBUG_MULTI_PWM
    gpio_put(IRQ_GPIO, false);
  #endif
}

bool PWMCluster::init() {
  if(!initialised && !pio_sm_is_claimed(pio, sm)) {
    dma_channel = dma_claim_unused_channel(false);
    if(dma_channel >= 0) {
      pio_sm_claim(pio, sm);
      uint pio_idx = pio_get_index(pio);

      // If this is the first time using a cluster on this PIO, add the program to the PIO memory
      if(claimed_sms[pio_idx] == 0) {
        #ifdef DEBUG_MULTI_PWM
          pio_program_offset = pio_add_program(pio, &debug_pwm_cluster_program);
        #else
          pio_program_offset = pio_add_program(pio, &pwm_cluster_program);
        #endif
      }

      #ifdef DEBUG_MULTI_PWM
        gpio_init(IRQ_GPIO);
        gpio_init(WRITE_GPIO);
        gpio_set_dir(IRQ_GPIO, GPIO_OUT);
        gpio_set_dir(WRITE_GPIO, GPIO_OUT);
      #endif

      // Initialise all the pins this PWM will control
      for(uint channel = 0; channel < channel_count; channel++) {
        pio_gpio_init(pio, channel_to_pin_map[channel]);
      }

      // Set their default state and direction
      pio_sm_set_pins_with_mask(pio, sm, 0x00, pin_mask);
      pio_sm_set_pindirs_with_mask(pio, sm, pin_mask, pin_mask);

    #ifdef DEBUG_MULTI_PWM
      pio_gpio_init(pio, DEBUG_SIDESET);
      pio_sm_set_consecutive_pindirs(pio, sm, DEBUG_SIDESET, 1, true);

      pio_sm_config c = debug_pwm_cluster_program_get_default_config(pio_program_offset);
      sm_config_set_out_pins(&c, 0, IRQ_GPIO);
      sm_config_set_sideset_pins(&c, DEBUG_SIDESET);
    #else
      pio_sm_config c = pwm_cluster_program_get_default_config(pio_program_offset);
      sm_config_set_out_pins(&c, 0, 32);
    #endif
      sm_config_set_out_shift(&c, false, true, 32);
      sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_NONE); // We actively do not want a joined FIFO even though we are not needing the RX

      float div = clock_get_hz(clk_sys) / 500000;
      sm_config_set_clkdiv(&c, div);

      dma_channel_config data_config = dma_channel_get_default_config(dma_channel);
      channel_config_set_bswap(&data_config, false);
      channel_config_set_dreq(&data_config, pio_get_dreq(pio, sm, true));
      channel_config_set_transfer_data_size(&data_config, DMA_SIZE_32);
      channel_config_set_read_increment(&data_config, true);

      dma_channel_configure(
        dma_channel,
        &data_config,
        &pio->txf[sm],
        NULL,
        0,
        false);

      dma_channel_set_irq0_enabled(dma_channel, true);

      pio_sm_init(pio, sm, pio_program_offset, &c);
      pio_sm_set_enabled(pio, sm, true);

      if(claimed_sms[0] == 0 && claimed_sms[1] == 0) {
        // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
        irq_add_shared_handler(DMA_IRQ_0, dma_interrupt_handler, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
        irq_set_enabled(DMA_IRQ_0, true);
      }

      //Keep a record of this cluster for the interrupt callback
      clusters[dma_channel] = this;
      claimed_sms[pio_idx] |= 1u << sm;

      // Manually set the next dma sequence to trigger the first transfer
      next_dma_sequence();

      initialised = true;
    }
  }

  return initialised;
}

uint8_t PWMCluster::get_chan_count() const {
  return channel_count;
}

uint8_t PWMCluster::get_chan_pair_count() const {
  return (channel_count / 2);
}

uint8_t PWMCluster::get_chan_pin(uint8_t channel) const {
  assert(channel < channel_count);
  return channel_to_pin_map[channel];
}

pin_pair PWMCluster::get_chan_pin_pair(uint8_t channel_pair) const {
  assert(channel_pair < get_chan_pair_count());
  uint8_t channel_base = channel_from_pair(channel_pair);
  return pin_pair(channel_to_pin_map[channel_base], channel_to_pin_map[channel_base + 1]);
}

uint8_t PWMCluster::channel_from_pair(uint8_t channel_pair) {
  return (channel_pair * 2);
}

uint32_t PWMCluster::get_chan_level(uint8_t channel) const {
  assert(channel < channel_count);
  return channels[channel].level;
}

void PWMCluster::set_chan_level(uint8_t channel, uint32_t level, bool load) {
  assert(channel < channel_count);
  channels[channel].level = level;
  if(load)
    load_pwm();
}

uint32_t PWMCluster::get_chan_offset(uint8_t channel) const {
  assert(channel < channel_count);
  return channels[channel].offset;
}

void PWMCluster::set_chan_offset(uint8_t channel, uint32_t offset, bool load) {
  assert(channel < channel_count);
  channels[channel].offset = offset;
  if(load)
    load_pwm();
}

bool PWMCluster::get_chan_polarity(uint8_t channel) const {
  assert(channel < channel_count);
  return channels[channel].polarity;
}

void PWMCluster::set_chan_polarity(uint8_t channel, bool polarity, bool load) {
  assert(channel < channel_count);
  channels[channel].polarity = polarity;
  if(load)
    load_pwm();
}

uint32_t PWMCluster::get_wrap() const {
  return wrap_level;
}

void PWMCluster::set_wrap(uint32_t wrap, bool load) {
  wrap_level = MAX(wrap, 1);  // Cannot have a wrap of zero!
  if(load)
    load_pwm();
}

// These apply immediately, so do not obey the PWM update trigger
void PWMCluster::set_clkdiv(float divider) {
  pio_sm_set_clkdiv(pio, sm, divider);
}

// These apply immediately, so do not obey the PWM update trigger
void PWMCluster::set_clkdiv_int_frac(uint16_t integer, uint8_t fract) {
  pio_sm_set_clkdiv_int_frac(pio, sm, integer, fract);
}

void PWMCluster::load_pwm() {
  #ifdef DEBUG_MULTI_PWM
    gpio_put(WRITE_GPIO, true);
  #endif

  uint data_size = 0;
  uint looping_data_size = 0;

  uint pin_states = 0; // Start with all pins low

  // Check if the data we last wrote has been picked up by the DMA yet?
  const bool read_since_last_write = (read_index == last_written_index);

  // Go through each channel that we are assigned to
  for(uint channel = 0; channel < channel_count; channel++) {
    ChannelState &state = channels[channel];

    // Invert this channel's initial state if it's polarity invert is set
    if(state.polarity) {
      pin_states |= (1u << channel_to_pin_map[channel]); // Set the pin
    }

    const uint channel_start = state.offset;
    const uint channel_end = (state.offset + state.level);
    const uint channel_wrapped_end = channel_end % wrap_level;

    // If the data has been read, copy the channel overruns from that sequence. Otherwise, keep the ones we had previously stored.
    if(read_since_last_write) {
      // This condition was added to deal with cases of load_pwm() being called multiple
      // times between DMA reads, and thus loosing memory of the previous sequence's overruns
      state.overrun = state.next_overrun;
    }
    state.next_overrun = 0u; // Always clear the next channel overruns, as we are loading new data

    // Did the previous sequence overrun the wrap level?
    if(state.overrun > 0) {
      // Flip the initial state so the pin starts "high" (or "low" if polarity inverted)
      pin_states ^= (1u << channel_to_pin_map[channel]);

      // Is our end level before our start level?
      if(channel_wrapped_end < channel_start) {
        // Yes, so add a transition to "low" (or "high" if polarity inverted) at the end level, rather than the overrun (so our pulse takes effect earlier)
        PWMCluster::sorted_insert(transitions, data_size, TransitionData(channel, channel_wrapped_end, state.polarity));
      }
      else if(state.overrun < channel_start) {
        // No, so add a transition to "low" (or "high" if polarity inverted) at the overrun instead
        PWMCluster::sorted_insert(transitions, data_size, TransitionData(channel, state.overrun, state.polarity));
      }
    }

    // Is the state level greater than zero, and the start level within the wrap?
    if(state.level > 0 && channel_start < wrap_level) {
      // Add a transition to "high" (or "low" if polarity inverted) at the start level
      PWMCluster::sorted_insert(transitions, data_size, TransitionData(channel, channel_start, !state.polarity));
      PWMCluster::sorted_insert(looping_transitions, looping_data_size, TransitionData(channel, channel_start, !state.polarity));

      // If the channel has overrun the wrap level, record by how much
      if(channel_wrapped_end < channel_start) {
          state.next_overrun = channel_wrapped_end;
      }
    }

    // Is the state level within the wrap?
    if(state.level < wrap_level) {
      // Is the end level within the wrap too?
      if(channel_end < wrap_level) {
        // Add a transition to "low" (or "high" if polarity inverted) at the end level
        PWMCluster::sorted_insert(transitions, data_size, TransitionData(channel, channel_end, state.polarity));
      }

      // Add a transition to "low" (or "high" if polarity inverted) at the wrapped end level
      PWMCluster::sorted_insert(looping_transitions, looping_data_size, TransitionData(channel, channel_wrapped_end, state.polarity));
    }
  }

  #ifdef DEBUG_MULTI_PWM
    gpio_put(WRITE_GPIO, false);
  #endif

  if(loading_zone) {
    // Introduce "Loading Zone" transitions to the end of the sequence to
    // prevent the DMA interrupt firing many milliseconds before the sequence ends.
    uint32_t zone_inserts = MIN(LOADING_ZONE_SIZE, wrap_level - LOADING_ZONE_POSITION);
    for(uint32_t i = zone_inserts + LOADING_ZONE_POSITION; i > LOADING_ZONE_POSITION; i--) {
      PWMCluster::sorted_insert(transitions, data_size, TransitionData(wrap_level - i));
      PWMCluster::sorted_insert(looping_transitions, looping_data_size, TransitionData(wrap_level - i));
    }
  }

  #ifdef DEBUG_MULTI_PWM
    gpio_put(WRITE_GPIO, true);
  #endif

  // Read | Last W = Write
  // 0    | 0      = 1 (or 2)
  // 0    | 1      = 2
  // 0    | 2      = 1
  // 1    | 0      = 2
  // 1    | 1      = 2 (or 0)
  // 1    | 2      = 0
  // 2    | 0      = 1
  // 2    | 1      = 0
  // 2    | 2      = 0 (or 1)

  // Choose the write index based on the read and last written indices (using the above table)
  uint write_index = (read_index + 1) % NUM_BUFFERS;
  if(write_index == last_written_index) {
    write_index = (write_index + 1) % NUM_BUFFERS;
  }

  populate_sequence(transitions, data_size, sequences[write_index], pin_states);
  populate_sequence(looping_transitions, looping_data_size, loop_sequences[write_index], pin_states);

  // Update the last written index so that the next DMA interrupt picks up the new sequence
  last_written_index = write_index;

  #ifdef DEBUG_MULTI_PWM
    gpio_put(WRITE_GPIO, false);
  #endif
}

// Derived from the rp2 Micropython implementation: https://github.com/micropython/micropython/blob/master/ports/rp2/machine_pwm.c
bool PWMCluster::calculate_pwm_factors(float freq, uint32_t& top_out, uint32_t& div256_out) {
  bool success = false;
  uint32_t source_hz = clock_get_hz(clk_sys) / PWM_CLUSTER_CYCLES;

  // Check the provided frequency is valid
  if((freq >= 0.01f) && (freq <= (float)(source_hz >> 1))) {
    uint64_t div256_top = (uint64_t)((float)((uint64_t)source_hz << 8) / freq);
    uint64_t top = 1;

    while(true) {
        // Try a few small prime factors to get close to the desired frequency.
        if((div256_top >= (11 << 8)) && (div256_top % 11 == 0) && (top * 11 <= MAX_PWM_CLUSTER_WRAP)) {
            div256_top /= 11;
            top *= 11;
        }
        else if((div256_top >= (7 << 8)) && (div256_top % 7 == 0) && (top * 7 <= MAX_PWM_CLUSTER_WRAP)) {
            div256_top /= 7;
            top *= 7;
        }
        else if((div256_top >= (5 << 8)) && (div256_top % 5 == 0) && (top * 5 <= MAX_PWM_CLUSTER_WRAP)) {
            div256_top /= 5;
            top *= 5;
        }
        else if((div256_top >= (3 << 8)) && (div256_top % 3 == 0) && (top * 3 <= MAX_PWM_CLUSTER_WRAP)) {
            div256_top /= 3;
            top *= 3;
        }
        else if((div256_top >= (2 << 8)) && (top * 2 <= MAX_PWM_CLUSTER_WRAP)) {
            div256_top /= 2;
            top *= 2;
        }
        else {
            break;
        }
    }

    // Only return valid factors if the divisor is actually achievable
    if(div256_top >= 256 && div256_top <= (UINT8_MAX << 8)) {
      top_out = top;
      div256_out = div256_top;

      success = true;
    }
  }
  return success;
}

bool PWMCluster::bit_in_mask(uint bit, uint mask) {
  return ((1u << bit) & mask) != 0;
}

void PWMCluster::sorted_insert(TransitionData array[], uint &size, const TransitionData &data) {
  uint i = size;
  for(; (i > 0 && array[i - 1].level > data.level); i--) {
    array[i] = array[i - 1];
  }
  array[i] = data;
  size++;
}

void PWMCluster::populate_sequence(const TransitionData transitions[], const uint &data_size, Sequence &seq_out, uint &pin_states_in_out) const {
  seq_out.size = 0; // Reset the sequence, otherwise we end up appending and weird things happen

  if(data_size > 0) {
    uint data_index = 0;
    uint current_level = 0;

    // Populate the selected write sequence with pin states and delays
    while(data_index < data_size) {
      uint next_level = wrap_level; // Set the next level to be the wrap, initially

      do {
        const TransitionData &transition = transitions[data_index];

        // Is the level of this transition at the current level being checked?
        if(transition.level <= current_level) {
          // Yes, so add the transition state to the pin states mask, if it's not a dummy transition
          if(!transition.dummy) {
            if(transition.state)
              pin_states_in_out |= (1u << channel_to_pin_map[transition.channel]);
            else
              pin_states_in_out &= ~(1u << channel_to_pin_map[transition.channel]);
          }

          data_index++; // Move on to the next transition
        }
        else {
          // No, it is higher, so set it as our next level and break out of the loop
          next_level = transition.level;
          break;
        }
      } while(data_index < data_size);

      // Add the transition to the sequence
      seq_out.data[seq_out.size].mask = pin_states_in_out;
      seq_out.data[seq_out.size].delay = (next_level - current_level) - 1;
      seq_out.size++;

      current_level = next_level;
    }
  }
  else {
    // There were no transitions (either because there was a zero wrap, or no channels because there was a zero wrap?),
    // so initialise the sequence with something, so the PIO functions correctly
    seq_out.data[seq_out.size].mask = 0u;
    seq_out.data[seq_out.size].delay = wrap_level - 1;
    seq_out.size++;
  }
}
}