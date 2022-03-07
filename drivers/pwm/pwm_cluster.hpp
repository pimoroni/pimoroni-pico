#pragma once

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include <initializer_list>

namespace pimoroni {

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


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
    bool compare(const TransitionData& other) const {
      return level <= other.level;
    }
  };

  class PWMCluster {
    //--------------------------------------------------
    // Substructures
    //--------------------------------------------------
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
    // Constants
    //--------------------------------------------------
  private:
    static const uint64_t MAX_PWM_CLUSTER_WRAP = UINT16_MAX; // UINT32_MAX works too, but seems to produce less accurate counters


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    PIO pio;
    uint sm;
    uint pio_program_offset;
    uint pin_mask;
    uint8_t channel_count;
    ChannelState* channels;
    uint8_t channel_to_pin_map[NUM_BANK0_GPIOS];
    uint wrap_level;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    PWMCluster(PIO pio, uint sm, uint pin_mask);
    PWMCluster(PIO pio, uint sm, uint pin_base, uint pin_count);
    PWMCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins);
    ~PWMCluster();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();

    uint8_t get_chan_count() const;
    uint8_t get_chan_pin(uint8_t channel) const;

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
    static bool calculate_pwm_factors(float freq, uint32_t& top_out, uint16_t& div16_out);
  private:
    static bool bit_in_mask(uint bit, uint mask);
    static void sorted_insert(TransitionData array[], uint &size, const TransitionData &data);
  };
}