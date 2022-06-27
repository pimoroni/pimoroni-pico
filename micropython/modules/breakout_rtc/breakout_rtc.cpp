#include "libraries/breakout_rtc/breakout_rtc.hpp"
#include "micropython/modules/util.hpp"
#include <string>
#include <cstring>


using namespace pimoroni;


extern "C" {
#include "breakout_rtc.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_rtc_BreakoutRTC_obj_t {
    mp_obj_base_t base;
    BreakoutRTC *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_rtc_BreakoutRTC_obj_t;


/***** Constructor *****/
mp_obj_t BreakoutRTC_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_rtc_BreakoutRTC_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_interrupt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = PIN_UNUSED} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_rtc_BreakoutRTC_obj_t);
    self->base.type = &breakout_rtc_BreakoutRTC_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(BreakoutRTC, (pimoroni::I2C *)(self->i2c->i2c), args[ARG_interrupt].u_int);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "BreakoutRTC: breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t BreakoutRTC_reset(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->reset();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_setup(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->setup();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_set_time(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_sec, ARG_min, ARG_hour, ARG_weekday, ARG_date, ARG_month, ARG_year };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sec, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_min, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_hour, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_weekday, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_date, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_month, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_year, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_rtc_BreakoutRTC_obj_t);

    int sec = args[ARG_sec].u_int;
    int min = args[ARG_min].u_int;
    int hour = args[ARG_hour].u_int;
    int weekday = args[ARG_weekday].u_int;
    int date = args[ARG_date].u_int;
    int month = args[ARG_month].u_int;
    int year = args[ARG_year].u_int;

    if(sec < 0 || sec > 59) mp_raise_ValueError("sec out of range. Expected 0 to 59");
    if(min < 0 || min > 59) mp_raise_ValueError("min out of range. Expected 0 to 59");
    if(hour < 0 || hour > 23) mp_raise_ValueError("hour out of range. Expected 0 to 23");
    if(weekday < 0 || weekday > 6) mp_raise_ValueError("weekday out of range. Expected 0 to 6");
    if(date < 1 || date > 31) mp_raise_ValueError("date out of range. Expected 1 to 31");
    if(month < 1 || month > 12) mp_raise_ValueError("month out of range. Expected 1 to 12");
    if(year < 2000 || year > 2099) mp_raise_ValueError("year out of range. Expected 2000 to 2099");

    return mp_obj_new_bool(self->breakout->set_time(sec, min, hour, weekday, date, month, year));
}

mp_obj_t BreakoutRTC_set_seconds(mp_obj_t self_in, mp_obj_t sec_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    int sec = mp_obj_get_int(sec_in);

    if(sec < 0 || sec > 59) mp_raise_ValueError("sec out of range. Expected 0 to 59");

    return mp_obj_new_bool(self->breakout->set_seconds(sec));
}

mp_obj_t BreakoutRTC_set_minutes(mp_obj_t self_in, mp_obj_t min_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    int min = mp_obj_get_int(min_in);

    if(min < 0 || min > 59) mp_raise_ValueError("min out of range. Expected 0 to 59");

    return mp_obj_new_bool(self->breakout->set_minutes(min));
}

mp_obj_t BreakoutRTC_set_hours(mp_obj_t self_in, mp_obj_t hour_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    int hour = mp_obj_get_int(hour_in);

    if(hour < 0 || hour > 23) mp_raise_ValueError("hour out of range. Expected 0 to 23");

    return mp_obj_new_bool(self->breakout->set_hours(hour));
}

mp_obj_t BreakoutRTC_set_weekday(mp_obj_t self_in, mp_obj_t weekday_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    int weekday = mp_obj_get_int(weekday_in);

    if(weekday < 0 || weekday > 6) mp_raise_ValueError("weekday out of range. Expected 0 to 6");

    return mp_obj_new_bool(self->breakout->set_weekday(weekday));
}

mp_obj_t BreakoutRTC_set_date(mp_obj_t self_in, mp_obj_t date_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    int date = mp_obj_get_int(date_in);

    if(date < 1 || date > 31) mp_raise_ValueError("date out of range. Expected 1 to 31");
    
    return mp_obj_new_bool(self->breakout->set_date(date));
}

mp_obj_t BreakoutRTC_set_month(mp_obj_t self_in, mp_obj_t month_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    int month = mp_obj_get_int(month_in);

    if(month < 1 || month > 12) mp_raise_ValueError("month out of range. Expected 1 to 12");

    return mp_obj_new_bool(self->breakout->set_month(month));
}

mp_obj_t BreakoutRTC_set_year(mp_obj_t self_in, mp_obj_t year_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    int year = mp_obj_get_int(year_in);

    if(year < 0 || year > 99) mp_raise_ValueError("year out of range. Expected 0 to 99");

    return mp_obj_new_bool(self->breakout->set_year(year));
}

mp_obj_t BreakoutRTC_set_to_compiler_time(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_bool(self->breakout->set_to_compiler_time());
}

mp_obj_t BreakoutRTC_update_time(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_bool(self->breakout->update_time());
}

mp_obj_t BreakoutRTC_string_date_usa(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    const char* date = self->breakout->string_date_usa();
    std::string str_date(date, strlen(date));
    return mp_obj_new_str(str_date.c_str(), str_date.length());
}

mp_obj_t BreakoutRTC_string_date(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    const char* date = self->breakout->string_date();
    std::string str_date(date, strlen(date));
    return mp_obj_new_str(str_date.c_str(), str_date.length());
}

mp_obj_t BreakoutRTC_string_time(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    const char* time = self->breakout->string_time();
    std::string str_time(time, strlen(time));
    return mp_obj_new_str(str_time.c_str(), str_time.length());
}

mp_obj_t BreakoutRTC_string_time_stamp(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    const char* time_stamp = self->breakout->string_time_stamp();
    std::string str_time_stamp(time_stamp, strlen(time_stamp));
    return mp_obj_new_str(str_time_stamp.c_str(), str_time_stamp.length());
}

mp_obj_t BreakoutRTC_get_seconds(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_int(self->breakout->get_seconds());
}

mp_obj_t BreakoutRTC_get_minutes(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_int(self->breakout->get_minutes());
}

mp_obj_t BreakoutRTC_get_hours(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_int(self->breakout->get_hours());
}

mp_obj_t BreakoutRTC_get_weekday(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_int(self->breakout->get_weekday());
}

mp_obj_t BreakoutRTC_get_date(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_int(self->breakout->get_date());
}

mp_obj_t BreakoutRTC_get_month(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_int(self->breakout->get_month());
}

mp_obj_t BreakoutRTC_get_year(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_int(self->breakout->get_year());
}

mp_obj_t BreakoutRTC_is_12_hour(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_bool(self->breakout->is_12_hour());
}

mp_obj_t BreakoutRTC_is_pm(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_bool(self->breakout->is_pm());
}

mp_obj_t BreakoutRTC_set_12_hour(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->set_12_hour();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_set_24_hour(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->set_24_hour();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_set_unix(mp_obj_t self_in, mp_obj_t value_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    int value = mp_obj_get_int(value_in);

    return mp_obj_new_bool(self->breakout->set_unix((uint32_t)value));
}

mp_obj_t BreakoutRTC_get_unix(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_int(self->breakout->get_unix());
}

mp_obj_t BreakoutRTC_enable_alarm_interrupt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args == 1) {
        enum { ARG_self };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_rtc_BreakoutRTC_obj_t);

        self->breakout->enable_alarm_interrupt();
    }
    else {
        enum { ARG_self, ARG_min, ARG_hour, ARG_date_or_weekday, ARG_set_weekday_alarm_not_date, ARG_mode, ARG_enable_clock_output };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_min, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_hour, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_date_or_weekday, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_set_weekday_alarm_not_date, MP_ARG_REQUIRED | MP_ARG_BOOL },
            { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_enable_clock_output, MP_ARG_BOOL, {.u_bool = false} },
        };

        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_rtc_BreakoutRTC_obj_t);

        int min = args[ARG_min].u_int;
        int hour = args[ARG_hour].u_int;
        int date_or_weekday = args[ARG_date_or_weekday].u_int;
        bool set_weekday_alarm_not_date = args[ARG_set_weekday_alarm_not_date].u_bool;
        int mode = args[ARG_mode].u_int;
        bool enable_clock_output = args[ARG_enable_clock_output].u_bool;

        if(min < 0 || min > 59)
            mp_raise_ValueError("min out of range. Expected 0 to 59");
        else if(hour < 0 || hour > 23)
            mp_raise_ValueError("hour out of range. Expected 0 to 23");
        else {
            if(set_weekday_alarm_not_date) {
                if(date_or_weekday < 0 || date_or_weekday > 6)
                    mp_raise_ValueError("date_or_weekday out of range. Expected 0 to 6");
            }
            else {
                if(date_or_weekday < 1 || date_or_weekday > 31)
                    mp_raise_ValueError("date_or_weekday out of range. Expected 1 to 31");
            }
            self->breakout->enable_alarm_interrupt(min, hour, date_or_weekday, set_weekday_alarm_not_date, mode, enable_clock_output);
        }
    }

    return mp_const_none;
}

mp_obj_t BreakoutRTC_disable_alarm_interrupt(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->disable_alarm_interrupt();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_read_alarm_interrupt_flag(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_bool(self->breakout->read_alarm_interrupt_flag());
}

mp_obj_t BreakoutRTC_clear_alarm_interrupt_flag(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->clear_alarm_interrupt_flag();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_set_timer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_timer_repeat, ARG_timer_frequency, ARG_timer_value, ARG_set_interrupt, ARG_start_timer, ARG_enable_clock_output };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_timer_repeat, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_timer_frequency, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_timer_value, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_set_interrupt, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_start_timer, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_enable_clock_output, MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_rtc_BreakoutRTC_obj_t);

    bool timer_repeat = args[ARG_timer_repeat].u_bool;
    int timer_frequency = args[ARG_timer_frequency].u_int;
    int timer_value = args[ARG_timer_value].u_int;
    bool set_interrupt = args[ARG_set_interrupt].u_bool;
    bool start_timer = args[ARG_start_timer].u_bool;
    bool enable_clock_output = args[ARG_enable_clock_output].u_bool;

    if(timer_value < 0 || timer_value > 4065) {
        mp_raise_ValueError("timer_value out of range. Expected 0 to 4095");
    }
    else {
        switch(timer_frequency) {
            case 4096:  // 4096Hz (default)    // up to 122us error on first time
            case 64:    // 64Hz          // up to 7.813ms error on first time
            case 1:     // 1Hz          // up to 7.813ms error on first time
            case 60000: // 1/60Hz        // up to 7.813ms error on first time
                self->breakout->set_timer(timer_repeat, timer_frequency, timer_value, set_interrupt, start_timer, enable_clock_output);
                break;

            default:
                mp_raise_ValueError("timer_frequency not valid. Expected, 4096, 64, 1, or 60000");
                break;
        }
    }

    return mp_const_none;
}

mp_obj_t BreakoutRTC_get_timer_count(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_int(self->breakout->get_timer_count());
}

mp_obj_t BreakoutRTC_enable_timer(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->enable_timer();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_disable_timer(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->disable_timer();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_enable_timer_interupt(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->enable_timer_interrupt();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_disable_timer_interrupt(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->disable_timer_interrupt();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_read_timer_interrupt_flag(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_bool(self->breakout->read_timer_interrupt_flag());
}

mp_obj_t BreakoutRTC_clear_timer_interrupt_flag(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->clear_timer_interrupt_flag();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_enable_periodic_update_interrupt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_every_second, ARG_enable_clock_output };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_every_second, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_enable_clock_output, MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_rtc_BreakoutRTC_obj_t);

    bool every_second = args[ARG_every_second].u_bool;
    bool enable_clock_output = args[ARG_enable_clock_output].u_bool;
    self->breakout->enable_periodic_update_interrupt(every_second, enable_clock_output);

    return mp_const_none;
}

mp_obj_t BreakoutRTC_disable_periodic_update_interrupt(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->disable_periodic_update_interrupt();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_read_periodic_update_interrupt_flag(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_bool(self->breakout->read_periodic_update_interrupt_flag());
}

mp_obj_t BreakoutRTC_clear_periodic_update_interrupt_flag(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->clear_periodic_update_interrupt_flag();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_enable_trickle_charge(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_tcr };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_tcr, MP_ARG_INT, {.u_int = TCR_15K} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_rtc_BreakoutRTC_obj_t);

    int tcr = args[ARG_tcr].u_int;

    if(tcr < 0 || tcr > 3) mp_raise_ValueError("tcr out of range. Expected 0 to 3 (TCR_3K, TCR_5K, TCR_9K, TCR_15K)");

    self->breakout->enable_trickle_charge(tcr);

    return mp_const_none;
}

mp_obj_t BreakoutRTC_disable_trickle_charge(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->disable_trickle_charge();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_set_backup_switchover_mode(mp_obj_t self_in, mp_obj_t val_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    int val = mp_obj_get_int(val_in);

    if(val < 0 || val > 3) mp_raise_ValueError("tcr out of range. Expected 0 to 3");

    self->breakout->set_backup_switchover_mode(val);

    return mp_const_none;
}

mp_obj_t BreakoutRTC_enable_clock_out(mp_obj_t self_in, mp_obj_t freq_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    int freq = mp_obj_get_int(freq_in);

    if(freq < 0 || freq > 7) mp_raise_ValueError("freq out of range. Expected 0 to 7");

    self->breakout->enable_clock_out(freq);

    return mp_const_none;
}

mp_obj_t BreakoutRTC_enable_interrupt_controlled_clockout(mp_obj_t self_in, mp_obj_t freq_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);

    int freq = mp_obj_get_int(freq_in);

    if(freq < 0 || freq > 7) mp_raise_ValueError("freq out of range. Expected 0 to 7");

    self->breakout->enable_interrupt_controlled_clockout(freq);

    return mp_const_none;
}

mp_obj_t BreakoutRTC_disable_clock_out(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->disable_clock_out();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_read_clock_output_interrupt_flag(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_int(self->breakout->read_clock_output_interrupt_flag());
}

mp_obj_t BreakoutRTC_clear_clock_output_interrupt_flag(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->clear_clock_output_interrupt_flag();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_status(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_obj_new_int(self->breakout->status());
}

mp_obj_t BreakoutRTC_clear_interrupts(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->clear_interrupts();

    return mp_const_none;
}
}