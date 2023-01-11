#include "pcf85063a.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// PCF85063A Class
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(PCF85063A_reset_obj, PCF85063A_reset);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(PCF85063A_datetime_obj, 1, 2, PCF85063A_datetime);

MP_DEFINE_CONST_FUN_OBJ_KW(PCF85063A_set_alarm_obj, 1, PCF85063A_set_alarm);
MP_DEFINE_CONST_FUN_OBJ_KW(PCF85063A_set_weekday_alarm_obj, 1, PCF85063A_set_weekday_alarm);
MP_DEFINE_CONST_FUN_OBJ_2(PCF85063A_enable_alarm_interrupt_obj, PCF85063A_enable_alarm_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(PCF85063A_read_alarm_flag_obj, PCF85063A_read_alarm_flag);
MP_DEFINE_CONST_FUN_OBJ_1(PCF85063A_clear_alarm_flag_obj, PCF85063A_clear_alarm_flag);
MP_DEFINE_CONST_FUN_OBJ_1(PCF85063A_unset_alarm_obj, PCF85063A_unset_alarm);

MP_DEFINE_CONST_FUN_OBJ_KW(PCF85063A_set_timer_obj, 2, PCF85063A_set_timer);
MP_DEFINE_CONST_FUN_OBJ_KW(PCF85063A_enable_timer_interrupt_obj, 2, PCF85063A_enable_timer_interrupt);
MP_DEFINE_CONST_FUN_OBJ_1(PCF85063A_read_timer_flag_obj, PCF85063A_read_timer_flag);
MP_DEFINE_CONST_FUN_OBJ_1(PCF85063A_clear_timer_flag_obj, PCF85063A_clear_timer_flag);
MP_DEFINE_CONST_FUN_OBJ_1(PCF85063A_unset_timer_obj, PCF85063A_unset_timer);

MP_DEFINE_CONST_FUN_OBJ_2(PCF85063A_set_clock_output_obj, PCF85063A_set_clock_output);
MP_DEFINE_CONST_FUN_OBJ_2(PCF85063A_set_byte_obj, PCF85063A_set_byte);
MP_DEFINE_CONST_FUN_OBJ_1(PCF85063A_get_byte_obj, PCF85063A_get_byte);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t PCF85063A_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&PCF85063A_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&PCF85063A_datetime_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_alarm), MP_ROM_PTR(&PCF85063A_set_alarm_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_weekday_alarm), MP_ROM_PTR(&PCF85063A_set_weekday_alarm_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_alarm_interrupt), MP_ROM_PTR(&PCF85063A_enable_alarm_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_alarm_flag), MP_ROM_PTR(&PCF85063A_read_alarm_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_alarm_flag), MP_ROM_PTR(&PCF85063A_clear_alarm_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_unset_alarm), MP_ROM_PTR(&PCF85063A_unset_alarm_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_timer), MP_ROM_PTR(&PCF85063A_set_timer_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_timer_interrupt), MP_ROM_PTR(&PCF85063A_enable_timer_interrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_timer_flag), MP_ROM_PTR(&PCF85063A_read_timer_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_timer_flag), MP_ROM_PTR(&PCF85063A_clear_timer_flag_obj) },
    { MP_ROM_QSTR(MP_QSTR_unset_timer), MP_ROM_PTR(&PCF85063A_unset_timer_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_clock_output), MP_ROM_PTR(&PCF85063A_set_clock_output_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_byte), MP_ROM_PTR(&PCF85063A_set_byte_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_byte), MP_ROM_PTR(&PCF85063A_get_byte_obj) },

    { MP_ROM_QSTR(MP_QSTR_CLOCK_OUT_32768HZ), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK_OUT_16384HZ), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK_OUT_8192HZ), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK_OUT_4096HZ), MP_ROM_INT(3) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK_OUT_2048HZ), MP_ROM_INT(4) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK_OUT_1024HZ), MP_ROM_INT(5) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK_OUT_1HZ), MP_ROM_INT(6) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK_OUT_OFF), MP_ROM_INT(7) },

    { MP_ROM_QSTR(MP_QSTR_TIMER_TICK_4096HZ), MP_ROM_INT(0b00) },
    { MP_ROM_QSTR(MP_QSTR_TIMER_TICK_64HZ), MP_ROM_INT(0b01) },
    { MP_ROM_QSTR(MP_QSTR_TIMER_TICK_1HZ), MP_ROM_INT(0b10) },
    { MP_ROM_QSTR(MP_QSTR_TIMER_TICK_1_OVER_60HZ), MP_ROM_INT(0b11) },
};
STATIC MP_DEFINE_CONST_DICT(PCF85063A_locals_dict, PCF85063A_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    pcf85063a_PCF85063A_type,
    MP_QSTR_pcf85063a,
    MP_TYPE_FLAG_NONE,
    make_new, PCF85063A_make_new,
    locals_dict, (mp_obj_dict_t*)&PCF85063A_locals_dict
);
#else
const mp_obj_type_t pcf85063a_PCF85063A_type = {
    { &mp_type_type },
    .name = MP_QSTR_pcf85063a,
    .make_new = PCF85063A_make_new,
    .locals_dict = (mp_obj_dict_t*)&PCF85063A_locals_dict,
};
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// pcf85063a Module
////////////////////////////////////////////////////////////////////////////////////////////////////

/***** Globals Table *****/
STATIC const mp_map_elem_t pcf85063a_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_pcf85063a) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PCF85063A), (mp_obj_t)&pcf85063a_PCF85063A_type },
    { MP_ROM_QSTR(MP_QSTR_MONDAY), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_TUESDAY), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_WEDNESDAY), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_THURSDAY), MP_ROM_INT(3) },
    { MP_ROM_QSTR(MP_QSTR_FRIDAY), MP_ROM_INT(4) },
    { MP_ROM_QSTR(MP_QSTR_SATURDAY), MP_ROM_INT(5) },
    { MP_ROM_QSTR(MP_QSTR_SUNDAY), MP_ROM_INT(6) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_pcf85063a_globals, pcf85063a_globals_table);

/***** Module Definition *****/
const mp_obj_module_t pcf85063a_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_pcf85063a_globals,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_pcf85063a, pcf85063a_user_cmodule, MODULE_BREAKOUT_RTC_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_pcf85063a, pcf85063a_user_cmodule);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////