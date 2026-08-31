// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t Motor_type;

/***** Extern of Class Methods *****/
extern void Motor_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t Motor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t Motor___del__(mp_obj_t self_in);
extern mp_obj_t Motor_pins(mp_obj_t self_in);
extern mp_obj_t Motor_enable(mp_obj_t self_in);
extern mp_obj_t Motor_disable(mp_obj_t self_in);
extern mp_obj_t Motor_is_enabled(mp_obj_t self_in);
extern mp_obj_t Motor_duty(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Motor_speed(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Motor_frequency(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Motor_stop(mp_obj_t self_in);
extern mp_obj_t Motor_coast(mp_obj_t self_in);
extern mp_obj_t Motor_brake(mp_obj_t self_in);
extern mp_obj_t Motor_full_negative(mp_obj_t self_in);
extern mp_obj_t Motor_full_positive(mp_obj_t self_in);
extern mp_obj_t Motor_to_percent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Motor_direction(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Motor_speed_scale(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Motor_zeropoint(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Motor_deadzone(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Motor_decay_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

