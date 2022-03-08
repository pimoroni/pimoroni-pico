// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t Calibration_type;
extern const mp_obj_type_t Servo_type;
extern const mp_obj_type_t ServoCluster_type;

/***** Extern of Class Methods *****/
extern void Calibration_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t Calibration_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t Calibration___del__(mp_obj_t self_in);
extern mp_obj_t Calibration_create_blank_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Calibration_create_two_point_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Calibration_create_three_point_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Calibration_create_uniform_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Calibration_create_default_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Calibration_size(mp_obj_t self_in);
extern mp_obj_t Calibration_point_at(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Calibration_first_point(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Calibration_last_point(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Calibration_has_lower_limit(mp_obj_t self_in);
extern mp_obj_t Calibration_has_upper_limit(mp_obj_t self_in);
extern mp_obj_t Calibration_limit_to_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Calibration_value_to_pulse(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Calibration_pulse_to_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern void Servo_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t Servo_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t Servo___del__(mp_obj_t self_in);
extern mp_obj_t Servo_pin(mp_obj_t self_in);
extern mp_obj_t Servo_enable(mp_obj_t self_in);
extern mp_obj_t Servo_disable(mp_obj_t self_in);
extern mp_obj_t Servo_is_enabled(mp_obj_t self_in);
extern mp_obj_t Servo_pulse(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Servo_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Servo_frequency(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Servo_min_value(mp_obj_t self_in);
extern mp_obj_t Servo_mid_value(mp_obj_t self_in);
extern mp_obj_t Servo_max_value(mp_obj_t self_in);
extern mp_obj_t Servo_to_min(mp_obj_t self_in);
extern mp_obj_t Servo_to_mid(mp_obj_t self_in);
extern mp_obj_t Servo_to_max(mp_obj_t self_in);
extern mp_obj_t Servo_to_percent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Servo_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern void ServoCluster_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t ServoCluster_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t ServoCluster___del__(mp_obj_t self_in);
extern mp_obj_t ServoCluster_count(mp_obj_t self_in);
extern mp_obj_t ServoCluster_pin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_enable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_disable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_is_enabled(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_pulse(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_phase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_frequency(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_min_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_mid_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_max_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_to_min(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_to_mid(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_to_max(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_to_percent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_load(mp_obj_t self_in);