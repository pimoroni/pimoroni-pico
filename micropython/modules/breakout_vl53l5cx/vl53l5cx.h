#include "py/runtime.h"
#include "drivers/vl53l5cx/src/VL53L5CX_ULD_API/inc/vl53l5cx_api.h"

extern const mp_obj_type_t VL53L5CX_type;


extern mp_obj_t VL53L5CX___del__(mp_obj_t self_in);
extern mp_obj_t VL53L5CX_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

extern mp_obj_t VL53L5CX_start_ranging(mp_obj_t self_in);
extern mp_obj_t VL53L5CX_stop_ranging(mp_obj_t self_in);

extern mp_obj_t VL53L5CX_enable_motion_indicator(mp_obj_t self_in, mp_obj_t value);
extern mp_obj_t VL53L5CX_set_motion_distance(mp_obj_t self_in, mp_obj_t distance_min, mp_obj_t distance_max);

extern mp_obj_t VL53L5CX_set_i2c_address(mp_obj_t self_in, mp_obj_t value);
extern mp_obj_t VL53L5CX_set_ranging_mode(mp_obj_t self_in, mp_obj_t value);
extern mp_obj_t VL53L5CX_set_ranging_frequency_hz(mp_obj_t self_in, mp_obj_t value);
extern mp_obj_t VL53L5CX_set_resolution(mp_obj_t self_in, mp_obj_t value);
extern mp_obj_t VL53L5CX_set_integration_time_ms(mp_obj_t self_in, mp_obj_t value);
extern mp_obj_t VL53L5CX_set_sharpener_percent(mp_obj_t self_in, mp_obj_t value);
extern mp_obj_t VL53L5CX_set_target_order(mp_obj_t self_in, mp_obj_t value);
extern mp_obj_t VL53L5CX_set_power_mode(mp_obj_t self_in, mp_obj_t value);

extern mp_obj_t VL53L5CX_data_ready(mp_obj_t self_in);
extern mp_obj_t VL53L5CX_get_data(mp_obj_t self_in);