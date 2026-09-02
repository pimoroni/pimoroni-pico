#pragma once

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "common/pimoroni_common.hpp"
#include "brushless_state.hpp"

using namespace pimoroni;

namespace brushless {

  class Brushless {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static constexpr float DEFAULT_FREQUENCY = BrushlessState::DEFAULT_FREQUENCY;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    pin_trio signal_pins;
    pin_trio inverse_pins;
    bool inverse_pins_used;
    uint32_t pwm_period;
    float pwm_frequency;
    BrushlessState state;

    // The first two pins found sharing a slice channel, PIN_UNUSED when none.
    // Such pins would emit the same signal, so init() refuses them
    uint conflict_pin_a = PIN_UNUSED;
    uint conflict_pin_b = PIN_UNUSED;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Brushless(const pin_trio &pins, Direction direction = NORMAL_DIR, float freq = DEFAULT_FREQUENCY);
    Brushless(const bridge_trio &bridges, Direction direction = NORMAL_DIR, float freq = DEFAULT_FREQUENCY);
    ~Brushless();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();
    bool pin_conflict(uint &pin_a, uint &pin_b) const;  // why init() refused, when it did

    pin_trio pins() const;  // the signal pins, either form
    bool has_inverses() const;

    void enable();
    void disable();
    bool is_enabled() const;

    float u_duty() const;
    float v_duty() const;
    float w_duty() const;
    void duties(float u, float v, float w);

    float frequency() const;
    bool frequency(float freq);

    Direction direction() const;
    void direction(Direction direction);

    //--------------------------------------------------
  private:
    uint8_t owned_pins(uint8_t pins_out[6]) const;
    void find_conflicts();
    uint32_t slice_mask() const;
    bool configure_slices(float freq);
    void apply_duties();
  };

}
