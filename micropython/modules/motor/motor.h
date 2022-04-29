// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t Motor_type;
extern const mp_obj_type_t MotorCluster_type;

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

extern void MotorCluster_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t MotorCluster_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t MotorCluster___del__(mp_obj_t self_in);
extern mp_obj_t MotorCluster_count(mp_obj_t self_in);
extern mp_obj_t MotorCluster_pins(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_enable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_enable_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_disable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_disable_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_is_enabled(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_duty(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_all_to_duty(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_speed(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_all_to_speed(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_phase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_all_to_phase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_frequency(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_stop(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_stop_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_coast(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_coast_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_brake(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_brake_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_full_negative(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_all_full_negative(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_full_positive(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_all_full_positive(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_to_percent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_all_to_percent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_load(mp_obj_t self_in);
extern mp_obj_t MotorCluster_direction(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_all_directions(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_speed_scale(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_all_speed_scales(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_zeropoint(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_all_zeropoints(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_deadzone(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_all_deadzones(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_decay_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t MotorCluster_all_decay_modes(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
