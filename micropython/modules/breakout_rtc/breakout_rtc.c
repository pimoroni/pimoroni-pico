#include "breakout_rtc.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutRTC Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_reset_obj, BreakoutRTC_reset);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_setup_obj, BreakoutRTC_setup);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRTC_set_time_obj, 8, BreakoutRTC_set_time);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_seconds_obj, BreakoutRTC_set_seconds);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_minutes_obj, BreakoutRTC_set_minutes);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_hours_obj, BreakoutRTC_set_hours);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_weekday_obj, BreakoutRTC_set_weekday);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_date_obj, BreakoutRTC_set_date);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_month_obj, BreakoutRTC_set_month);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_year_obj, BreakoutRTC_set_year);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_set_to_compiler_time_obj, BreakoutRTC_set_to_compiler_time);

MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_update_time_obj, BreakoutRTC_update_time);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_string_date_usa_obj, BreakoutRTC_string_date_usa);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_string_date_obj, BreakoutRTC_string_date);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_string_time_obj, BreakoutRTC_string_time);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_string_time_stamp_obj, BreakoutRTC_string_time_stamp);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_seconds_obj, BreakoutRTC_get_seconds);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_minutes_obj, BreakoutRTC_get_minutes);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_hours_obj, BreakoutRTC_get_hours);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_weekday_obj, BreakoutRTC_get_weekday);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_date_obj, BreakoutRTC_get_date);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_month_obj, BreakoutRTC_get_month);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_year_obj, BreakoutRTC_get_year);

MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_is_12_hour_obj, BreakoutRTC_is_12_hour);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_is_pm_obj, BreakoutRTC_is_pm);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_set_12_hour_obj, BreakoutRTC_set_12_hour);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_set_24_hour_obj, BreakoutRTC_set_24_hour);

MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_unix_obj, BreakoutRTC_set_unix);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_unix_obj, BreakoutRTC_get_unix);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRTC_enable_alarm_interrupt_obj, 6, BreakoutRTC_enable_alarm_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_disable_alarm_interrupt_obj, BreakoutRTC_disable_alarm_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_read_alarm_interrupt_flag_obj, BreakoutRTC_read_alarm_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_clear_alarm_interrupt_flag_obj, BreakoutRTC_clear_alarm_interrupt_flag);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRTC_set_timer_obj, 6, BreakoutRTC_set_timer);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_get_timer_count_obj, BreakoutRTC_get_timer_count);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_enable_timer_obj, BreakoutRTC_enable_timer);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_disable_timer_obj, BreakoutRTC_disable_timer);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_enable_timer_interupt_obj, BreakoutRTC_enable_timer_interupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_disable_timer_interrupt_obj, BreakoutRTC_disable_timer_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_read_timer_interrupt_flag_obj, BreakoutRTC_read_timer_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_clear_timer_interrupt_flag_obj, BreakoutRTC_clear_timer_interrupt_flag);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRTC_enable_periodic_update_interrupt_obj, 2, BreakoutRTC_enable_periodic_update_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_disable_periodic_update_interrupt_obj, BreakoutRTC_disable_periodic_update_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_read_periodic_update_interrupt_flag_obj, BreakoutRTC_read_periodic_update_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_clear_periodic_update_interrupt_flag_obj, BreakoutRTC_clear_periodic_update_interrupt_flag);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRTC_enable_trickle_charge_obj, 2, BreakoutRTC_enable_trickle_charge);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_disable_trickle_charge_obj, BreakoutRTC_disable_trickle_charge);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_set_backup_switchover_mode_obj, BreakoutRTC_set_backup_switchover_mode);

MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_enable_clock_out_obj, BreakoutRTC_enable_clock_out);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRTC_enable_interrupt_controlled_clockout_obj, BreakoutRTC_enable_interrupt_controlled_clockout);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_disable_clock_out_obj, BreakoutRTC_disable_clock_out);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_read_clock_output_interrupt_flag_obj, BreakoutRTC_read_clock_output_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_clear_clock_output_interrupt_flag_obj, BreakoutRTC_clear_clock_output_interrupt_flag);

MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_status_obj, BreakoutRTC_status);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRTC_clear_interrupts_obj, BreakoutRTC_clear_interrupts);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutRTC_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&BreakoutRTC_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_setup), MP_ROM_PTR(&BreakoutRTC_setup_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_time), MP_ROM_PTR(&BreakoutRTC_set_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_seconds), MP_ROM_PTR(&BreakoutRTC_set_seconds_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_minutes), MP_ROM_PTR(&BreakoutRTC_set_minutes_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_hours), MP_ROM_PTR(&BreakoutRTC_set_hours_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_weekday), MP_ROM_PTR(&BreakoutRTC_set_weekday_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_date), MP_ROM_PTR(&BreakoutRTC_set_date_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_month), MP_ROM_PTR(&BreakoutRTC_set_month_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_year), MP_ROM_PTR(&BreakoutRTC_set_year_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_to_compiler_time), MP_ROM_PTR(&BreakoutRTC_set_to_compiler_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_update_time), MP_ROM_PTR(&BreakoutRTC_update_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_string_date_usa), MP_ROM_PTR(&BreakoutRTC_string_date_usa_obj) },
    { MP_ROM_QSTR(MP_QSTR_string_date), MP_ROM_PTR(&BreakoutRTC_string_date_obj) },
    { MP_ROM_QSTR(MP_QSTR_string_time), MP_ROM_PTR(&BreakoutRTC_string_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_string_time_stamp), MP_ROM_PTR(&BreakoutRTC_string_time_stamp_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_seconds), MP_ROM_PTR(&BreakoutRTC_get_seconds_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_minutes), MP_ROM_PTR(&BreakoutRTC_get_minutes_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_hours), MP_ROM_PTR(&BreakoutRTC_get_hours_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_weekday), MP_ROM_PTR(&BreakoutRTC_get_weekday_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_date), MP_ROM_PTR(&BreakoutRTC_get_date_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_month), MP_ROM_PTR(&BreakoutRTC_get_month_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_year), MP_ROM_PTR(&BreakoutRTC_get_year_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_12_hour), MP_ROM_PTR(&BreakoutRTC_is_12_hour_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_pm), MP_ROM_PTR(&BreakoutRTC_is_pm_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_12_hour), MP_ROM_PTR(&BreakoutRTC_set_12_hour_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_24_hour), MP_ROM_PTR(&BreakoutRTC_set_24_hour_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_unix), MP_ROM_PTR(&BreakoutRTC_set_unix_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_unix), MP_ROM_PTR(&BreakoutRTC_get_unix_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_alarm_interrupt), MP_ROM_PTR(&BreakoutRTC_enable_alarm_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_alarm_interrupt), MP_ROM_PTR(&BreakoutRTC_disable_alarm_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_alarm_interrupt_flag), MP_ROM_PTR(&BreakoutRTC_read_alarm_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_alarm_interrupt_flag), MP_ROM_PTR(&BreakoutRTC_clear_alarm_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_timer), MP_ROM_PTR(&BreakoutRTC_set_timer_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_timer_count), MP_ROM_PTR(&BreakoutRTC_get_timer_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_timer), MP_ROM_PTR(&BreakoutRTC_enable_timer_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_timer), MP_ROM_PTR(&BreakoutRTC_disable_timer_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_timer_interrupt), MP_ROM_PTR(&BreakoutRTC_enable_timer_interupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_timer_interrupt), MP_ROM_PTR(&BreakoutRTC_disable_timer_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_timer_interrupt_flag), MP_ROM_PTR(&BreakoutRTC_read_timer_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_timer_interrupt_flag), MP_ROM_PTR(&BreakoutRTC_clear_timer_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_periodic_update_interrupt), MP_ROM_PTR(&BreakoutRTC_enable_periodic_update_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_periodic_update_interrupt), MP_ROM_PTR(&BreakoutRTC_disable_periodic_update_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_periodic_update_interrupt_flag), MP_ROM_PTR(&BreakoutRTC_read_periodic_update_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_periodic_update_interrupt_flag), MP_ROM_PTR(&BreakoutRTC_clear_periodic_update_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_trickle_charge), MP_ROM_PTR(&BreakoutRTC_enable_trickle_charge_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_trickle_charge), MP_ROM_PTR(&BreakoutRTC_disable_trickle_charge_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_backup_switchover_mode), MP_ROM_PTR(&BreakoutRTC_set_backup_switchover_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_clock_out), MP_ROM_PTR(&BreakoutRTC_enable_clock_out_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_interrupt_controlled_clockout), MP_ROM_PTR(&BreakoutRTC_enable_interrupt_controlled_clockout_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_clock_out), MP_ROM_PTR(&BreakoutRTC_disable_clock_out_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_clock_output_interrupt_flag), MP_ROM_PTR(&BreakoutRTC_read_clock_output_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_clock_output_interrupt_flag), MP_ROM_PTR(&BreakoutRTC_clear_clock_output_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_status), MP_ROM_PTR(&BreakoutRTC_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_interrupts), MP_ROM_PTR(&BreakoutRTC_clear_interrupts_obj) },
    { MP_ROM_QSTR(MP_QSTR_TCR_3K), MP_ROM_INT(MP_TCR_3K) },
    { MP_ROM_QSTR(MP_QSTR_TCR_5K), MP_ROM_INT(MP_TCR_5K) },
    { MP_ROM_QSTR(MP_QSTR_TCR_9K), MP_ROM_INT(MP_TCR_9K) },
    { MP_ROM_QSTR(MP_QSTR_TCR_15K), MP_ROM_INT(MP_TCR_15K) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutRTC_locals_dict, BreakoutRTC_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    breakout_rtc_BreakoutRTC_type,
    MP_QSTR_BreakoutRTC,
    MP_TYPE_FLAG_NONE,
    make_new, BreakoutRTC_make_new,
    locals_dict, (mp_obj_dict_t*)&BreakoutRTC_locals_dict
);
#else
const mp_obj_type_t breakout_rtc_BreakoutRTC_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutRTC,
    .make_new = BreakoutRTC_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutRTC_locals_dict,
};
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_rtc Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_rtc_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_rtc) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BreakoutRTC), (mp_obj_t)&breakout_rtc_BreakoutRTC_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_rtc_globals, breakout_rtc_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_rtc_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_rtc_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_rtc, breakout_rtc_user_cmodule, MODULE_BREAKOUT_RTC_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_rtc, breakout_rtc_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////