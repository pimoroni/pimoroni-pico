// Include MicroPython API.
#include "py/runtime.h"

/***** Constants *****/
enum axes {
    AXIS_X = 0,
    AXIS_Y,
    AXIS_Z
};

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_3dof_Breakout3DOF_type;

/***** Extern of Class Methods *****/
extern void Breakout3DOF_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t Breakout3DOF_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t Breakout3DOF_get_axis(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);