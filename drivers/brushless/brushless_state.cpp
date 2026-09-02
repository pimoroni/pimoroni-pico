#include "brushless_state.hpp"

namespace brushless {
  BrushlessState::BrushlessState()
    : leg_duties{NEUTRAL_POINT, NEUTRAL_POINT, NEUTRAL_POINT}
    , motor_direction(NORMAL_DIR)
    , enabled(false) {
  }

  BrushlessState::BrushlessState(Direction direction)
    : BrushlessState() {
    motor_direction = direction;
  }

  void BrushlessState::enable() {
    enabled = true;
  }

  void BrushlessState::disable() {
    enabled = false;
  }

  bool BrushlessState::is_enabled() const {
    return enabled;
  }

  float BrushlessState::get_duty(uint8_t leg) const {
    assert(leg < NUM_LEGS);
    return leg_duties[leg];
  }

  void BrushlessState::set_duties(float u, float v, float w) {
    leg_duties[0] = MIN(MAX(u, 0.0f), 1.0f);
    leg_duties[1] = MIN(MAX(v, 0.0f), 1.0f);
    leg_duties[2] = MIN(MAX(w, 0.0f), 1.0f);
  }

  float BrushlessState::applied_duty(uint8_t leg) const {
    assert(leg < NUM_LEGS);
    if(!enabled) {
      return 0.0f;
    }
    if(motor_direction == REVERSED_DIR && leg > 0) {
      leg = NUM_LEGS - leg;  // swap V and W
    }
    return leg_duties[leg];
  }

  Direction BrushlessState::get_direction() const {
    return motor_direction;
  }

  void BrushlessState::set_direction(Direction direction) {
    motor_direction = direction;
  }

  uint32_t BrushlessState::duty_to_level(float duty, uint32_t resolution) {
    return (uint32_t)(duty * (float)resolution);
  }
}
