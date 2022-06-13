// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_icp10125_BreakoutICP10125_type;

enum meas_command {
    NORMAL = 0x6825,
    LOW_POWER = 0x609C,
    LOW_NOISE = 0x70DF,
    ULTRA_LOW_NOISE = 0x7866,
};

enum reading_status {
    OK = 0,
    CRC_FAIL = 1,
};

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutICP10125_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutICP10125_soft_reset(mp_obj_t self_in);
extern mp_obj_t BreakoutICP10125_measure(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);