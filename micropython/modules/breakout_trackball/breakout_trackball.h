// Include MicroPython API.
#include "py/runtime.h"

/***** Constants *****/
enum {
    LEFT = 0,
    RIGHT,
    UP,
    DOWN,
    SW_CHANGED,
    SW_PRESSED
};

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_trackball_BreakoutTrackball_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutTrackball_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t  BreakoutTrackball_change_address(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutTrackball_enable_interrupt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutTrackball_get_interrupt(mp_obj_t self_in);
extern mp_obj_t  BreakoutTrackball_set_rgbw(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutTrackball_set_red(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutTrackball_set_green(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutTrackball_set_blue(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutTrackball_set_white(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t  BreakoutTrackball_read(mp_obj_t self_in);