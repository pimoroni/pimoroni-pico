// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t PlasmaAPA102_type;
extern const mp_obj_type_t PlasmaWS2812_type;

/***** Extern of Class Methods *****/
extern void PlasmaAPA102_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t PlasmaAPA102_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t PlasmaAPA102___del__(mp_obj_t self_in);
extern mp_obj_t PlasmaAPA102_start(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PlasmaAPA102_set_rgb(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PlasmaAPA102_set_hsv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PlasmaAPA102_set_brightness(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PlasmaAPA102_get(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PlasmaAPA102_clear(mp_obj_t self_in);
extern mp_obj_t PlasmaAPA102_update(mp_obj_t self_in);

extern void PlasmaWS2812_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t PlasmaWS2812_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t PlasmaWS2812___del__(mp_obj_t self_in);
extern mp_obj_t PlasmaWS2812_start(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PlasmaWS2812_set_rgb(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PlasmaWS2812_set_hsv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PlasmaWS2812_get(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PlasmaWS2812_clear(mp_obj_t self_in);
extern mp_obj_t PlasmaWS2812_update(mp_obj_t self_in);

extern bool Pimoroni_mp_obj_to_i2c(mp_obj_t in, void *out);