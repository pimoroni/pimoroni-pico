// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_encoder_BreakoutEncoder_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutEncoder_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutEncoder_set_address(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutEncoder_get_interrupt_flag(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoder_clear_interrupt_flag(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoder_get_direction(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoder_set_direction(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutEncoder_set_brightness(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutEncoder_set_led(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutEncoder_available(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoder_read(mp_obj_t self_in);
extern mp_obj_t BreakoutEncoder_clear(mp_obj_t self_in);