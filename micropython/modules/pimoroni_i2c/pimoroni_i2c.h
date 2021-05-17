// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t PimoroniI2C_type;

/***** Extern of Class Methods *****/
extern void PimoroniI2C_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t PimoroniI2C_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

extern bool Pimoroni_mp_obj_to_i2c(mp_obj_t in, void *out);