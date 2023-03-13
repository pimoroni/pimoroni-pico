#pragma once

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "common/pimoroni_common.hpp"
#include <initializer_list>

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
    static const uint BUFFER_SIZE = 64;     // Set to 64, the maximum number of single rises and falls for 32 channels within a looping time period
    static const uint NUM_BUFFERS = 3;
    static const uint MAX_PWM_CHANNELS = 32;


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
      Transition data[BUFFER_SIZE];


      //--------------------------------------------------
      // Constructors/Destructor
      //--------------------------------------------------
      Sequence() : size(1), data{Transition()} {};
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
    uint pin_mask;
    uint8_t channel_count;
    ChannelState channels[NUM_BANK0_GPIOS];
    uint8_t channel_to_pin_map[NUM_BANK0_GPIOS];
    uint wrap_level;

    Sequence sequences[NUM_BUFFERS];
    Sequence loop_sequences[NUM_BUFFERS];

    TransitionData transitions[BUFFER_SIZE];
    TransitionData looping_transitions[BUFFER_SIZE];

    volatile uint read_index = 0;
    volatile uint last_written_index = 0;

    bool initialised = false;
    bool loading_zone = true;


    //--------------------------------------------------
    // Statics
    //--------------------------------------------------
    static PWMCluster* clusters[NUM_DMA_CHANNELS];
    static uint8_t claimed_sms[NUM_PIOS];
    static uint pio_program_offset;
    static void dma_interrupt_handler();


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    PWMCluster(PIO pio, uint sm, uint pin_mask, bool loading_zone = DEFAULT_USE_LOADING_ZONE);
    PWMCluster(PIO pio, uint sm, uint pin_base, uint pin_count, bool loading_zone = DEFAULT_USE_LOADING_ZONE);
    PWMCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t length, bool loading_zone = DEFAULT_USE_LOADING_ZONE);
    PWMCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins, bool loading_zone = DEFAULT_USE_LOADING_ZONE);

    PWMCluster(PIO pio, uint sm, const pin_pair *pin_pairs, uint32_t length, bool loading_zone = DEFAULT_USE_LOADING_ZONE);
    PWMCluster(PIO pio, uint sm, std::initializer_list<pin_pair> pin_pairs, bool loading_zone = DEFAULT_USE_LOADING_ZONE);
    ~PWMCluster();

  private:
    void constructor_common();


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
    static void sorted_insert(TransitionData array[], uint &size, const TransitionData &data);
    void populate_sequence(const TransitionData transitions[], const uint &data_size, Sequence &seq_out, uint &pin_states_in_out) const;

    void next_dma_sequence();
  };
}