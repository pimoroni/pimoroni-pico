// Include MicroPython API.
#include "py/runtime.h"

/***** Constants *****/
static const int WIDTH      = 5;
static const int HEIGHT     = 5;

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_rgbmatrix5x5_BreakoutRGBMatrix5x5_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutRGBMatrix5x5_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutRGBMatrix5x5_set_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutRGBMatrix5x5_update(mp_obj_t self_in);
extern mp_obj_t BreakoutRGBMatrix5x5_clear(mp_obj_t self_in);