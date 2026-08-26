// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_scd41_BreakoutSCD41_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutSCD41_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

extern mp_obj_t BreakoutSCD41_start_periodic_measurement(mp_obj_t self_in);
extern mp_obj_t BreakoutSCD41_start_low_power_periodic_measurement(mp_obj_t self_in);
extern mp_obj_t BreakoutSCD41_stop_periodic_measurement(mp_obj_t self_in);
extern mp_obj_t BreakoutSCD41_read_measurement(mp_obj_t self_in);
extern mp_obj_t BreakoutSCD41_get_data_ready(mp_obj_t self_in);

extern mp_obj_t BreakoutSCD41_set_temperature_offset(mp_obj_t self_in, mp_obj_t offset);
extern mp_obj_t BreakoutSCD41_get_temperature_offset(mp_obj_t self_in);
extern mp_obj_t BreakoutSCD41_set_sensor_altitude(mp_obj_t self_in, mp_obj_t altitude);
extern mp_obj_t BreakoutSCD41_set_ambient_pressure(mp_obj_t self_in, mp_obj_t pressure);

extern mp_obj_t BreakoutSCD41_perform_forced_recalibration(mp_obj_t self_in, mp_obj_t target_co2_concentration_in);
extern mp_obj_t BreakoutSCD41_set_automatic_self_calibration(mp_obj_t self_in, mp_obj_t asc_enabled);
extern mp_obj_t BreakoutSCD41_get_automatic_self_calibration(mp_obj_t self_in);
