// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

/***** Constants *****/

//Intentionally does not match numbering used in MSA class
enum {
    MSA_AXIS_X = 0,
    MSA_AXIS_Y,
    MSA_AXIS_Z
};

enum {
    MSA_PORTRAIT            = 0b00,
    MSA_PORTRAIT_INVERTED   = 0b01,
    MSA_LANDSCAPE           = 0b10,
    MSA_LANDSCAPE_INVERTED  = 0b11
};

enum {
    MSA_NORMAL  = 0b00,
    MSA_LOW     = 0b01,
    MSA_SUSPEND = 0b10
};

enum {
    MSA_G_2   = 0b00,
    MSA_G_4   = 0b01,
    MSA_G_8   = 0b10,
    MSA_G_16  = 0b11
};

enum {
    MSA_BITS_14   = 0b0000,
    MSA_BITS_12   = 0b0100,
    MSA_BITS_10   = 0b1000,
    MSA_BITS_8    = 0b1100
};

enum {
    MSA_INVERT_X  = 0b1000,
    MSA_INVERT_Y  = 0b0100,
    MSA_INVERT_Z  = 0b0010,
    MSA_XY_SWAP   = 0b0001
};

enum {
    MSA_NONE          = 0,
    MSA_ACTIVE        = 0b0000111,
    MSA_NEW_DATA      = 0b1000000000000,
    MSA_FREEFALL      = 0b0100000000000,
    MSA_ORIENTATION   = 0b1000000,
    MSA_SINGLE_TAP    = 0b0100000,
    MSA_DOUBLE_TAP    = 0b0010000,
    MSA_Z_ACTIVE      = 0b0000100,
    MSA_Y_ACTIVE      = 0b0000010,
    MSA_X_ACTIVE      = 0b0000001
};

//Intentionally does not match numbering used in MSA class
enum {
    MSA_LATCH_1MS     = 0,
    MSA_LATCH_2MS,
    MSA_LATCH_25MS,
    MSA_LATCH_50MS,
    MSA_LATCH_100MS,
    MSA_LATCH_250MS,
    MSA_LATCH_500MS,
    MSA_LATCH_1S,
    MSA_LATCH_2S,
    MSA_LATCH_4S,
    MSA_LATCH_8S,
};

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_msa301_BreakoutMSA301_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutMSA301_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutMSA301_part_id(mp_obj_t self_in);
extern mp_obj_t BreakoutMSA301_get_axis(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMSA301_get_x_axis(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMSA301_get_y_axis(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMSA301_get_z_axis(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMSA301_get_orientation(mp_obj_t self_in);
extern mp_obj_t BreakoutMSA301_set_power_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMSA301_set_range_and_resolution(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMSA301_set_axis_polarity(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMSA301_disable_all_interrupts(mp_obj_t self_in);
extern mp_obj_t BreakoutMSA301_enable_interrupts(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMSA301_set_interrupt_latch(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutMSA301_read_interrupt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);