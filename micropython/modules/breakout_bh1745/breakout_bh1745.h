// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

/***** Constants *****/
enum {
    RGBC_R,
    RGBC_G,
    RGBC_B,
    RGBC_C
};

static const uint8_t BH1745_I2C_ADDRESS_DEFAULT = 0x38;
static const uint8_t BH1745_I2C_ADDRESS_ALTERNATE = 0x39;

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_bh1745_BreakoutBH1745_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutBH1745_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutBH1745_chip_id(mp_obj_t self_in);
extern mp_obj_t BreakoutBH1745_manufacturer_id(mp_obj_t self_in);
extern mp_obj_t BreakoutBH1745_threshold(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutBH1745_measurement_time_ms(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutBH1745_rgbc_raw(mp_obj_t self_in);
extern mp_obj_t BreakoutBH1745_rgbc_clamped(mp_obj_t self_in);
extern mp_obj_t BreakoutBH1745_rgbc_scaled(mp_obj_t self_in);
extern mp_obj_t BreakoutBH1745_leds(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
