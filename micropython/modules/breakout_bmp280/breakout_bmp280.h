// Include MicroPython API.
#include "py/runtime.h"
#include "drivers/bmp280/src/bmp280_defs.h"

/***** Constants *****/

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_bmp280_BreakoutBMP280_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutBMP280_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutBMP280_read(mp_obj_t self_in);
extern mp_obj_t BreakoutBMP280_configure(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);