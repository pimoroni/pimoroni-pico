// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_potentiometer_BreakoutPotentiometer_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutPotentiometer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutPotentiometer_set_address(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutPotentiometer_get_adc_vref(mp_obj_t self_in);
extern mp_obj_t BreakoutPotentiometer_set_adc_vref(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutPotentiometer_get_direction(mp_obj_t self_in);
extern mp_obj_t BreakoutPotentiometer_set_direction(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutPotentiometer_set_brightness(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutPotentiometer_set_led(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutPotentiometer_read(mp_obj_t self_in);
extern mp_obj_t BreakoutPotentiometer_read_raw(mp_obj_t self_in);