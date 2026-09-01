#pragma once

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "common/pimoroni_common.hpp"
#include <initializer_list>
#include <cstddef>

// The cluster classes claim and release their blocks through these, so a port can supply
// memory from its own heap. PWMCluster's block is a DMA source, so the memory must be
// somewhere the DMA reads coherently with the CPU's writes, in practice SRAM. The defaults,
// defined weak in pwm_cluster.cpp, take the C heap, which the RP2 ports place in SRAM.
// Returns nullptr on failure; init() reports that to its caller.
extern "C" void* pwm_cluster_allocate(size_t size);
extern "C" void pwm_cluster_deallocate(void* mem);

namespace pimoroni {


  class PWMCluster {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  private:
    static const uint32_t MAX_PWM_CLUSTER_WRAP = UINT16_MAX;  // UINT32_MAX works too, but seems to produce less accurate counters.
                                                              // Wraps, levels and offsets are clamped to this, which is what lets
                                                              // ChannelState and TransitionData store them as uint16_t
    static const uint32_t LOADING_ZONE_SIZE = 3;              // The number of dummy transitions to insert into the data to delay the DMA interrupt (if zero then no zone is used)
    static const uint32_t LOADING_ZONE_POSITION = 55;         // The number of levels before the wrap level to insert the load zone.
                                                              // A level lasts at least PWM_CLUSTER_CYCLES clocks, so this position gives the
                                                              // interrupt a cushion of at least 2.2us on RP2040 (at a clock divider of 1;
                                                              // larger dividers only widen it), against a measured worst case of about 1.1us
                                                              // for entry plus the RAM-resident handler. A too-large position costs little,
                                                              // as the interrupt just fires earlier in the period
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
      uint16_t level;
      uint8_t channel;
      bool state : 1;
      bool dummy : 1;


      //--------------------------------------------------
      // Constructors/Destructor
      //--------------------------------------------------
      TransitionData() : level(0), channel(0), state(false), dummy(false) {};
      TransitionData(uint8_t channel, uint32_t level, bool new_state) : level(level), channel(channel), state(new_state), dummy(false) {};
      TransitionData(uint32_t level) : level(level), channel(0), state(false), dummy(true) {};
    };

  private:
    struct ChannelState {
      //--------------------------------------------------
      // Variables
      //--------------------------------------------------
      uint16_t level;
      uint16_t offset;
      uint16_t overrun;
      uint16_t next_overrun;
      bool polarity;


      //--------------------------------------------------
      // Constructors/Destructor
      //--------------------------------------------------
      ChannelState() : level(0), offset(0), overrun(0), next_overrun(0), polarity(false) {}
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

    // First GPIO the PIO instance's 32-pin window can reach, set by init() from the pin set.
    // The map above stays absolute; the transition masks subtract this
    uint gpio_base = 0;

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
    static uint32_t claimed_channel_mask;  // A bit per DMA channel with a cluster attached
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
    static constexpr bool bit_in_mask(uint bit, uint64_t mask) {
      return ((1llu << bit) & mask) != 0;
    }
    static void sorted_insert(TransitionData array[], uint &size, uint capacity, const TransitionData &data);

    // Worst case inserts per load for this cluster's channel count, as for the class limits above
    uint transition_capacity() const { return (channel_count * 3u) + LOADING_ZONE_SIZE; }
    uint loop_transition_capacity() const { return (channel_count * 2u) + LOADING_ZONE_SIZE; }
    void populate_sequence(const TransitionData transitions[], uint data_size, Transition sequence_data[], uint sequence_capacity, uint32_t &sequence_size, uint &pin_states_in_out) const;

    void next_dma_sequence();
  };
}
