#pragma once

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "common/pimoroni_common.hpp"
#include <initializer_list>
#include <cstddef>

// PWMCluster's buffer block is claimed and released through these, so a port can supply
// memory from its own heap. The block is a DMA source, so it must be somewhere the DMA
// reads coherently with the CPU's writes, in practice SRAM. The defaults, defined weak in
// pwm_cluster.cpp, take the C heap, which the RP2 ports place in SRAM. Returns nullptr on
// failure; init() reports that to its caller.
extern "C" void* pwm_cluster_allocate(size_t size);
extern "C" void pwm_cluster_deallocate(void* mem);

namespace pimoroni {


  class PWMCluster {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  private:
    static const uint64_t MAX_PWM_CLUSTER_WRAP = UINT16_MAX;  // UINT32_MAX works too, but seems to produce less accurate counters
    static const uint32_t LOADING_ZONE_SIZE = 3;              // The number of dummy transitions to insert into the data to delay the DMA interrupt (if zero then no zone is used)
    static const uint32_t LOADING_ZONE_POSITION = 55;         // The number of levels before the wrap level to insert the load zone
                                                              // Smaller values will make the DMA interrupt trigger closer to the time the data is needed,
                                                              // but risks stalling the PIO if the interrupt takes longer due to other processes
    static const bool DEFAULT_USE_LOADING_ZONE = true;        // Whether or not the default behaviour of PWMCluster is to use the loading zone
  public:
    static const uint NUM_BUFFERS = 3;
    static const uint MAX_PWM_CHANNELS = 32;                  // The most channels a single state machine can drive, from its "out pins, 32"
  private:
    static const uint CHANNEL_LIMIT = (MAX_PWM_CHANNELS < NUM_BANK0_GPIOS) ? MAX_PWM_CHANNELS : NUM_BANK0_GPIOS;

    // A load inserts up to three transitions per channel into the one-shot data (a fall for a
    // previous overrun, a rise, and a fall), two per channel into the looping data, plus the
    // loading zone. A sequence holds one entry more than its source array, for the leading
    // delay emitted when no transition sits at level zero.
    static const uint TRANSITION_LIMIT = (CHANNEL_LIMIT * 3) + LOADING_ZONE_SIZE;
    static const uint LOOP_TRANSITION_LIMIT = (CHANNEL_LIMIT * 2) + LOADING_ZONE_SIZE;


    //--------------------------------------------------
    // Substructures
    //--------------------------------------------------
  public:
    struct Transition {
      //--------------------------------------------------
      // Variables
      //--------------------------------------------------
      uint32_t mask;
      uint32_t delay;


      //--------------------------------------------------
      // Constructors/Destructor
      //--------------------------------------------------
      Transition() : mask(0), delay(0) {};
    };

    struct Sequence {
      //--------------------------------------------------
      // Variables
      //--------------------------------------------------
      uint32_t size;
      Transition* data;


      //--------------------------------------------------
      // Constructors/Destructor
      //--------------------------------------------------
      Sequence() : size(1), data(nullptr) {};
    };

    struct TransitionData {
      //--------------------------------------------------
      // Variables
      //--------------------------------------------------
      uint8_t channel;
      uint32_t level;
      bool state;
      bool dummy;


      //--------------------------------------------------
      // Constructors/Destructor
      //--------------------------------------------------
      TransitionData() : channel(0), level(0), state(false), dummy(false) {};
      TransitionData(uint8_t channel, uint32_t level, bool new_state) : channel(channel), level(level), state(new_state), dummy(false) {};
      TransitionData(uint32_t level) : channel(0), level(level), state(false), dummy(true) {};
    };

  private:
    struct ChannelState {
      //--------------------------------------------------
      // Variables
      //--------------------------------------------------
      uint level;
      uint offset;
      bool polarity;
      uint overrun;
      uint next_overrun;


      //--------------------------------------------------
      // Constructors/Destructor
      //--------------------------------------------------
      ChannelState() : level(0), offset(0), polarity(false), overrun(0), next_overrun(0) {}
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    PIO pio;
    uint sm;
    int dma_channel;
    uint64_t pin_mask;
    uint8_t channel_count;
    uint8_t channel_to_pin_map[CHANNEL_LIMIT];
    uint wrap_level;

    // The channel states and both sequence sets live in one block, sized for channel_count
    // and claimed by init() through pwm_cluster_allocate
    uint8_t* allocation = nullptr;
    ChannelState* channels = nullptr;
    Sequence sequences[NUM_BUFFERS];
    Sequence loop_sequences[NUM_BUFFERS];

    volatile uint read_index = 0;
    volatile uint last_written_index = 0;

    bool initialised = false;
    bool loading_zone = true;


    //--------------------------------------------------
    // Statics
    //--------------------------------------------------
    static PWMCluster* clusters[NUM_DMA_CHANNELS];
    static uint8_t claimed_sms[NUM_PIOS];
    static uint pio_program_offsets[NUM_PIOS];

    // Scratch used by load_pwm, shared by every cluster under a mutex
    static TransitionData transitions[TRANSITION_LIMIT];
    static TransitionData looping_transitions[LOOP_TRANSITION_LIMIT];

    static void dma_interrupt_handler();


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    PWMCluster(PIO pio, uint sm, uint64_t pin_mask, bool loading_zone = DEFAULT_USE_LOADING_ZONE);
    PWMCluster(PIO pio, uint sm, uint pin_base, uint pin_count, bool loading_zone = DEFAULT_USE_LOADING_ZONE);
    PWMCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, bool loading_zone = DEFAULT_USE_LOADING_ZONE);
    PWMCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, bool loading_zone = DEFAULT_USE_LOADING_ZONE);

    PWMCluster(PIO pio, uint sm, const pin_pair *pin_pairs, uint32_t length, bool loading_zone = DEFAULT_USE_LOADING_ZONE);
    PWMCluster(PIO pio, uint sm, std::initializer_list<pin_pair> pin_pairs, bool loading_zone = DEFAULT_USE_LOADING_ZONE);
    ~PWMCluster();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    uint8_t get_chan_count() const;
    uint8_t get_chan_pair_count() const;
    uint8_t get_chan_pin(uint8_t channel) const;
    pin_pair get_chan_pin_pair(uint8_t channel_pair) const;
    static uint8_t channel_from_pair(uint8_t channel_pair);

    uint32_t get_chan_level(uint8_t channel) const;
    void set_chan_level(uint8_t channel, uint32_t level, bool load = true);

    uint32_t get_chan_offset(uint8_t channel) const;
    void set_chan_offset(uint8_t channel, uint32_t offset, bool load = true);

    bool get_chan_polarity(uint8_t channel) const;
    void set_chan_polarity(uint8_t channel, bool polarity, bool load = true);

    uint32_t get_wrap() const;
    void set_wrap(uint32_t wrap, bool load = true);

    void set_clkdiv(float divider);
    void set_clkdiv_int_frac(uint16_t integer, uint8_t fract);

    void load_pwm();

    //--------------------------------------------------
  public:
    static bool calculate_pwm_factors(float freq, uint32_t& top_out, uint32_t& div256_out);
  private:
    static bool bit_in_mask(uint bit, uint mask);
    static void sorted_insert(TransitionData array[], uint &size, uint capacity, const TransitionData &data);

    // Worst case inserts per load for this cluster's channel count, as for the class limits above
    uint transition_capacity() const { return (channel_count * 3u) + LOADING_ZONE_SIZE; }
    uint loop_transition_capacity() const { return (channel_count * 2u) + LOADING_ZONE_SIZE; }
    void populate_sequence(const TransitionData transitions[], uint data_size, Transition sequence_data[], uint sequence_capacity, uint32_t &sequence_size, uint &pin_states_in_out) const;

    void next_dma_sequence();
  };
}
