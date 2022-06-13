// Include MicroPython API.
#include "py/runtime.h"

enum {
    REF = 0,
    REDUCING,
    NH3,
    OXIDISING
};

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_mics6814_BreakoutMICS6814_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutMICS6814_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutMICS6814_set_address(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMICS6814_get_adc_vref(mp_obj_t self_in);
extern mp_obj_t BreakoutMICS6814_set_adc_vref(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMICS6814_set_brightness(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMICS6814_set_led(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMICS6814_set_heater(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMICS6814_disable_heater(mp_obj_t self_in);
extern mp_obj_t BreakoutMICS6814_get_raw_ref(mp_obj_t self_in);
extern mp_obj_t BreakoutMICS6814_get_raw_red(mp_obj_t self_in);
extern mp_obj_t BreakoutMICS6814_get_raw_nh3(mp_obj_t self_in);
extern mp_obj_t BreakoutMICS6814_get_raw_oxd(mp_obj_t self_in);
extern mp_obj_t BreakoutMICS6814_read_all(mp_obj_t self_in);
extern mp_obj_t BreakoutMICS6814_read_ref(mp_obj_t self_in);
extern mp_obj_t BreakoutMICS6814_read_reducing(mp_obj_t self_in);
extern mp_obj_t BreakoutMICS6814_read_nh3(mp_obj_t self_in);
extern mp_obj_t BreakoutMICS6814_read_oxidising(mp_obj_t self_in);