#include "drivers/pcf85063a/pcf85063a.hpp"
#include "micropython/modules/util.hpp"
#include <string>
#include <cstring>


using namespace pimoroni;


extern "C" {
#include "pcf85063a.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _pcf85063a_PCF85063A_obj_t {
    mp_obj_base_t base;
    PCF85063A *breakout;
    _PimoroniI2C_obj_t *i2c;
} pcf85063a_PCF85063A_obj_t;

/***** Print *****/
void PCF85063A_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(self_in, pcf85063a_PCF85063A_obj_t);
    PCF85063A* breakout = self->breakout;
    mp_print_str(print, "PCF85063A(");

    mp_print_str(print, "i2c = ");
    mp_obj_print_helper(print, mp_obj_new_int((breakout->get_i2c() == i2c0) ? 0 : 1), PRINT_REPR);

    mp_print_str(print, ", address = 0x");
    char buf[3];
    sprintf(buf, "%02X", breakout->get_address());
    mp_print_str(print, buf);

    mp_print_str(print, ", sda = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_sda()), PRINT_REPR);

    mp_print_str(print, ", scl = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_scl()), PRINT_REPR);

    mp_print_str(print, ", int = ");
    uint int_pin = breakout->get_int();
    if(int_pin == PIN_UNUSED)
        mp_print_str(print, "PIN_UNUSED");
    else
        mp_obj_print_helper(print, mp_obj_new_int(breakout->get_int()), PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Constructor *****/
mp_obj_t PCF85063A_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    pcf85063a_PCF85063A_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_interrupt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = PIN_UNUSED} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(pcf85063a_PCF85063A_obj_t);
    self->base.type = &pcf85063a_PCF85063A_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(PCF85063A, (pimoroni::I2C *)(self->i2c->i2c), args[ARG_interrupt].u_int);
    self->breakout->init();

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t PCF85063A_reset(mp_obj_t self_in) {
    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(self_in, pcf85063a_PCF85063A_obj_t);
    self->breakout->reset();

    return mp_const_none;
}

mp_obj_t PCF85063A_datetime(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_time };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_time, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, pcf85063a_PCF85063A_obj_t);

    if(n_args <= 1) {
        datetime_t t = self->breakout->get_datetime();

        // Return the date
        mp_obj_t tuple[7];
        tuple[0] = mp_obj_new_int(t.year);
        tuple[1] = mp_obj_new_int(t.month);
        tuple[2] = mp_obj_new_int(t.day);
        tuple[3] = mp_obj_new_int(t.hour);
        tuple[4] = mp_obj_new_int(t.min);
        tuple[5] = mp_obj_new_int(t.sec);
        tuple[6] = mp_obj_new_int((t.dotw + 6) % 7);
        return mp_obj_new_tuple(7, tuple);
    }
    else {
        // Provide a new date
        const mp_obj_t object = args[ARG_time].u_obj;

        size_t length = 0;
        mp_obj_t *items = nullptr;
        if(mp_obj_is_type(object, &mp_type_list)) {
            mp_obj_list_t *list = MP_OBJ_TO_PTR2(object, mp_obj_list_t);
            length = list->len;
            items = list->items;
        }
        else if(mp_obj_is_type(object, &mp_type_tuple)) {
            mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(object, mp_obj_tuple_t);
            length = tuple->len;
            items = tuple->items;
        }

        if(items == nullptr)
            mp_raise_TypeError("cannot convert object to a list or tuple of integers");
        else if(length < 7 || length > 8)
            mp_raise_TypeError("list or tuple must contain integers in the form (year, month, mday, hour, minute, second, weekday)");
        else {
            int year = mp_obj_get_int(items[0]);
            int month = mp_obj_get_int(items[1]);
            int day = mp_obj_get_int(items[2]);
            int hour = mp_obj_get_int(items[3]);
            int minute = mp_obj_get_int(items[4]);
            int second = mp_obj_get_int(items[5]);
            int dotw = mp_obj_get_int(items[6]);

            if(year < 2000 || year > 2099)
                mp_raise_ValueError("year out of range. Expected 2000 to 2099");
            else if(month < 1 || month > 12)
                mp_raise_ValueError("month out of range. Expected 1 to 12");
            else if(day < 1 || day > 31)
                mp_raise_ValueError("day out of range. Expected 1 to 31");
            else if(hour < 0 || hour > 23)
                mp_raise_ValueError("hour out of range. Expected 0 to 23");
            else if(minute < 0 || minute > 59)
                mp_raise_ValueError("minute out of range. Expected 0 to 59");
            else if(second < 0 || second > 59)
                mp_raise_ValueError("second out of range. Expected 0 to 59");
            else if(dotw < 0 || dotw > 6)
                mp_raise_ValueError("dotw out of range. Expected 0 to 6");
            else {
                datetime_t t;
                t.year = year;
                t.month = month;
                t.day = day;
                t.hour = hour;
                t.min = minute;
                t.sec = second;
                t.dotw = (dotw + 1) % 7;
                self->breakout->set_datetime(&t);
            }
        }
    }
    return mp_const_none;
}

mp_obj_t PCF85063A_set_alarm(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_second, ARG_minute, ARG_hour, ARG_day};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_second, MP_ARG_INT, { .u_int = PCF85063A::PARAM_UNUSED }},
        { MP_QSTR_minute, MP_ARG_INT, { .u_int = PCF85063A::PARAM_UNUSED }},
        { MP_QSTR_hour, MP_ARG_INT, { .u_int = PCF85063A::PARAM_UNUSED }},
        { MP_QSTR_day, MP_ARG_INT, { .u_int = PCF85063A::PARAM_UNUSED }},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, pcf85063a_PCF85063A_obj_t);

    int second = args[ARG_second].u_int;
    int minute = args[ARG_minute].u_int;
    int hour = args[ARG_hour].u_int;
    int day = args[ARG_day].u_int;

    if(second != PCF85063A::PARAM_UNUSED && (second < 0 || second > 59))
        mp_raise_ValueError("second out of range. Expected 0 to 59");
    else if(minute != PCF85063A::PARAM_UNUSED && (minute < 0 || minute > 59))
        mp_raise_ValueError("minute out of range. Expected 0 to 59");
    else if(hour != PCF85063A::PARAM_UNUSED && (hour < 0 || hour > 23))
        mp_raise_ValueError("hour out of range. Expected 0 to 23");
    else if(day != PCF85063A::PARAM_UNUSED && (day < 1 || day > 31))
        mp_raise_ValueError("date out of range. Expected 1 to 31");
    else
        self->breakout->set_alarm(second, minute, hour, day);

    return mp_const_none;
}

mp_obj_t PCF85063A_set_weekday_alarm(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_second, ARG_minute, ARG_hour, ARG_dotw};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_second, MP_ARG_INT, { .u_int = PCF85063A::PARAM_UNUSED }},
        { MP_QSTR_minute, MP_ARG_INT, { .u_int = PCF85063A::PARAM_UNUSED }},
        { MP_QSTR_hour, MP_ARG_INT, { .u_int = PCF85063A::PARAM_UNUSED }},
        { MP_QSTR_dotw, MP_ARG_INT, { .u_int = PCF85063A::PARAM_UNUSED }},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, pcf85063a_PCF85063A_obj_t);

    int second = args[ARG_second].u_int;
    int minute = args[ARG_minute].u_int;
    int hour = args[ARG_hour].u_int;
    int dotw = args[ARG_dotw].u_int;

    if(second != PCF85063A::PARAM_UNUSED && (second < 0 || second > 59))
        mp_raise_ValueError("second out of range. Expected 0 to 59");
    else if(minute != PCF85063A::PARAM_UNUSED && (minute < 0 || minute > 59))
        mp_raise_ValueError("minute out of range. Expected 0 to 59");
    else if(hour != PCF85063A::PARAM_UNUSED && (hour < 0 || hour > 23))
        mp_raise_ValueError("hour out of range. Expected 0 to 23");
    else if(dotw != PCF85063A::PARAM_UNUSED && (dotw < 0 || dotw > 6))
        mp_raise_ValueError("dotw out of range. Expected 0 to 6");
    else
        self->breakout->set_weekday_alarm(second, minute, hour, (PCF85063A::DayOfWeek)dotw);

    return mp_const_none;
}

mp_obj_t PCF85063A_enable_alarm_interrupt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_enable};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_enable, MP_ARG_REQUIRED | MP_ARG_BOOL},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, pcf85063a_PCF85063A_obj_t);

    bool enable = args[ARG_enable].u_bool;
    self->breakout->enable_alarm_interrupt(enable);

    return mp_const_none;
}

mp_obj_t PCF85063A_read_alarm_flag(mp_obj_t self_in) {
    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(self_in, pcf85063a_PCF85063A_obj_t);
    return mp_obj_new_bool(self->breakout->read_alarm_flag());
}

mp_obj_t PCF85063A_clear_alarm_flag(mp_obj_t self_in) {
    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(self_in, pcf85063a_PCF85063A_obj_t);
    self->breakout->clear_alarm_flag();

    return mp_const_none;
}

mp_obj_t PCF85063A_unset_alarm(mp_obj_t self_in) {
    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(self_in, pcf85063a_PCF85063A_obj_t);
    self->breakout->unset_alarm();

    return mp_const_none;
}

mp_obj_t PCF85063A_set_timer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_ticks, ARG_ttp};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_ticks, MP_ARG_REQUIRED | MP_ARG_INT},
        { MP_QSTR_ttp, MP_ARG_INT, { .u_int = PCF85063A::TIMER_TICK_1HZ}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, pcf85063a_PCF85063A_obj_t);

    int ticks = args[ARG_ticks].u_int;
    int ttp = args[ARG_ttp].u_int;

    if(ticks < 0 || ticks > 255)
        mp_raise_ValueError("ticks out of range. Expected 0 to 255");
    else if(ttp < 0 || ttp > 3)
        mp_raise_ValueError("ttp out of range. Expected 0 to 3");
    else
        self->breakout->set_timer(ticks, (PCF85063A::TimerTickPeriod)ttp);

    return mp_const_none;
}

mp_obj_t PCF85063A_enable_timer_interrupt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_enable, ARG_flag_only};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_enable, MP_ARG_REQUIRED | MP_ARG_BOOL},
        { MP_QSTR_flag_only, MP_ARG_BOOL, { .u_bool = false}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, pcf85063a_PCF85063A_obj_t);

    bool enable = args[ARG_enable].u_bool;
    bool flag_only = args[ARG_flag_only].u_bool;
    self->breakout->enable_timer_interrupt(enable, flag_only);

    return mp_const_none;
}

mp_obj_t PCF85063A_read_timer_flag(mp_obj_t self_in) {
    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(self_in, pcf85063a_PCF85063A_obj_t);
    return mp_obj_new_bool(self->breakout->read_timer_flag());
}

mp_obj_t PCF85063A_clear_timer_flag(mp_obj_t self_in) {
    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(self_in, pcf85063a_PCF85063A_obj_t);
    self->breakout->clear_timer_flag();

    return mp_const_none;
}

mp_obj_t PCF85063A_unset_timer(mp_obj_t self_in) {
    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(self_in, pcf85063a_PCF85063A_obj_t);
    self->breakout->unset_timer();

    return mp_const_none;
}

mp_obj_t PCF85063A_set_clock_output(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_co};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_co, MP_ARG_REQUIRED | MP_ARG_INT},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    pcf85063a_PCF85063A_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, pcf85063a_PCF85063A_obj_t);

    int co = args[ARG_co].u_int;

    if(co < 0 || co > 7)
        mp_raise_ValueError("co out of range. Expected 0 to 7");
    else
        self->breakout->set_clock_output((PCF85063A::ClockOut)co);

    return mp_const_none;
}
}