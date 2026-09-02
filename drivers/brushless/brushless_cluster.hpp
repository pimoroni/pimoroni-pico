#pragma once

#include "pico/stdlib.h"
#include "pwm_cluster.hpp"
#include "brushless_state.hpp"

using namespace pimoroni;

namespace brushless {

  class BrushlessCluster {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint MAX_BRUSHLESS_MOTORS = PWMCluster::MAX_PWM_CHANNELS / 3;  // half that in the bridge form
    static constexpr float DEFAULT_FREQUENCY = BrushlessState::DEFAULT_FREQUENCY;
    // Shares MotorCluster's ceiling: above roughly 230 kHz the computed pwm top falls below
    // the loading zone position, and the per-period DMA interrupt rate is impractical
    static constexpr float MAX_FREQUENCY = 200000.0f;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    PWMCluster pwms;
    uint32_t pwm_period;
    float pwm_frequency;
    uint8_t channels_per_motor;  // 3, or 6 with inverse channels

    // Both arrays live in one block sized to the motor count, claimed by the constructors
    // through pwm_allocate; init() fails if the claim did
    BrushlessState* states = nullptr;
    float* motor_phases = nullptr;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    BrushlessCluster(PIO pio, uint sm, const pin_trio *pin_trios, uint32_t length,
                     Direction direction = NORMAL_DIR, float freq = DEFAULT_FREQUENCY, bool auto_phase = false);
    BrushlessCluster(PIO pio, uint sm, std::initializer_list<pin_trio> pin_trios,
                     Direction direction = NORMAL_DIR, float freq = DEFAULT_FREQUENCY, bool auto_phase = false);
    BrushlessCluster(PIO pio, uint sm, uint pin_base, uint pin_trio_count,
                     Direction direction = NORMAL_DIR, float freq = DEFAULT_FREQUENCY, bool auto_phase = false);

    BrushlessCluster(PIO pio, uint sm, const bridge_trio *bridge_trios, uint32_t length,
                     Direction direction = NORMAL_DIR, float freq = DEFAULT_FREQUENCY, bool auto_phase = false);
    BrushlessCluster(PIO pio, uint sm, std::initializer_list<bridge_trio> bridge_trios,
                     Direction direction = NORMAL_DIR, float freq = DEFAULT_FREQUENCY, bool auto_phase = false);
    ~BrushlessCluster();


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    bool init();
    bool pins_reachable() const { return pwms.pins_reachable(); }
    bool pins_available() const { return pwms.pins_available(); }

    uint8_t count() const;
    pin_trio pins(uint8_t motor) const;  // the signal pins, either form
    bool has_inverses() const;

    void enable(uint8_t motor, bool load = true);
    void enable(const uint8_t *motors, uint8_t length, bool load = true);
    void enable(std::initializer_list<uint8_t> motors, bool load = true);
    void enable_all(bool load = true);

    void disable(uint8_t motor, bool load = true);
    void disable(const uint8_t *motors, uint8_t length, bool load = true);
    void disable(std::initializer_list<uint8_t> motors, bool load = true);
    void disable_all(bool load = true);

    bool is_enabled(uint8_t motor) const;

    float u_duty(uint8_t motor) const;
    float v_duty(uint8_t motor) const;
    float w_duty(uint8_t motor) const;
    void duties(uint8_t motor, float u, float v, float w, bool load = true);
    void duties(const uint8_t *motors, uint8_t length, float u, float v, float w, bool load = true);
    void duties(std::initializer_list<uint8_t> motors, float u, float v, float w, bool load = true);
    void all_to_duties(float u, float v, float w, bool load = true);

    float phase(uint8_t motor) const;
    void phase(uint8_t motor, float phase, bool load = true);
    void phase(const uint8_t *motors, uint8_t length, float phase, bool load = true);
    void phase(std::initializer_list<uint8_t> motors, float phase, bool load = true);
    void all_to_phase(float phase, bool load = true);

    float frequency() const;
    bool frequency(float freq);

    Direction direction(uint8_t motor) const;
    void direction(uint8_t motor, Direction direction);

    void load();

    //--------------------------------------------------
  private:
    BrushlessCluster(PIO pio, uint sm, const uint8_t *pins, uint32_t pin_count, uint8_t channels_per_motor,
                     Direction direction, float freq, bool auto_phase);

    void apply_duties(uint8_t motor, bool load);
    void apply_phase(uint8_t motor, bool load);
    void create_motor_states(Direction direction, bool auto_phase);
  };

}
