// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t Encoder_type;

/***** Extern of Class Methods *****/
extern void Encoder_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t Encoder_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t Encoder___del__(mp_obj_t self_in);
extern mp_obj_t Encoder_pins(mp_obj_t self_in);
extern mp_obj_t Encoder_common_pin(mp_obj_t self_in);
extern mp_obj_t Encoder_state(mp_obj_t self_in);
extern mp_obj_t Encoder_count(mp_obj_t self_in);
extern mp_obj_t Encoder_delta(mp_obj_t self_in);
extern mp_obj_t Encoder_zero(mp_obj_t self_in);
extern mp_obj_t Encoder_step(mp_obj_t self_in);
extern mp_obj_t Encoder_turn(mp_obj_t self_in);
extern mp_obj_t Encoder_revolutions(mp_obj_t self_in);
extern mp_obj_t Encoder_degrees(mp_obj_t self_in);
extern mp_obj_t Encoder_radians(mp_obj_t self_in);
extern mp_obj_t Encoder_direction(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Encoder_counts_per_rev(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Encoder_capture(mp_obj_t self_in);