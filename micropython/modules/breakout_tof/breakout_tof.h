// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_tof_BreakoutTOF_type;

/***** Extern of Class Methods *****/
extern void BreakoutTOF_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t BreakoutTOF_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutTOF_get_id(mp_obj_t self_in);
extern mp_obj_t BreakoutTOF_get_osc(mp_obj_t self_in);
extern mp_obj_t BreakoutTOF_set_osc(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutTOF_set_address(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutTOF_get_address(mp_obj_t self_in);
extern mp_obj_t BreakoutTOF_set_distance_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutTOF_set_distance_mode_int(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutTOF_get_distance_mode(mp_obj_t self_in);
extern mp_obj_t BreakoutTOF_set_measurement_timing_budget(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutTOF_get_measurement_timing_budget(mp_obj_t self_in);
extern mp_obj_t BreakoutTOF_start_continuous(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutTOF_stop_continuous(mp_obj_t self_in);
extern mp_obj_t BreakoutTOF_read(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutTOF_read_range_continuous_millimeters(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutTOF_read_single(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutTOF_read_range_single_millimeters(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutTOF_data_ready(mp_obj_t self_in);

//TODO Below is a static in C, so may want to be global?
extern mp_obj_t BreakoutTOF_range_status_to_string(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

extern mp_obj_t BreakoutTOF_set_timeout(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutTOF_get_timeout(mp_obj_t self_in);
extern mp_obj_t BreakoutTOF_timeout_occurred(mp_obj_t self_in);
extern mp_obj_t BreakoutTOF_get_curr_ms(mp_obj_t self_in);