// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_paa5100_BreakoutPAA5100_type;

/***** Extern of Class Methods *****/
extern void BreakoutPAA5100_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t BreakoutPAA5100_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutPAA5100___del__(mp_obj_t self_in);
extern mp_obj_t BreakoutPAA5100_get_id(mp_obj_t self_in);
extern mp_obj_t BreakoutPAA5100_get_revision(mp_obj_t self_in);
extern mp_obj_t BreakoutPAA5100_set_rotation(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutPAA5100_set_orientation(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutPAA5100_get_motion(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutPAA5100_get_motion_slow(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutPAA5100_frame_capture(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);