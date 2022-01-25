// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

// Declare the functions we'll make available in Python
extern mp_obj_t scd41_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t scd41_start_periodic_measurement();
extern mp_obj_t scd41_stop_periodic_measurement();
extern mp_obj_t scd41_read_measurement();
extern mp_obj_t scd41_get_data_ready();

extern mp_obj_t scd41_set_temperature_offset(mp_obj_t offset);
extern mp_obj_t scd41_get_temperature_offset();
extern mp_obj_t scd41_set_sensor_altitude(mp_obj_t altitude);
extern mp_obj_t scd41_set_ambient_pressure(mp_obj_t pressure);
