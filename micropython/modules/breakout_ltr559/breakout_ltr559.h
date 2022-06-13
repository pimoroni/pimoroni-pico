// Include MicroPython API.
#include "py/runtime.h"
#include "py/objstr.h"

/***** Constants *****/
enum {
    PROXIMITY = 0,
    ALS_0,
    ALS_1,
    INTEGRATION_TIME,
    GAIN,
    RATIO,
    LUX
};

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_ltr559_BreakoutLTR559_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutLTR559_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutLTR559_part_id(mp_obj_t self_in);
extern mp_obj_t BreakoutLTR559_revision_id(mp_obj_t self_in);
extern mp_obj_t BreakoutLTR559_manufacturer_id(mp_obj_t self_in);
extern mp_obj_t BreakoutLTR559_get_reading(mp_obj_t self_in);
extern mp_obj_t BreakoutLTR559_interrupts(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutLTR559_proximity_led(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutLTR559_light_control(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutLTR559_proximity_control(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutLTR559_light_threshold(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutLTR559_proximity_threshold(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutLTR559_light_measurement_rate(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutLTR559_proximity_measurement_rate(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutLTR559_proximity_offset(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);