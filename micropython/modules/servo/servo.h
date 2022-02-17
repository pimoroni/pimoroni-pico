// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t Servo_type;
extern const mp_obj_type_t ServoCluster_type;

/***** Extern of Class Methods *****/
extern void Servo_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t Servo_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t Servo___del__(mp_obj_t self_in);
extern mp_obj_t Servo_enable(mp_obj_t self_in);
extern mp_obj_t Servo_disable(mp_obj_t self_in);
extern mp_obj_t Servo_is_enabled(mp_obj_t self_in);
extern mp_obj_t Servo_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Servo_pulse(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Servo_to_min(mp_obj_t self_in);
extern mp_obj_t Servo_to_mid(mp_obj_t self_in);
extern mp_obj_t Servo_to_max(mp_obj_t self_in);
extern mp_obj_t Servo_to_percent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern void ServoCluster_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t ServoCluster_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t ServoCluster___del__(mp_obj_t self_in);
// extern mp_obj_t ServoCluster_start(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
// extern mp_obj_t ServoCluster_set_rgb(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
// extern mp_obj_t ServoCluster_set_hsv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
// extern mp_obj_t ServoCluster_set_brightness(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
// extern mp_obj_t ServoCluster_get(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
// extern mp_obj_t ServoCluster_clear(mp_obj_t self_in);
// extern mp_obj_t ServoCluster_update(mp_obj_t self_in);

extern bool Pimoroni_mp_obj_to_i2c(mp_obj_t in, void *out);