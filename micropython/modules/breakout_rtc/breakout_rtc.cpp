#include "../../../libraries/breakout_rtc/breakout_rtc.hpp"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))


using namespace pimoroni;


extern "C" {
#include "breakout_rtc.h"

/***** Variables Struct *****/
typedef struct _breakout_rtc_BreakoutRTC_obj_t {
    mp_obj_base_t base;
    BreakoutRTC *breakout;
} breakout_rtc_BreakoutRTC_obj_t;

/***** Print *****/
void BreakoutRTC_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    BreakoutRTC* breakout = self->breakout;
    mp_print_str(print, "BreakoutRTC(");

    mp_print_str(print, "i2c = ");
    mp_obj_print_helper(print, mp_obj_new_int((breakout->get_i2c() == i2c0) ? 0 : 1), PRINT_REPR);

    mp_print_str(print, ", sda = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_sda()), PRINT_REPR);

    mp_print_str(print, ", scl = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_scl()), PRINT_REPR);

    mp_print_str(print, ", int = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_int()), PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Constructor *****/
mp_obj_t BreakoutRTC_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_rtc_BreakoutRTC_obj_t *self = nullptr;

    if(n_args == 0) {
        mp_arg_check_num(n_args, n_kw, 0, 0, true);
        self = m_new_obj(breakout_rtc_BreakoutRTC_obj_t);
        self->base.type = &breakout_rtc_BreakoutRTC_type;
        self->breakout = new BreakoutRTC();
    }
    else {
        enum { ARG_i2c, ARG_sda, ARG_scl, ARG_interrupt };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_i2c, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = BreakoutRTC::PIN_UNUSED} },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        // Get I2C bus.
        int i2c_id = args[ARG_i2c].u_int;
        if(i2c_id < 0 || i2c_id > 1) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C(%d) doesn't exist"), i2c_id);
        }

        int sda = args[ARG_sda].u_int;
        if (!IS_VALID_SDA(i2c_id, sda)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad SDA pin"));
        }

        int scl = args[ARG_scl].u_int;
        if (!IS_VALID_SCL(i2c_id, scl)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad SCL pin"));
        }        

        self = m_new_obj(breakout_rtc_BreakoutRTC_obj_t);
        self->base.type = &breakout_rtc_BreakoutRTC_type;
        
        i2c_inst_t *i2c = (i2c_id == 0) ? i2c0 : i2c1;
        self->breakout = new BreakoutRTC(i2c, sda, scl, args[ARG_interrupt].u_int);
    }

    self->breakout->init();

    return MP_OBJ_FROM_PTR(self);
}

// mp_obj_t picoscroll_init() {
//     scroll.init();
//     return mp_const_none;
// }

// mp_obj_t picoscroll_get_width() {
//     return mp_obj_new_int(PicoScroll::WIDTH);
// }

// mp_obj_t picoscroll_get_height() {
//     return mp_obj_new_int(PicoScroll::HEIGHT);
// }

// mp_obj_t picoscroll_update() {
//     scroll.update();
//     return mp_const_none;
// }

// mp_obj_t picoscroll_set_pixel(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t v_obj) {
//     int x = mp_obj_get_int(x_obj);
//     int y = mp_obj_get_int(y_obj);
//     int val = mp_obj_get_int(v_obj);

//     if(x < 0 || x >= PicoScroll::WIDTH || y < 0 || y >= PicoScroll::HEIGHT)
//         mp_raise_ValueError("x or y out of range.");
//     else
//     {
//         if(val < 0 || val > 255)
//             mp_raise_ValueError("val out of range. Expected 0 to 255");
//         else
//             scroll.set_pixel(x, y, val);
//     }

//     return mp_const_none;
// }

// mp_obj_t picoscroll_clear() {
//     scroll.clear();
//     return mp_const_none;
// }

// mp_obj_t picoscroll_is_pressed(mp_obj_t button_obj) {
//     int buttonID = mp_obj_get_int(button_obj);

//     bool buttonPressed = false;
//     switch(buttonID)
//     {
//     case 0:
//         buttonPressed = scroll.is_pressed(PicoScroll::A);
//         break;

//     case 1:
//         buttonPressed = scroll.is_pressed(PicoScroll::B);
//         break;

//     case 2:
//         buttonPressed = scroll.is_pressed(PicoScroll::X);
//         break;

//     case 3:
//         buttonPressed = scroll.is_pressed(PicoScroll::Y);
//         break;

//     default:
//         mp_raise_ValueError("button not valid. Expected 0 to 3");
//         break;
//     }

//     return buttonPressed ? mp_const_true : mp_const_false;
// }

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

    if(sec < 0 || sec > 59)
        mp_raise_ValueError("sec out of range. Expected 0 to 59");
    else if(min < 0 || min > 59)
        mp_raise_ValueError("min out of range. Expected 0 to 59");
    else if(hour < 0 || hour > 23)
        mp_raise_ValueError("hour out of range. Expected 0 to 23");
    else if(weekday < 0 || weekday > 6)
        mp_raise_ValueError("weekday out of range. Expected 0 to 6");
    else if(date < 1 || date > 31)
        mp_raise_ValueError("date out of range. Expected 1 to 31");
    else if(month < 1 || month > 12)
        mp_raise_ValueError("month out of range. Expected 1 to 12");
    else if(year < 0 || year > 99)
        mp_raise_ValueError("year out of range. Expected 0 to 99");
    else
        return mp_obj_new_bool(self->breakout->set_time(sec, min, hour, weekday, date, month, year));

    return mp_const_none;
}

mp_obj_t BreakoutRTC_set_seconds(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_sec};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sec, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_rtc_BreakoutRTC_obj_t);

    int sec = args[ARG_sec].u_int;

    if(sec < 0 || sec > 59)
        mp_raise_ValueError("sec out of range. Expected 0 to 59");
    else
        return mp_obj_new_bool(self->breakout->set_seconds(sec));

    return mp_const_none;
}

mp_obj_t BreakoutRTC_set_minutes(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_min };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_min, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_rtc_BreakoutRTC_obj_t);

    int min = args[ARG_min].u_int;

    if(min < 0 || min > 59)
        mp_raise_ValueError("min out of range. Expected 0 to 59");
    else
        return mp_obj_new_bool(self->breakout->set_minutes(min));

    return mp_const_none;
}

mp_obj_t BreakoutRTC_set_hours(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_hour };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_hour, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_rtc_BreakoutRTC_obj_t);

    int hour = args[ARG_hour].u_int;

    if(hour < 0 || hour > 23)
        mp_raise_ValueError("hour out of range. Expected 0 to 23");
    else
        return mp_obj_new_bool(self->breakout->set_hours(hour));

    return mp_const_none;
}

mp_obj_t BreakoutRTC_set_weekday(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_weekday };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_weekday, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_rtc_BreakoutRTC_obj_t);

    int weekday = args[ARG_weekday].u_int;

    if(weekday < 0 || weekday > 6)
        mp_raise_ValueError("weekday out of range. Expected 0 to 6");
    else
        return mp_obj_new_bool(self->breakout->set_weekday(weekday));

    return mp_const_none;
}

mp_obj_t BreakoutRTC_set_date(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_date };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_date, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_rtc_BreakoutRTC_obj_t);

    int date = args[ARG_date].u_int;

    if(date < 1 || date > 31)
        mp_raise_ValueError("date out of range. Expected 1 to 31");
    else
        return mp_obj_new_bool(self->breakout->set_date(date));

    return mp_const_none;
}

mp_obj_t BreakoutRTC_set_month(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_month };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_month, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_rtc_BreakoutRTC_obj_t);

    int month = args[ARG_month].u_int;

    if(month < 1 || month > 12)
        mp_raise_ValueError("month out of range. Expected 1 to 12");
    else
        return mp_obj_new_bool(self->breakout->set_month(month));

    return mp_const_none;
}

mp_obj_t BreakoutRTC_set_year(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_year };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_year, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_rtc_BreakoutRTC_obj_t);

    int year = args[ARG_year].u_int;

    if(year < 0 || year > 99)
        mp_raise_ValueError("year out of range. Expected 0 to 99");
    else
        return mp_obj_new_bool(self->breakout->set_year(year));

    return mp_const_none;
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
    //TODO
    //breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_const_none;
}

mp_obj_t BreakoutRTC_string_date(mp_obj_t self_in) {
    //TODO
    //breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_const_none;
}

mp_obj_t BreakoutRTC_string_time(mp_obj_t self_in) {
    //TODO
    //breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_const_none;
}

mp_obj_t BreakoutRTC_string_time_stamp(mp_obj_t self_in) {
    //TODO
    //breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    return mp_const_none;
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

mp_obj_t BreakoutRTC_set_unix(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_rtc_BreakoutRTC_obj_t);

    int value = args[ARG_value].u_int;

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
    //TODO
    enum { ARG_self, ARG_light, ARG_proximity };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_light, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_proximity, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

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
    //TODO
    enum { ARG_self, ARG_light, ARG_proximity };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_light, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_proximity, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

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
    //TODO
    enum { ARG_self, ARG_light, ARG_proximity };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_light, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_proximity, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    return mp_const_none;
}

mp_obj_t BreakoutRTC_disable_trickle_charge(mp_obj_t self_in) {
    breakout_rtc_BreakoutRTC_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_rtc_BreakoutRTC_obj_t);
    self->breakout->disable_trickle_charge();

    return mp_const_none;
}

mp_obj_t BreakoutRTC_set_backup_switchover_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_light, ARG_proximity };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_light, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_proximity, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    return mp_const_none;
}

mp_obj_t BreakoutRTC_enable_clock_out(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_light, ARG_proximity };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_light, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_proximity, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    return mp_const_none;
}

mp_obj_t BreakoutRTC_enable_interrupt_controlled_clockout(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_light, ARG_proximity };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_light, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_proximity, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
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