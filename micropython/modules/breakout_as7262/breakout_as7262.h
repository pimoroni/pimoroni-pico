// Include MicroPython API.
#include "py/runtime.h"

/***** Constants *****/
enum {
    MP_X1   = 0b00,
    MP_X3_7 = 0b01,
    MP_X16  = 0b10,
    MP_X64  = 0b11
};

enum {
    MP_MA12  = 0b00,
    MP_MA25  = 0b01,
    MP_MA50  = 0b10,
    MP_MA100 = 0b11
};

enum {
    MP_MA1 = 0b00,
    MP_MA2 = 0b01,
    MP_MA4 = 0b10,
    MP_MA8 = 0b11
};

enum {
    MP_CONT_YGNV    = 0b00, // yellow, green, blue, violet - continuous
    MP_CONT_ROYG    = 0b01, // red, orange, yellow, green - continuous
    MP_CONT_ROYGBR  = 0b10, // red, orange, yellow, green, violet - continuous
    MP_ONESHOT      = 0b11  // everything - one-shot
};

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_as7262_BreakoutAS7262_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutAS7262_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutAS7262_reset(mp_obj_t self_in);
extern mp_obj_t BreakoutAS7262_device_type(mp_obj_t self_in);
extern mp_obj_t BreakoutAS7262_hardware_version(mp_obj_t self_in);
extern mp_obj_t BreakoutAS7262_firmware_version(mp_obj_t self_in);
extern mp_obj_t BreakoutAS7262_read(mp_obj_t self_in);
extern mp_obj_t BreakoutAS7262_temperature(mp_obj_t self_in);
extern mp_obj_t BreakoutAS7262_set_gain(mp_obj_t self_in, mp_obj_t value_in);
extern mp_obj_t BreakoutAS7262_set_measurement_mode(mp_obj_t self_in, mp_obj_t value_in);
extern mp_obj_t BreakoutAS7262_set_indicator_current(mp_obj_t self_in, mp_obj_t value_in);
extern mp_obj_t BreakoutAS7262_set_illumination_current(mp_obj_t self_in, mp_obj_t value_in);
extern mp_obj_t BreakoutAS7262_set_integration_time(mp_obj_t self_in, mp_obj_t value_in);
extern mp_obj_t BreakoutAS7262_set_leds(mp_obj_t self_in, mp_obj_t illumination, mp_obj_t indicator);