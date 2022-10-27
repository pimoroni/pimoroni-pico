#include "breakout_rv3032.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BreakoutRTC Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_reset_obj, BreakoutRV3032_reset);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_setup_obj, BreakoutRV3032_setup);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRV3032_set_time_obj, 8, BreakoutRV3032_set_time);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRV3032_set_seconds_obj, BreakoutRV3032_set_seconds);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRV3032_set_minutes_obj, BreakoutRV3032_set_minutes);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRV3032_set_hours_obj, BreakoutRV3032_set_hours);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRV3032_set_weekday_obj, BreakoutRV3032_set_weekday);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRV3032_set_date_obj, BreakoutRV3032_set_date);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRV3032_set_month_obj, BreakoutRV3032_set_month);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRV3032_set_year_obj, BreakoutRV3032_set_year);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_set_to_compiler_time_obj, BreakoutRV3032_set_to_compiler_time);

MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_update_time_obj, BreakoutRV3032_update_time);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_string_date_usa_obj, BreakoutRV3032_string_date_usa);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_string_date_obj, BreakoutRV3032_string_date);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_string_time_obj, BreakoutRV3032_string_time);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_string_time_stamp_obj, BreakoutRV3032_string_time_stamp);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_get_seconds_obj, BreakoutRV3032_get_seconds);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_get_minutes_obj, BreakoutRV3032_get_minutes);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_get_hours_obj, BreakoutRV3032_get_hours);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_get_weekday_obj, BreakoutRV3032_get_weekday);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_get_date_obj, BreakoutRV3032_get_date);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_get_month_obj, BreakoutRV3032_get_month);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_get_year_obj, BreakoutRV3032_get_year);

MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_is_12_hour_obj, BreakoutRV3032_is_12_hour);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_is_pm_obj, BreakoutRV3032_is_pm);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_set_12_hour_obj, BreakoutRV3032_set_12_hour);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_set_24_hour_obj, BreakoutRV3032_set_24_hour);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRV3032_enable_alarm_interrupt_obj, 6, BreakoutRV3032_enable_alarm_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_disable_alarm_interrupt_obj, BreakoutRV3032_disable_alarm_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_read_alarm_interrupt_flag_obj, BreakoutRV3032_read_alarm_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_clear_alarm_interrupt_flag_obj, BreakoutRV3032_clear_alarm_interrupt_flag);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRV3032_set_timer_obj, 6, BreakoutRV3032_set_timer);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_enable_timer_obj, BreakoutRV3032_enable_timer);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_disable_timer_obj, BreakoutRV3032_disable_timer);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_enable_timer_interupt_obj, BreakoutRV3032_enable_timer_interupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_disable_timer_interrupt_obj, BreakoutRV3032_disable_timer_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_read_timer_interrupt_flag_obj, BreakoutRV3032_read_timer_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_clear_timer_interrupt_flag_obj, BreakoutRV3032_clear_timer_interrupt_flag);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRV3032_enable_periodic_update_interrupt_obj, 2, BreakoutRV3032_enable_periodic_update_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_disable_periodic_update_interrupt_obj, BreakoutRV3032_disable_periodic_update_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_read_periodic_update_interrupt_flag_obj, BreakoutRV3032_read_periodic_update_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_clear_periodic_update_interrupt_flag_obj, BreakoutRV3032_clear_periodic_update_interrupt_flag);

MP_DEFINE_CONST_FUN_OBJ_KW(BreakoutRV3032_enable_trickle_charge_obj, 2, BreakoutRV3032_enable_trickle_charge);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_disable_trickle_charge_obj, BreakoutRV3032_disable_trickle_charge);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRV3032_set_backup_switchover_mode_obj, BreakoutRV3032_set_backup_switchover_mode);

MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRV3032_enable_clock_out_obj, BreakoutRV3032_enable_clock_out);
MP_DEFINE_CONST_FUN_OBJ_2(BreakoutRV3032_enable_interrupt_controlled_clockout_obj, BreakoutRV3032_enable_interrupt_controlled_clockout);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_disable_clock_out_obj, BreakoutRV3032_disable_clock_out);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_read_clock_output_interrupt_flag_obj, BreakoutRV3032_read_clock_output_interrupt_flag);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_clear_clock_output_interrupt_flag_obj, BreakoutRV3032_clear_clock_output_interrupt_flag);

MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_status_obj, BreakoutRV3032_status);
MP_DEFINE_CONST_FUN_OBJ_1(BreakoutRV3032_clear_interrupts_obj, BreakoutRV3032_clear_interrupts);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t BreakoutRV3032_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&BreakoutRV3032_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_setup), MP_ROM_PTR(&BreakoutRV3032_setup_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_time), MP_ROM_PTR(&BreakoutRV3032_set_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_seconds), MP_ROM_PTR(&BreakoutRV3032_set_seconds_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_minutes), MP_ROM_PTR(&BreakoutRV3032_set_minutes_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_hours), MP_ROM_PTR(&BreakoutRV3032_set_hours_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_weekday), MP_ROM_PTR(&BreakoutRV3032_set_weekday_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_date), MP_ROM_PTR(&BreakoutRV3032_set_date_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_month), MP_ROM_PTR(&BreakoutRV3032_set_month_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_year), MP_ROM_PTR(&BreakoutRV3032_set_year_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_to_compiler_time), MP_ROM_PTR(&BreakoutRV3032_set_to_compiler_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_update_time), MP_ROM_PTR(&BreakoutRV3032_update_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_string_date_usa), MP_ROM_PTR(&BreakoutRV3032_string_date_usa_obj) },
    { MP_ROM_QSTR(MP_QSTR_string_date), MP_ROM_PTR(&BreakoutRV3032_string_date_obj) },
    { MP_ROM_QSTR(MP_QSTR_string_time), MP_ROM_PTR(&BreakoutRV3032_string_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_string_time_stamp), MP_ROM_PTR(&BreakoutRV3032_string_time_stamp_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_seconds), MP_ROM_PTR(&BreakoutRV3032_get_seconds_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_minutes), MP_ROM_PTR(&BreakoutRV3032_get_minutes_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_hours), MP_ROM_PTR(&BreakoutRV3032_get_hours_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_weekday), MP_ROM_PTR(&BreakoutRV3032_get_weekday_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_date), MP_ROM_PTR(&BreakoutRV3032_get_date_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_month), MP_ROM_PTR(&BreakoutRV3032_get_month_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_year), MP_ROM_PTR(&BreakoutRV3032_get_year_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_12_hour), MP_ROM_PTR(&BreakoutRV3032_is_12_hour_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_pm), MP_ROM_PTR(&BreakoutRV3032_is_pm_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_12_hour), MP_ROM_PTR(&BreakoutRV3032_set_12_hour_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_24_hour), MP_ROM_PTR(&BreakoutRV3032_set_24_hour_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_alarm_interrupt), MP_ROM_PTR(&BreakoutRV3032_enable_alarm_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_alarm_interrupt), MP_ROM_PTR(&BreakoutRV3032_disable_alarm_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_alarm_interrupt_flag), MP_ROM_PTR(&BreakoutRV3032_read_alarm_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_alarm_interrupt_flag), MP_ROM_PTR(&BreakoutRV3032_clear_alarm_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_timer), MP_ROM_PTR(&BreakoutRV3032_set_timer_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_timer), MP_ROM_PTR(&BreakoutRV3032_enable_timer_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_timer), MP_ROM_PTR(&BreakoutRV3032_disable_timer_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_timer_interrupt), MP_ROM_PTR(&BreakoutRV3032_enable_timer_interupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_timer_interrupt), MP_ROM_PTR(&BreakoutRV3032_disable_timer_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_timer_interrupt_flag), MP_ROM_PTR(&BreakoutRV3032_read_timer_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_timer_interrupt_flag), MP_ROM_PTR(&BreakoutRV3032_clear_timer_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_periodic_update_interrupt), MP_ROM_PTR(&BreakoutRV3032_enable_periodic_update_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_periodic_update_interrupt), MP_ROM_PTR(&BreakoutRV3032_disable_periodic_update_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_periodic_update_interrupt_flag), MP_ROM_PTR(&BreakoutRV3032_read_periodic_update_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_periodic_update_interrupt_flag), MP_ROM_PTR(&BreakoutRV3032_clear_periodic_update_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_trickle_charge), MP_ROM_PTR(&BreakoutRV3032_enable_trickle_charge_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_trickle_charge), MP_ROM_PTR(&BreakoutRV3032_disable_trickle_charge_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_backup_switchover_mode), MP_ROM_PTR(&BreakoutRV3032_set_backup_switchover_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_clock_out), MP_ROM_PTR(&BreakoutRV3032_enable_clock_out_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_interrupt_controlled_clockout), MP_ROM_PTR(&BreakoutRV3032_enable_interrupt_controlled_clockout_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_clock_out), MP_ROM_PTR(&BreakoutRV3032_disable_clock_out_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_clock_output_interrupt_flag), MP_ROM_PTR(&BreakoutRV3032_read_clock_output_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_clock_output_interrupt_flag), MP_ROM_PTR(&BreakoutRV3032_clear_clock_output_interrupt_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_status), MP_ROM_PTR(&BreakoutRV3032_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_interrupts), MP_ROM_PTR(&BreakoutRV3032_clear_interrupts_obj) },
    { MP_ROM_QSTR(MP_QSTR_TCR_3K), MP_ROM_INT(MP_TCR_3K) },
    { MP_ROM_QSTR(MP_QSTR_TCR_5K), MP_ROM_INT(MP_TCR_5K) },
    { MP_ROM_QSTR(MP_QSTR_TCR_9K), MP_ROM_INT(MP_TCR_9K) },
    { MP_ROM_QSTR(MP_QSTR_TCR_15K), MP_ROM_INT(MP_TCR_15K) },
};
STATIC MP_DEFINE_CONST_DICT(BreakoutRV3032_locals_dict, BreakoutRV3032_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t breakout_rtc_Breakoutrv3032_type = {
    { &mp_type_type },
    .name = MP_QSTR_BreakoutRV3032,
    .make_new = BreakoutRV3032_make_new,
    .locals_dict = (mp_obj_dict_t*)&BreakoutRV3032_locals_dict,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// breakout_rtc Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t breakout_rtc_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_breakout_rv3032) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Breakout_RV3032), (mp_obj_t)&breakout_rtc_Breakoutrv3032_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_breakout_rtc_globals, breakout_rtc_globals_table);

/***** Module Definition *****/
const mp_obj_module_t breakout_rtc_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_breakout_rtc_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_breakout_rv3032, breakout_rtc_user_cmodule, MODULE_BREAKOUT_RTC_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_breakout_rv3032, breakout_rtc_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////