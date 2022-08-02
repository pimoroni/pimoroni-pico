// Include MicroPython API.
#include "py/runtime.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t pcf85063a_PCF85063A_type;

/***** Extern of Class Methods *****/
extern mp_obj_t PCF85063A_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t PCF85063A_reset(mp_obj_t self_in);

extern mp_obj_t PCF85063A_datetime(size_t n_args, const mp_obj_t *args);

extern mp_obj_t PCF85063A_set_alarm(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PCF85063A_set_weekday_alarm(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PCF85063A_enable_alarm_interrupt(mp_obj_t self_in, mp_obj_t enable_in);
extern mp_obj_t PCF85063A_clear_alarm_flag(mp_obj_t self_in);
extern mp_obj_t PCF85063A_read_alarm_flag(mp_obj_t self_in);
extern mp_obj_t PCF85063A_unset_alarm(mp_obj_t self_in);

extern mp_obj_t PCF85063A_set_timer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PCF85063A_enable_timer_interrupt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t PCF85063A_clear_timer_flag(mp_obj_t self_in);
extern mp_obj_t PCF85063A_read_timer_flag(mp_obj_t self_in);
extern mp_obj_t PCF85063A_unset_timer(mp_obj_t self_in);

extern mp_obj_t PCF85063A_set_clock_output(mp_obj_t self_in, mp_obj_t co_in);

extern mp_obj_t PCF85063A_set_byte(mp_obj_t self_in, mp_obj_t v);
extern mp_obj_t PCF85063A_get_byte(mp_obj_t self_in);