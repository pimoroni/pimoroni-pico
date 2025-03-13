// Include MicroPython API.
#include "py/runtime.h"
#include "drivers/bme69x/src/bme69x_defs.h"

/***** Constants *****/

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_bme69x_BreakoutBME69X_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutBME69X_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutBME69X_read(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutBME69X_configure(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);