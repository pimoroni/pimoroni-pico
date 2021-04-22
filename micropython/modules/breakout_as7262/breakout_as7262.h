
// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_as7262_BreakoutAS7262_type;

/***** Extern of Class Methods *****/
extern void BreakoutAS7262_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t BreakoutAS7262_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutAS7262_reset(mp_obj_t self_in);

//extern mp_obj_t BreakoutAS7262_device_type(mp_obj_t self_in);       //TODO needs C implementation
//extern mp_obj_t BreakoutAS7262_hardware_version(mp_obj_t self_in);  //TODO needs C implementation
extern mp_obj_t BreakoutAS7262_firmware_version(mp_obj_t self_in);
extern mp_obj_t BreakoutAS7262_read(mp_obj_t self_in);
extern mp_obj_t BreakoutAS7262_temperature(mp_obj_t self_in);

extern mp_obj_t BreakoutAS7262_set_gain(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutAS7262_set_measurement_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutAS7262_set_indicator_current(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutAS7262_set_illumination_current(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutAS7262_set_leds(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);