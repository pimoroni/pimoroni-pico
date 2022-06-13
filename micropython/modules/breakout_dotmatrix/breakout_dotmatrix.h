// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

/***** Constants *****/
static const int WIDTH      = 10;
static const int HEIGHT     = 7;

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_dotmatrix_BreakoutDotMatrix_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutDotMatrix_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutDotMatrix_set_brightness(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutDotMatrix_set_decimal(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutDotMatrix_set_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutDotMatrix_set_character(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutDotMatrix_set_image(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutDotMatrix_show(mp_obj_t self_in);
extern mp_obj_t BreakoutDotMatrix_clear(mp_obj_t self_in);
