// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_encoder_wheel_BreakoutEncoderWheel_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutEncoderWheel_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutEncoderWheel_set_ioe_address(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutEncoderWheel_get_interrupt_flag(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoderWheel_clear_interrupt_flag(mp_obj_t self_in);

extern mp_obj_t BreakoutEncoderWheel_pressed(mp_obj_t self_in, mp_obj_t button_in);
extern mp_obj_t BreakoutEncoderWheel_count(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoderWheel_delta(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoderWheel_step(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoderWheel_turn(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoderWheel_zero(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoderWheel_revolutions(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoderWheel_degrees(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoderWheel_radians(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoderWheel_direction(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t BreakoutEncoderWheel_set_rgb(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutEncoderWheel_set_hsv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutEncoderWheel_clear(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoderWheel_show(mp_obj_t self_in);

extern mp_obj_t BreakoutEncoderWheel_gpio_pin_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutEncoderWheel_gpio_pin_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutEncoderWheel_gpio_pwm_load(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutEncoderWheel_gpio_pwm_frequency(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
