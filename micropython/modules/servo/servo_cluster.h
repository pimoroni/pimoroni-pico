// Include MicroPython API.
#include "py/runtime.h"
#include "servo.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t ServoCluster_type;

/***** Extern of Class Methods *****/
extern void ServoCluster_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t ServoCluster_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t ServoCluster___del__(mp_obj_t self_in);
extern mp_obj_t ServoCluster_count(mp_obj_t self_in);
extern mp_obj_t ServoCluster_pin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_enable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_enable_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_disable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_disable_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_is_enabled(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_pulse(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_all_to_pulse(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_all_to_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_phase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_all_to_phase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_frequency(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_min_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_mid_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_max_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_to_min(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_all_to_min(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_to_mid(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_all_to_mid(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_to_max(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_all_to_max(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_to_percent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_all_to_percent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t ServoCluster_load(mp_obj_t self_in);