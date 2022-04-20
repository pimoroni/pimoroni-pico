// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t Snapshot_type;
extern const mp_obj_type_t Encoder_type;

/***** Extern of Class Methods *****/
extern void Snapshot_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t Snapshot_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t Snapshot___del__(mp_obj_t self_in);
extern mp_obj_t Snapshot_count(mp_obj_t self_in);
extern mp_obj_t Snapshot_delta(mp_obj_t self_in);
extern mp_obj_t Snapshot_frequency(mp_obj_t self_in);
extern mp_obj_t Snapshot_revolutions(mp_obj_t self_in);
extern mp_obj_t Snapshot_degrees(mp_obj_t self_in);
extern mp_obj_t Snapshot_radians(mp_obj_t self_in);
extern mp_obj_t Snapshot_revolutions_delta(mp_obj_t self_in);
extern mp_obj_t Snapshot_degrees_delta(mp_obj_t self_in);
extern mp_obj_t Snapshot_radians_delta(mp_obj_t self_in);
extern mp_obj_t Snapshot_revolutions_per_second(mp_obj_t self_in);
extern mp_obj_t Snapshot_revolutions_per_minute(mp_obj_t self_in);
extern mp_obj_t Snapshot_degrees_per_second(mp_obj_t self_in);
extern mp_obj_t Snapshot_radians_per_second(mp_obj_t self_in);

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
extern mp_obj_t Encoder_counts_per_revolution(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t Encoder_take_snapshot(mp_obj_t self_in);
extern mp_obj_t Encoder_capture(mp_obj_t self_in);