#pragma once

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include <initializer_list>

namespace pimoroni {

  struct TransitionData {
    uint8_t channel;
    uint32_t level;
    bool state;
    bool dummy;

    TransitionData() : channel(0), level(0), state(false), dummy(false) {};
    TransitionData(uint8_t channel, uint32_t level, bool new_state) : channel(channel), level(level), state(new_state), dummy(false) {};
    TransitionData(uint32_t level) : channel(0), level(level), state(false), dummy(true) {};

    bool compare(const TransitionData& other) const {
      return level <= other.level;
    }
  };

  class PWMCluster {
  public:
    PWMCluster(PIO pio, uint sm, uint pin_mask);
    PWMCluster(PIO pio, uint sm, uint pin_base, uint pin_count);
    PWMCluster(PIO pio, uint sm, std::initializer_list<uint8_t> pins);
    ~PWMCluster();

    bool init();

    uint get_pin_mask() const;
    void set_wrap(uint32_t wrap, bool load = true);
    void set_chan_level(uint8_t channel, uint32_t level, bool load = true);
    void set_chan_offset(uint8_t channel, uint32_t offset, bool load = true);
    void set_chan_polarity(uint8_t channel, bool polarity, bool load = true);
    void set_clkdiv(float divider);
    void set_clkdiv_int_frac(uint16_t integer, uint8_t fract);
    //void set_phase_correct(bool phase_correct);
    //void set_enabled(bool enabled);
    void load_pwm();
    static bool bit_in_mask(uint bit, uint mask);
  private:
    static void sorted_insert(TransitionData array[], uint &size, const TransitionData &data);
  private:
    PIO pio;
    uint sm;
    uint pio_program_offset;
    uint pin_mask;
    uint channel_levels[NUM_BANK0_GPIOS];
    uint channel_offsets[NUM_BANK0_GPIOS];
    uint channel_polarities;
    uint wrap_level;

    uint channel_overruns[NUM_BANK0_GPIOS];

  public:
    static bool calculate_pwm_factors(float freq, uint32_t& top_out, uint16_t& div16_out);
  private:
    static const uint64_t MAX_PWM_CLUSTER_WRAP = UINT16_MAX; // UINT32_MAX works too, but seems to produce less accurate counters
  };
}