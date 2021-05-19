// Include MicroPython API.
#include "py/runtime.h"

/***** Constants *****/

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_bme280_BreakoutBME280_type;

/***** Extern of Class Methods *****/
extern void BreakoutBME280_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t BreakoutBME280_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutBME280_read(mp_obj_t self_in);