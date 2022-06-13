// Include MicroPython API.
#include "py/runtime.h"

/***** Constants *****/
enum {
    ECO2 = 0,
    TVOC,
};

enum {
    H2 = 0,
    ETHANOL,
};

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_sgp30_BreakoutSGP30_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutSGP30_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutSGP30_retrieve_unique_id(mp_obj_t self_in);
extern mp_obj_t BreakoutSGP30_get_unique_id(mp_obj_t self_in);
extern mp_obj_t BreakoutSGP30_start_measurement(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutSGP30_get_air_quality(mp_obj_t self_in);
extern mp_obj_t BreakoutSGP30_get_air_quality_raw(mp_obj_t self_in);
extern mp_obj_t BreakoutSGP30_soft_reset(mp_obj_t self_in);
extern mp_obj_t BreakoutSGP30_get_baseline(mp_obj_t self_in);
extern mp_obj_t BreakoutSGP30_set_baseline(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutSGP30_set_humidity(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);