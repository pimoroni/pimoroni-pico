#include "py/runtime.h"
#include "py/objstr.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t BrushlessCluster_type;

/***** Extern of Class Methods *****/
extern void BrushlessCluster_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t BrushlessCluster_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BrushlessCluster___del__(mp_obj_t self_in);
extern mp_obj_t BrushlessCluster_count(mp_obj_t self_in);
extern mp_obj_t BrushlessCluster_pins(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_has_inverses(mp_obj_t self_in);
extern mp_obj_t BrushlessCluster_enable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_enable_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_disable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_disable_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_is_enabled(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_u_duty(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_v_duty(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_w_duty(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_duties(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_all_to_duties(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_phase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_all_to_phase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_frequency(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_direction(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BrushlessCluster_load(mp_obj_t self_in);
