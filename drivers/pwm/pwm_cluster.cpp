#include "pwm_cluster.hpp"
#include <cstdio> // TOREMOVE once done debugging
#include "hardware/gpio.h" // TOREMOVE once done debugging
#include "hardware/clocks.h"
#include "pwm_cluster.pio.h"

// Uncomment the below line to enable debugging
#define DEBUG_MULTI_PWM

namespace pimoroni {

#ifdef DEBUG_MULTI_PWM
    static const uint DEBUG_SIDESET = 17;
#endif

int data_dma_channel;
int ctrl_dma_channel;

static const uint BUFFER_SIZE = 64;     // Set to 64, the maximum number of single rises and falls for 32 channels within a looping time period
struct alignas(8) Transition {
  uint32_t mask;
  uint32_t delay;
  Transition() : mask(0), delay(0) {};
};
static const uint NUM_BUFFERS = 3;
static const uint LOADING_ZONE_SIZE = 3;
struct Sequence {
  uint32_t size;
  Transition data[BUFFER_SIZE];
  Sequence() : size(1), data({Transition()}) {};
};


Sequence sequences[NUM_BUFFERS];
uint sequence_index = 0;

volatile uint read_index = 0;
volatile uint last_written_index = 0;

const bool use_loading_zone = true;


uint irq_gpio = 15;
uint write_gpio = 16;

void __isr pwm_dma_handler() {
  // Clear the interrupt request.
  dma_hw->ints0 = 1u << data_dma_channel;

  gpio_put(irq_gpio, 1); //TOREMOVE Just for debug

  // If new data been written since the last time, switch to reading that buffer
  if(last_written_index != read_index) {
    read_index = last_written_index;
  }

  uint32_t transitions = sequences[read_index].size * 2;
  uint32_t* buffer = (uint32_t *)sequences[read_index].data;

  dma_channel_set_trans_count(data_dma_channel, transitions, false);
  dma_channel_set_read_addr(data_dma_channel, buffer, true);

  gpio_put(irq_gpio, 0); //TOREMOVE Just for debug
}

    /***
     * From RP2040 datasheet
     * *
     * One disadvantage of this technique is that we don’t start to reconfigure the channel until some time after the channel
makes its last transfer. If there is heavy interrupt activity on the processor, this may be quite a long time, and therefore
quite a large gap in transfers, which is problematic if we need to sustain a high data throughput.
This is solved by using two channels, with their CHAIN_TO fields crossed over, so that channel A triggers channel B when it
completes, and vice versa. At any point in time, one of the channels is transferring data, and the other is either already
configured to start the next transfer immediately when the current one finishes, or it is in the process of being
reconfigured. When channel A completes, it immediately starts the cued-up transfer on channel B. At the same time, the
interrupt is fired, and the handler reconfigures channel A so that it is ready for when channel B completes.
     * */
    

PWMCluster::PWMCluster(PIO pio, uint sm, uint channel_mask) : pio(pio), sm(sm), channel_mask(channel_mask) {
#ifdef DEBUG_MULTI_PWM
  pio_program_offset = pio_add_program(pio, &debug_pwm_cluster_program);
#else
  pio_program_offset = pio_add_program(pio, &pwm_cluster_program);
#endif
  channel_polarities = 0x00000000;
  wrap_level = 0;

  gpio_init(irq_gpio);
  gpio_set_dir(irq_gpio, GPIO_OUT);
  gpio_init(write_gpio);
  gpio_set_dir(write_gpio, GPIO_OUT);

  // Initialise all the channels this PWM will control
  for(uint channel = 0; channel < NUM_BANK0_GPIOS; channel++) {
    if(bit_in_mask(channel, channel_mask)) {
      pio_gpio_init(pio, channel);
      channel_levels[channel] = 0u;
      channel_offsets[channel] = 0u;
    }
  }

  // Set their default state and direction
  pio_sm_set_pins_with_mask(pio, sm, 0x00, channel_mask);
  pio_sm_set_pindirs_with_mask(pio, sm, channel_mask, channel_mask);

#ifdef DEBUG_MULTI_PWM
  pio_gpio_init(pio, DEBUG_SIDESET);
  pio_sm_set_consecutive_pindirs(pio, sm, DEBUG_SIDESET, 1, true);
#endif

#ifdef DEBUG_MULTI_PWM
  pio_sm_config c = debug_pwm_cluster_program_get_default_config(pio_program_offset);
#else
  pio_sm_config c = pwm_cluster_program_get_default_config(pio_program_offset);
#endif
  sm_config_set_out_pins(&c, 0, irq_gpio); //TODO change this to be 32
#ifdef DEBUG_MULTI_PWM
  sm_config_set_sideset_pins(&c, DEBUG_SIDESET);
#endif
  sm_config_set_out_shift(&c, false, true, 32);
  sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_NONE); // We actively do not want a joined FIFO even though we are not needing the RX


  float div = clock_get_hz(clk_sys) / 5000000;
  sm_config_set_clkdiv(&c, div);

  pio_sm_init(pio, sm, pio_program_offset, &c);
  pio_sm_set_enabled(pio, sm, true);

  data_dma_channel = dma_claim_unused_channel(true);
  /*ctrl_dma_channel = dma_claim_unused_channel(true);

  dma_channel_config ctrl_config = dma_channel_get_default_config(ctrl_dma_channel);
  channel_config_set_transfer_data_size(&ctrl_config, DMA_SIZE_32);
  //channel_config_set_read_increment(&ctrl_config, false);
  //channel_config_set_write_increment(&ctrl_config, false);
  channel_config_set_read_increment(&ctrl_config, true);
  channel_config_set_write_increment(&ctrl_config, true);
  channel_config_set_ring(&ctrl_config, true, 3); // 1 << 3 byte boundary on write ptr
  channel_config_set_ring(&ctrl_config, false, 3); // 1 << 3 byte boundary on read ptr

  dma_channel_configure(
    ctrl_dma_channel,
    &ctrl_config,
    //The below two work
    //&dma_hw->ch[data_dma_channel].al1_transfer_count_trig,
    //&transfer_count,
    //1,
    //These two do not
    //&dma_hw->ch[data_dma_channel].al3_read_addr_trig,
    //&((uint32_t *)buffer),
    &dma_hw->ch[data_dma_channel].al3_transfer_count, // Initial write address
    &control_blocks[0],
    2,
    false
  );*/

  dma_channel_config data_config = dma_channel_get_default_config(data_dma_channel);
  channel_config_set_bswap(&data_config, false);
  channel_config_set_dreq(&data_config, pio_get_dreq(pio, sm, true));
  channel_config_set_transfer_data_size(&data_config, DMA_SIZE_32);
  channel_config_set_read_increment(&data_config, true);
  //channel_config_set_chain_to(&data_config, ctrl_dma_channel);
  //channel_config_set_ring(&data_config, false, 7);

  dma_channel_configure(
    data_dma_channel,
    &data_config,
    &pio->txf[sm],
    NULL,
    0,
    false);

  dma_channel_set_irq0_enabled(data_dma_channel, true);

  // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
  irq_set_exclusive_handler(DMA_IRQ_0, pwm_dma_handler);
  irq_set_enabled(DMA_IRQ_0, true);

  // Set up the transition buffers
  for(uint i = 0; i < NUM_BUFFERS; i++) {
    Sequence& seq = sequences[i];
    seq = Sequence();
  }

  // Manually call the handler once, to trigger the first transfer
  pwm_dma_handler();

  //dma_start_channel_mask(1u << ctrl_dma_channel);
}

PWMCluster::~PWMCluster() {
  dma_channel_unclaim(data_dma_channel);
  dma_channel_unclaim(ctrl_dma_channel);
  pio_sm_set_enabled(pio, sm, false);
#ifdef DEBUG_MULTI_PWM
  pio_remove_program(pio, &debug_pwm_cluster_program, pio_program_offset);
#else
  pio_remove_program(pio, &pwm_cluster_program, pio_program_offset);
#endif
#ifndef MICROPY_BUILD_TYPE
  // pio_sm_unclaim seems to hardfault in MicroPython
  pio_sm_unclaim(pio, sm);
#endif
  // Reset all the pins this PWM will control back to an unused state
  for(uint pin = 0; pin < 32; pin++) { // 32 is number of bits
    if((1u << pin) != 0) {
      gpio_set_function(pin, GPIO_FUNC_NULL);
    }
  }
}

uint PWMCluster::get_chan_mask() const {
  return channel_mask;
}

void PWMCluster::set_wrap(uint32_t wrap, bool load) {
  wrap_level = MAX(wrap, 1);  // Cannot have a wrap of zero!
  if(load)
    load_pwm();
}

void PWMCluster::set_chan_level(uint8_t channel, uint32_t level, bool load) {
  if((channel < NUM_BANK0_GPIOS) && bit_in_mask(channel, channel_mask)) {
    channel_levels[channel] = level;
    if(load)
      load_pwm();
  }
}

void PWMCluster::set_chan_offset(uint8_t channel, uint32_t offset, bool load) {
  if((channel < NUM_BANK0_GPIOS) && bit_in_mask(channel, channel_mask)) {
    channel_offsets[channel] = offset;
    if(load)
      load_pwm();
  }
}

void PWMCluster::set_chan_polarity(uint8_t channel, bool polarity, bool load) {
  if((channel < NUM_BANK0_GPIOS) && bit_in_mask(channel, channel_mask)) {
    if(polarity)
      channel_polarities |= (1u << channel);
    else
      channel_polarities &= ~(1u << channel);
    if(load)
      load_pwm();
  }
}

// These apply immediately, so do not obey the PWM update trigger
void PWMCluster::set_clkdiv(float divider) {
  pio_sm_set_clkdiv(pio, sm, divider);
}

// These apply immediately, so do not obey the PWM update trigger
void PWMCluster::set_clkdiv_int_frac(uint16_t integer, uint8_t fract) {
  pio_sm_set_clkdiv_int_frac(pio, sm, integer, fract);
}

/*
void PWMCluster::set_phase_correct(bool phase_correct);

void PWMCluster::set_enabled(bool enabled);*/

void PWMCluster::load_pwm() {
  gpio_put(write_gpio, 1);

  TransitionData transitions[64];
  uint data_size = 0;

  // Go through each channel that we are assigned to
  for(uint channel = 0; channel < NUM_BANK0_GPIOS; channel++) {
    if(bit_in_mask(channel, channel_mask)) {
      // Get the channel polarity, remembering that true means inverted
      bool polarity = bit_in_mask(channel, channel_polarities);

      // If the level is greater than zero, add a transition to high
      if(channel_levels[channel] > 0) {
        PWMCluster::sorted_insert(transitions, data_size, TransitionData(channel, channel_offsets[channel], !polarity));
        //if((channel_offsets[channel] < wrap_level) && (channel_offsets[channel] + channel_levels[channel] >= wrap_level)) {
        //    PWMCluster::sorted_insert(transitions, data_size, TransitionData(channel, 0, !polarity)) // Adds an initial state for PWMs that have their end offset beyond the transition line
        //}
      }
      // If the level is less than the wrap, add a transition to low
      if(channel_levels[channel] < wrap_level) {
        PWMCluster::sorted_insert(transitions, data_size, TransitionData(channel, channel_offsets[channel] + channel_levels[channel], polarity));
      }
    }
  }

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

  Sequence& seq = sequences[write_index];
  seq.size = 0; // Reset the sequence, otherwise we end up appending and weird things happen

  if(data_size > 0) {
    uint pin_states = channel_polarities;
    uint data_index = 0;
    uint current_level = 0;

    // Populate the selected write sequence with pin states and delays
    while(data_index < data_size) {
      uint next_level = wrap_level; // Set the next level to be the wrap, initially

      do {
        // Is the level of this transition at the current level being checked?
        if(transitions[data_index].level <= current_level) {
          // Yes, so add the transition state to the pin states mask
          if(transitions[data_index].state)
            pin_states |= (1u << transitions[data_index].channel);
          else
            pin_states &= ~(1u << transitions[data_index].channel);

          data_index++; // Move on to the next transition
        }
        else {
          // No, it is higher, so set it as our next level and break out of the loop
          next_level = transitions[data_index].level;
          break;
        }
      } while(data_index < data_size);

      // Add the transition to the sequence
      seq.data[seq.size].mask = pin_states;
      seq.data[seq.size].delay = (next_level - current_level) - 1;
      seq.size++;

      current_level = next_level;
    }
  }
  else {
    // There were no transitions (either because there was a zero wrap, or no channels because there was a zero wrap?),
    // so initialise the sequence with something, so the PIO functions correctly
    seq.data[seq.size].mask = 0u;
    seq.data[seq.size].delay = wrap_level - 1;
    seq.size++;
  }

  // Introduce "Loading Zone" transitions to the end of the sequence
  // to prevent the DMA interrupt firing many milliseconds before the
  // sequence end.
  // TODO, have this account for PWMS close to 100% that may overlap with it
  seq.data[seq.size - 1].delay -= LOADING_ZONE_SIZE;
  for(uint i = 0; i < LOADING_ZONE_SIZE; i++) {
    seq.data[seq.size].mask = channel_polarities;
    seq.data[seq.size].delay = 0;
    seq.size++;
  }

  // Update the last written index so that the next DMA interrupt picks up the new sequence
  last_written_index = write_index;

  gpio_put(write_gpio, 0); //TOREMOVE
}

bool PWMCluster::bit_in_mask(uint bit, uint mask) {
  return ((1u << bit) & mask) != 0;
}

void PWMCluster::sorted_insert(TransitionData array[], uint &size, const TransitionData &data) {
  uint i;
  for(i = size; (i > 0 && !array[i - 1].compare(data)); i--) {
    array[i] = array[i - 1];
  }
  array[i] = data;
  //printf("Added %d, %ld, %d\n", data.channel, data.level, data.state);
  size++;
}
}