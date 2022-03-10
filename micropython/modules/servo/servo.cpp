#include "drivers/servo/servo.hpp"
#include "drivers/servo/servo_cluster.hpp"
#include <cstdio>

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

using namespace servo;

extern "C" {
#include "servo.h"
#include "py/builtin.h"


/********** Calibration **********/

/***** Variables Struct *****/
typedef struct _Calibration_obj_t {
    mp_obj_base_t base;
    Calibration *calibration;
} _Calibtration_obj_t;


/***** Print *****/
void Calibration_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _Calibtration_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Calibtration_obj_t);
    Calibration* calib = self->calibration;
    mp_print_str(print, "Calibration(");

    uint size = calib->size();
    mp_print_str(print, "size = ");
    mp_obj_print_helper(print, mp_obj_new_int(size), PRINT_REPR);

    mp_print_str(print, ", points = {");
    for(uint i = 0; i < size; i++) {
        Calibration::Point *point = calib->point_at(i);
        mp_print_str(print, "{");
        mp_obj_print_helper(print, mp_obj_new_float(point->pulse), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_float(point->value), PRINT_REPR);
        mp_print_str(print, "}");
        if(i < size - 1)
            mp_print_str(print, ", ");
    }
    mp_print_str(print, "}, limit_lower = ");
    mp_obj_print_helper(print, calib->has_lower_limit() ? mp_const_true : mp_const_false, PRINT_REPR);
    mp_print_str(print, ", limit_upper = ");
    mp_obj_print_helper(print, calib->has_upper_limit() ? mp_const_true : mp_const_false, PRINT_REPR);
    mp_print_str(print, ")");
}


/***** Constructor *****/
mp_obj_t Calibration_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _Calibtration_obj_t *self = nullptr;

    enum { ARG_type };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_type, MP_ARG_INT, {.u_int = (uint8_t)servo::CalibrationType::ANGULAR} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    servo::CalibrationType calibration_type = (servo::CalibrationType)args[ARG_type].u_int;

    self = m_new_obj_with_finaliser(_Calibtration_obj_t);
    self->base.type = &Calibration_type;

    self->calibration = new Calibration(calibration_type);
    return MP_OBJ_FROM_PTR(self);
}


/***** Destructor ******/
mp_obj_t Calibration___del__(mp_obj_t self_in) {
    _Calibtration_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Calibtration_obj_t);
    delete self->calibration;
    return mp_const_none;
}


/***** Methods *****/
mp_obj_t Calibration_create_blank_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_size };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_size, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

    int size = args[ARG_size].u_int;
    if(size < 0)
        mp_raise_ValueError("size out of range. Expected 0 or greater");
    else
        self->calibration->create_blank_calibration((uint)size);

    return mp_const_none;
}

mp_obj_t Calibration_create_two_point_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_min_pulse, ARG_max_pulse, ARG_min_value, ARG_max_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_min_pulse, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_max_pulse, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_min_value, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_max_value, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

    float min_pulse = mp_obj_get_float(args[ARG_min_pulse].u_obj);
    float max_pulse = mp_obj_get_float(args[ARG_max_pulse].u_obj);
    float min_value = mp_obj_get_float(args[ARG_min_value].u_obj);
    float max_value = mp_obj_get_float(args[ARG_max_value].u_obj);
    self->calibration->create_two_point_calibration(min_pulse, max_pulse, min_value, max_value);

    return mp_const_none;
}

mp_obj_t Calibration_create_three_point_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_min_pulse, ARG_mid_pulse, ARG_max_pulse, ARG_min_value, ARG_mid_value, ARG_max_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_min_pulse, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_mid_pulse, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_max_pulse, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_min_value, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_mid_value, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_max_value, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

    float min_pulse = mp_obj_get_float(args[ARG_min_pulse].u_obj);
    float mid_pulse = mp_obj_get_float(args[ARG_mid_pulse].u_obj);
    float max_pulse = mp_obj_get_float(args[ARG_max_pulse].u_obj);
    float min_value = mp_obj_get_float(args[ARG_min_value].u_obj);
    float mid_value = mp_obj_get_float(args[ARG_mid_value].u_obj);
    float max_value = mp_obj_get_float(args[ARG_max_value].u_obj);
    self->calibration->create_three_point_calibration(min_pulse, mid_pulse, max_pulse, min_value, mid_value, max_value);

    return mp_const_none;
}

mp_obj_t Calibration_create_uniform_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_size, ARG_min_pulse, ARG_max_pulse, ARG_min_value, ARG_max_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_size, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_min_pulse, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_max_pulse, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_min_value, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_max_value, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

    int size = args[ARG_size].u_int;
    if(size < 0)
        mp_raise_ValueError("size out of range. Expected 0 or greater");
    else {
        float min_pulse = mp_obj_get_float(args[ARG_min_pulse].u_obj);
        float max_pulse = mp_obj_get_float(args[ARG_max_pulse].u_obj);
        float min_value = mp_obj_get_float(args[ARG_min_value].u_obj);
        float max_value = mp_obj_get_float(args[ARG_max_value].u_obj);
        self->calibration->create_uniform_calibration((uint)size, min_pulse, max_pulse, min_value, max_value);
    }

    return mp_const_none;
}

mp_obj_t Calibration_create_default_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_type };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_type, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

    servo::CalibrationType calibration_type = (servo::CalibrationType)args[ARG_type].u_int;
    self->calibration->create_default_calibration(calibration_type);

    return mp_const_none;
}

mp_obj_t Calibration_size(mp_obj_t self_in) {
    _Calibration_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Calibration_obj_t);
    return mp_obj_new_int(self->calibration->size());
}

mp_obj_t Calibration_point_at(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 2) {
        enum { ARG_self, ARG_index };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        int index = args[ARG_index].u_int;
        int calibration_size = (int)self->calibration->size();
        if(calibration_size == 0)
            mp_raise_ValueError("this calibration does not have any points");
        if(index < 0 || index >= calibration_size)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("index out of range. Expected 0 to %d"), calibration_size - 1);
        else {
            Calibration::Point *point = self->calibration->point_at((uint)index);

            mp_obj_t tuple[2];
            tuple[0] = mp_obj_new_float(point->pulse);
            tuple[1] = mp_obj_new_float(point->value);
            return mp_obj_new_tuple(2, tuple);
        }
    }
    else {
        enum { ARG_self, ARG_index, ARG_point };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_point, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        int index = args[ARG_index].u_int;
        int calibration_size = (int)self->calibration->size();
        if(calibration_size == 0)
            mp_raise_ValueError("this calibration does not have any points");
        if(index < 0 || index >= calibration_size)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("index out of range. Expected 0 to %d"), calibration_size - 1);
        else {
            Calibration::Point *point = self->calibration->point_at((uint)index);

            const mp_obj_t object = args[ARG_point].u_obj;
            if(mp_obj_is_type(object, &mp_type_list)) {
                mp_obj_list_t *list = MP_OBJ_TO_PTR2(object, mp_obj_list_t);
                if(list->len == 2) {
                    point->pulse = mp_obj_get_float(list->items[0]);
                    point->value = mp_obj_get_float(list->items[1]);
                }
                else {
                    mp_raise_ValueError("list must contain two numbers");
                }
            }
            else if(!mp_obj_is_type(object, &mp_type_tuple)) {
                mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(object, mp_obj_tuple_t);
                if(tuple->len == 2) {
                    point->pulse = mp_obj_get_float(tuple->items[0]);
                    point->value = mp_obj_get_float(tuple->items[1]);
                }
                else {
                    mp_raise_ValueError("tuple must contain two numbers");
                }
            }
            else {
                mp_raise_TypeError("can't convert object to list or tuple");
            }
        }
    }

    return mp_const_none;
}

mp_obj_t Calibration_first_point(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 1) {
        enum { ARG_self };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        Calibration::Point *point = self->calibration->first_point();
        if(point == nullptr)
            mp_raise_ValueError("this calibration does not have any points");
        else {
            mp_obj_t tuple[2];
            tuple[0] = mp_obj_new_float(point->pulse);
            tuple[1] = mp_obj_new_float(point->value);
            return mp_obj_new_tuple(2, tuple);
        }
    }
    else {
        enum { ARG_self, ARG_point };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_point, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        Calibration::Point *point = self->calibration->first_point();
        if(point == nullptr)
            mp_raise_ValueError("this calibration does not have any points");
        else {
            const mp_obj_t object = args[ARG_point].u_obj;
            if(mp_obj_is_type(object, &mp_type_list)) {
                mp_obj_list_t *list = MP_OBJ_TO_PTR2(object, mp_obj_list_t);
                if(list->len == 2) {
                    point->pulse = mp_obj_get_float(list->items[0]);
                    point->value = mp_obj_get_float(list->items[1]);
                }
                else {
                    mp_raise_ValueError("list must contain two numbers");
                }
            }
            else if(!mp_obj_is_type(object, &mp_type_tuple)) {
                mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(object, mp_obj_tuple_t);
                if(tuple->len == 2) {
                    point->pulse = mp_obj_get_float(tuple->items[0]);
                    point->value = mp_obj_get_float(tuple->items[1]);
                }
                else {
                    mp_raise_ValueError("tuple must contain two numbers");
                }
            }
            else {
                mp_raise_TypeError("can't convert object to list or tuple");
            }
        }
    }

    return mp_const_none;
}

mp_obj_t Calibration_last_point(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 1) {
        enum { ARG_self };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        Calibration::Point *point = self->calibration->last_point();
        if(point == nullptr)
            mp_raise_ValueError("this calibration does not have any points");
        else {
            mp_obj_t tuple[2];
            tuple[0] = mp_obj_new_float(point->pulse);
            tuple[1] = mp_obj_new_float(point->value);
            return mp_obj_new_tuple(2, tuple);
        }
    }
    else {
        enum { ARG_self, ARG_point };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_point, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        Calibration::Point *point = self->calibration->last_point();
        if(point == nullptr)
            mp_raise_ValueError("this calibration does not have any points");
        else {
            const mp_obj_t object = args[ARG_point].u_obj;
            if(mp_obj_is_type(object, &mp_type_list)) {
                mp_obj_list_t *list = MP_OBJ_TO_PTR2(object, mp_obj_list_t);
                if(list->len == 2) {
                    point->pulse = mp_obj_get_float(list->items[0]);
                    point->value = mp_obj_get_float(list->items[1]);
                }
                else {
                    mp_raise_ValueError("list must contain two numbers");
                }
            }
            else if(!mp_obj_is_type(object, &mp_type_tuple)) {
                mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(object, mp_obj_tuple_t);
                if(tuple->len == 2) {
                    point->pulse = mp_obj_get_float(tuple->items[0]);
                    point->value = mp_obj_get_float(tuple->items[1]);
                }
                else {
                    mp_raise_ValueError("tuple must contain two numbers");
                }
            }
            else {
                mp_raise_TypeError("can't convert object to list or tuple");
            }
        }
    }

    return mp_const_none;
}

mp_obj_t Calibration_has_lower_limit(mp_obj_t self_in) {
    _Calibration_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Calibration_obj_t);
    return self->calibration->has_lower_limit() ? mp_const_true : mp_const_false;
}

mp_obj_t Calibration_has_upper_limit(mp_obj_t self_in) {
    _Calibration_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Calibration_obj_t);
    return self->calibration->has_upper_limit() ? mp_const_true : mp_const_false;
}

mp_obj_t Calibration_limit_to_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_lower, ARG_upper };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_lower, MP_ARG_REQUIRED | MP_ARG_BOOL },
        { MP_QSTR_upper, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

    bool lower = args[ARG_lower].u_bool;
    bool upper = args[ARG_upper].u_bool;
    self->calibration->limit_to_calibration(lower, upper);

    return mp_const_none;
}

mp_obj_t Calibration_value_to_pulse(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

    float value = mp_obj_get_float(args[ARG_value].u_obj);

    float pulse_out, value_out;
    if(self->calibration->value_to_pulse(value, pulse_out, value_out)) {
        mp_obj_t tuple[2];
        tuple[0] = mp_obj_new_float(pulse_out);
        tuple[1] = mp_obj_new_float(value_out);
        return mp_obj_new_tuple(2, tuple);
    }
    else {
        mp_raise_msg(&mp_type_RuntimeError, "Unable to convert value to pulse. Calibration needs at least 2 points");
    }
    return mp_const_none;
}

mp_obj_t Calibration_pulse_to_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_pulse };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_pulse, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

    float pulse = mp_obj_get_float(args[ARG_pulse].u_obj);

    float value_out, pulse_out;
    if(self->calibration->pulse_to_value(pulse, value_out, pulse_out)) {
        mp_obj_t tuple[2];
        tuple[0] = mp_obj_new_float(pulse_out);
        tuple[1] = mp_obj_new_float(value_out);
        return mp_obj_new_tuple(2, tuple);
    }
    else {
        mp_raise_msg(&mp_type_RuntimeError, "Unable to convert pulse to value. Calibration needs at least 2 points");
    }
    return mp_const_none;
}


/********** Servo **********/

/***** Variables Struct *****/
typedef struct _Servo_obj_t {
    mp_obj_base_t base;
    Servo* servo;
} _Servo_obj_t;


/***** Print *****/
void Servo_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    mp_print_str(print, "Servo(");

    mp_print_str(print, "pin = ");
    mp_obj_print_helper(print, mp_obj_new_int(self->servo->get_pin()), PRINT_REPR);
    mp_print_str(print, ", enabled = ");
    mp_obj_print_helper(print, self->servo->is_enabled() ? mp_const_true : mp_const_false, PRINT_REPR);
    mp_print_str(print, ", pulse = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->servo->get_pulse()), PRINT_REPR);
    mp_print_str(print, ", value = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->servo->get_value()), PRINT_REPR);
    mp_print_str(print, ", freq = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->servo->get_frequency()), PRINT_REPR);

    mp_print_str(print, ")");
}


/***** Constructor *****/
mp_obj_t Servo_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _Servo_obj_t *self = nullptr;

    enum { ARG_pin, ARG_type, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_type, MP_ARG_INT, {.u_int = (uint8_t)servo::CalibrationType::ANGULAR} },
        { MP_QSTR_freq, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int pin = args[ARG_pin].u_int;
    servo::CalibrationType calibration_type = (servo::CalibrationType)args[ARG_type].u_int;

    float freq = servo::ServoState::DEFAULT_FREQUENCY;
    if(args[ARG_freq].u_obj != mp_const_none) {
        freq = mp_obj_get_float(args[ARG_freq].u_obj);
    }

    self = m_new_obj_with_finaliser(_Servo_obj_t);
    self->base.type = &Servo_type;

    self->servo = new Servo(pin, calibration_type, freq);
    self->servo->init();

    return MP_OBJ_FROM_PTR(self);
}


/***** Destructor ******/
mp_obj_t Servo___del__(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    delete self->servo;
    return mp_const_none;
}


/***** Methods *****/
extern mp_obj_t Servo_pin(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    return mp_obj_new_int(self->servo->get_pin());
}

extern mp_obj_t Servo_enable(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    self->servo->enable();
    return mp_const_none;
}

extern mp_obj_t Servo_disable(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    self->servo->disable();
    return mp_const_none;
}

extern mp_obj_t Servo_is_enabled(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    return self->servo->is_enabled() ? mp_const_true : mp_const_false;
}

extern mp_obj_t Servo_pulse(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 1) {
        enum { ARG_self };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Servo_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Servo_obj_t);

        return mp_obj_new_float(self->servo->get_pulse());
    }
    else {
        enum { ARG_self, ARG_pulse };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_pulse, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Servo_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Servo_obj_t);

        float pulse = mp_obj_get_float(args[ARG_pulse].u_obj);

        self->servo->set_pulse(pulse);
        return mp_const_none;
    }
}

extern mp_obj_t Servo_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 1) {
        enum { ARG_self };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Servo_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Servo_obj_t);

        return mp_obj_new_float(self->servo->get_value());
    }
    else {
        enum { ARG_self, ARG_value };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Servo_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Servo_obj_t);

        float value = mp_obj_get_float(args[ARG_value].u_obj);

        self->servo->set_value(value);
        return mp_const_none;
    }
}

extern mp_obj_t Servo_frequency(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 1) {
        enum { ARG_self };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Servo_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Servo_obj_t);

        return mp_obj_new_float(self->servo->get_frequency());
    }
    else {
        enum { ARG_self, ARG_freq };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_freq, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Servo_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Servo_obj_t);

        float freq = mp_obj_get_float(args[ARG_freq].u_obj);

        if(!self->servo->set_frequency(freq)) {
            mp_raise_ValueError("freq out of range. Expected 10Hz to 350Hz");
        }
        return mp_const_none;
    }
}

extern mp_obj_t Servo_min_value(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    return mp_obj_new_float(self->servo->get_min_value());
}

extern mp_obj_t Servo_mid_value(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    return mp_obj_new_float(self->servo->get_mid_value());
}

extern mp_obj_t Servo_max_value(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    return mp_obj_new_float(self->servo->get_max_value());
}

extern mp_obj_t Servo_to_min(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    self->servo->to_min();
    return mp_const_none;
}

extern mp_obj_t Servo_to_mid(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    self->servo->to_mid();
    return mp_const_none;
}

extern mp_obj_t Servo_to_max(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    self->servo->to_max();
    return mp_const_none;
}

extern mp_obj_t Servo_to_percent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 2) {
        enum { ARG_self, ARG_in };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Servo_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Servo_obj_t);

        float in = mp_obj_get_float(args[ARG_in].u_obj);

        self->servo->to_percent(in);
    }
    else if(n_args <= 4) {
        enum { ARG_self, ARG_in, ARG_in_min, ARG_in_max };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_min, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_max, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Servo_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Servo_obj_t);

        float in = mp_obj_get_float(args[ARG_in].u_obj);
        float in_min = mp_obj_get_float(args[ARG_in_min].u_obj);
        float in_max = mp_obj_get_float(args[ARG_in_max].u_obj);

        self->servo->to_percent(in, in_min, in_max);
    }
    else {
        enum { ARG_self, ARG_in, ARG_in_min, ARG_in_max, ARG_value_min, ARG_value_max };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_min, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_max, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_value_min, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_value_max, MP_ARG_REQUIRED | MP_ARG_OBJ }
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Servo_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Servo_obj_t);

        float in = mp_obj_get_float(args[ARG_in].u_obj);
        float in_min = mp_obj_get_float(args[ARG_in_min].u_obj);
        float in_max = mp_obj_get_float(args[ARG_in_max].u_obj);
        float value_min = mp_obj_get_float(args[ARG_value_min].u_obj);
        float value_max = mp_obj_get_float(args[ARG_value_max].u_obj);

        self->servo->to_percent(in, in_min, in_max, value_min, value_max);
    }

    return mp_const_none;
}

extern mp_obj_t Servo_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 1) {
        enum { ARG_self };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Servo_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Servo_obj_t);

        // Create a new MP Calibration instance and assign a copy of the servo's calibration to it
        _Calibration_obj_t *calib = m_new_obj_with_finaliser(_Calibration_obj_t);
        calib->base.type = &Calibration_type;

        calib->calibration = new Calibration(self->servo->calibration());
        return MP_OBJ_FROM_PTR(calib);
    }
    else {
        enum { ARG_self, ARG_calibration };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_calibration, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Servo_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Servo_obj_t);

        const mp_obj_t object = args[ARG_calibration].u_obj;
        if(mp_obj_is_type(object, &Calibration_type)) {
            _Calibration_obj_t *calib = MP_OBJ_TO_PTR2(object, _Calibration_obj_t);
            self->servo->calibration() = *(calib->calibration);
        }
        else {
            mp_raise_TypeError("cannot convert object to a Calibration class instance");
        }
    }

    return mp_const_none;


}


/********** ServoCluster **********/

/***** Variables Struct *****/
typedef struct _ServoCluster_obj_t {
    mp_obj_base_t base;
    ServoCluster* cluster;
} _ServoCluster_obj_t;


/***** Print *****/
void ServoCluster_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _ServoCluster_obj_t);
    mp_print_str(print, "ServoCluster(");

    mp_print_str(print, "servos = {");

    uint8_t servo_count = self->cluster->get_count();
    for(uint8_t servo = 0; servo < servo_count; servo++) {
        mp_print_str(print, "\n\t{pin = ");
        mp_obj_print_helper(print, mp_obj_new_int(self->cluster->get_pin(servo)), PRINT_REPR);
        mp_print_str(print, ", enabled = ");
        mp_obj_print_helper(print, self->cluster->is_enabled(servo) ? mp_const_true : mp_const_false, PRINT_REPR);
        mp_print_str(print, ", pulse = ");
        mp_obj_print_helper(print, mp_obj_new_float(self->cluster->get_pulse(servo)), PRINT_REPR);
        mp_print_str(print, ", value = ");
        mp_obj_print_helper(print, mp_obj_new_float(self->cluster->get_value(servo)), PRINT_REPR);
        mp_print_str(print, ", phase = ");
        mp_obj_print_helper(print, mp_obj_new_float(self->cluster->get_phase(servo)), PRINT_REPR);
        mp_print_str(print, "}");
        if(servo < servo_count - 1)
            mp_print_str(print, ", ");
    }
    if(servo_count > 0) {
        mp_print_str(print, "\n");
    }
    mp_print_str(print, "}, freq = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->cluster->get_frequency()), PRINT_REPR);

    mp_print_str(print, ")");
}


/***** Constructor *****/
mp_obj_t ServoCluster_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _ServoCluster_obj_t *self = nullptr;

    enum { ARG_pio, ARG_sm, ARG_pins, ARG_type, ARG_freq, ARG_auto_phase };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pio, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_sm, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pins, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_type, MP_ARG_INT, {.u_int = (uint8_t)servo::CalibrationType::ANGULAR} },
        { MP_QSTR_freq, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_auto_phase, MP_ARG_BOOL, {.u_bool = true} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    PIO pio = args[ARG_pio].u_int == 0 ? pio0 : pio1;
    int sm = args[ARG_sm].u_int;

    uint pin_mask = 0;
    bool mask_provided = true;
    uint32_t pin_count = 0;
    uint8_t* pins = nullptr;

    // Determine what pins this cluster will use
    const mp_obj_t object = args[ARG_pins].u_obj;
    if(mp_obj_is_int(object)) {
        pin_mask = (uint)mp_obj_get_int(object);
    }
    else {
        mp_obj_t *items = nullptr;
        if(mp_obj_is_type(object, &mp_type_list)) {
            mp_obj_list_t *list = MP_OBJ_TO_PTR2(object, mp_obj_list_t);
            pin_count = list->len;
            items = list->items;
        }
        else if(mp_obj_is_type(object, &mp_type_tuple)) {
            mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(object, mp_obj_tuple_t);
            pin_count = tuple->len;
            items = tuple->items;
        }

        if(items == nullptr)
            mp_raise_TypeError("cannot convert object to a list or tuple of pins, or a pin mask integer");
        else if(pin_count == 0)
            mp_raise_TypeError("list or tuple must contain at least one integer");
        else {
            // Create and populate a local array of pins
            uint8_t *pins = new uint8_t[pin_count];
            for(size_t i = 0; i < pin_count; i++) {
                int pin = mp_obj_get_int(items[i]);
                if(pin < 0 || pin >= (int)NUM_BANK0_GPIOS) {
                    delete[] pins;
                    mp_raise_ValueError("a pin in the list or tuple is out of range. Expected 0 to 29");
                }
                else {
                    pins[i] = (uint8_t)pin;
                }
            }
            mask_provided = false;
        }
    }

    servo::CalibrationType calibration_type = (servo::CalibrationType)args[ARG_type].u_int;

    float freq = servo::ServoState::DEFAULT_FREQUENCY;
    if(args[ARG_freq].u_obj != mp_const_none) {
        freq = mp_obj_get_float(args[ARG_freq].u_obj);
    }

    bool auto_phase = args[ARG_auto_phase].u_bool;

    self = m_new_obj_with_finaliser(_ServoCluster_obj_t);
    self->base.type = &ServoCluster_type;

    if(mask_provided)
        self->cluster = new ServoCluster(pio, sm, pin_mask, calibration_type, freq, auto_phase);
    else
        self->cluster = new ServoCluster(pio, sm, pins, pin_count, calibration_type, freq, auto_phase);
    self->cluster->init();

    // Cleanup the pins array
    if(pins != nullptr)
        delete[] pins;

    return MP_OBJ_FROM_PTR(self);
}


/***** Destructor ******/
mp_obj_t ServoCluster___del__(mp_obj_t self_in) {
    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _ServoCluster_obj_t);
    delete self->cluster;
    return mp_const_none;
}


/***** Methods *****/
extern mp_obj_t ServoCluster_count(mp_obj_t self_in) {
    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _ServoCluster_obj_t);
    return mp_obj_new_int(self->cluster->get_count());
}

extern mp_obj_t ServoCluster_pin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_servo };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

    int servo = args[ARG_servo].u_int;
    int servo_count = (int)self->cluster->get_count();
    if(servo_count == 0)
        mp_raise_ValueError("this cluster does not have any servos");
    else if(servo < 0 || servo >= servo_count)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
    else
        return mp_obj_new_int(self->cluster->get_pin((uint)servo));

    return mp_const_none;
}

extern mp_obj_t ServoCluster_enable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_servos, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_servos, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

    int servo_count = (int)self->cluster->get_count();
    if(servo_count == 0)
        mp_raise_ValueError("this cluster does not have any servos");
    else {
        // Determine what servo(s) to enable
        const mp_obj_t object = args[ARG_servos].u_obj;
        if(mp_obj_is_int(object)) {
            int servo = mp_obj_get_int(object);
            if(servo < 0 || servo >= servo_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
            else
                self->cluster->enable((uint)servo, args[ARG_load].u_bool);
        }
        else {
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
                mp_raise_TypeError("cannot convert object to a list or tuple of integers, or a single integer");
            else if(length == 0)
                mp_raise_TypeError("list or tuple must contain at least one integer");
            else {
                // Create and populate a local array of servo indices
                uint8_t *servos = new uint8_t[length];
                for(size_t i = 0; i < length; i++) {
                    int servo = mp_obj_get_int(items[i]);
                    if(servo < 0 || servo >= servo_count) {
                        delete[] servos;
                        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("a servo in the list or tuple is out of range. Expected 0 to %d"), servo_count - 1);
                    }
                    else {
                        servos[i] = (uint8_t)servo;
                    }
                }
                self->cluster->enable(servos, length, args[ARG_load].u_bool);
                delete[] servos;
            }
        }
    }

    return mp_const_none;
}

extern mp_obj_t ServoCluster_enable_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);
    self->cluster->enable_all(args[ARG_load].u_bool);

    return mp_const_none;
}

extern mp_obj_t ServoCluster_disable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_servos, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_servos, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

    int servo_count = (int)self->cluster->get_count();
    if(servo_count == 0)
        mp_raise_ValueError("this cluster does not have any servos");
    else {
        // Determine what servo(s) to disable
        const mp_obj_t object = args[ARG_servos].u_obj;
        if(mp_obj_is_int(object)) {
            int servo = mp_obj_get_int(object);
            if(servo < 0 || servo >= servo_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
            else
                self->cluster->disable((uint)servo, args[ARG_load].u_bool);
        }
        else {
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
                mp_raise_TypeError("cannot convert object to a list or tuple of integers, or a single integer");
            else if(length == 0)
                mp_raise_TypeError("list or tuple must contain at least one integer");
            else {
                // Create and populate a local array of servo indices
                uint8_t *servos = new uint8_t[length];
                for(size_t i = 0; i < length; i++) {
                    int servo = mp_obj_get_int(items[i]);
                    if(servo < 0 || servo >= servo_count) {
                        delete[] servos;
                        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("a servo in the list or tuple is out of range. Expected 0 to %d"), servo_count - 1);
                    }
                    else {
                        servos[i] = (uint8_t)servo;
                    }
                }
                self->cluster->disable(servos, length, args[ARG_load].u_bool);
                delete[] servos;
            }
        }
    }

    return mp_const_none;
}

extern mp_obj_t ServoCluster_disable_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);
    self->cluster->disable_all(args[ARG_load].u_bool);

    return mp_const_none;
}

extern mp_obj_t ServoCluster_is_enabled(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_servo };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

    int servo = args[ARG_servo].u_int;
    int servo_count = (int)self->cluster->get_count();
    if(servo_count == 0)
        mp_raise_ValueError("this cluster does not have any servos");
    else if(servo < 0 || servo >= servo_count)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
    else
        return self->cluster->is_enabled((uint)servo) ? mp_const_true : mp_const_false;

    return mp_const_none;
}

extern mp_obj_t ServoCluster_pulse(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 2) {
        enum { ARG_self, ARG_servo };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

        int servo = args[ARG_servo].u_int;
        int servo_count = (int)self->cluster->get_count();
        if(servo_count == 0)
            mp_raise_ValueError("this cluster does not have any servos");
        else if(servo < 0 || servo >= servo_count)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
        else
            return mp_obj_new_float(self->cluster->get_pulse((uint)servo));
    }
    else {
        enum { ARG_self, ARG_servo, ARG_pulse, ARG_load };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_pulse, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

        int servo = args[ARG_servo].u_int;
        int servo_count = (int)self->cluster->get_count();
        if(servo_count == 0)
            mp_raise_ValueError("this cluster does not have any servos");
        else if(servo < 0 || servo >= servo_count)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
        else {
            float pulse = mp_obj_get_float(args[ARG_pulse].u_obj);
            self->cluster->set_pulse((uint)servo, pulse, args[ARG_load].u_bool);
        }
    }
    return mp_const_none;
}

extern mp_obj_t ServoCluster_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 2) {
        enum { ARG_self, ARG_servo };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

        int servo = args[ARG_servo].u_int;
        int servo_count = (int)self->cluster->get_count();
        if(servo_count == 0)
            mp_raise_ValueError("this cluster does not have any servos");
        else if(servo < 0 || servo >= servo_count)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
        else
            return mp_obj_new_float(self->cluster->get_value((uint)servo));
    }
    else {
        enum { ARG_self, ARG_servo, ARG_value, ARG_load };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

        int servo = args[ARG_servo].u_int;
        int servo_count = (int)self->cluster->get_count();
        if(servo_count == 0)
            mp_raise_ValueError("this cluster does not have any servos");
        else if(servo < 0 || servo >= servo_count)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
        else {
            float value = mp_obj_get_float(args[ARG_value].u_obj);
            self->cluster->set_value((uint)servo, value, args[ARG_load].u_bool);
        }
    }
    return mp_const_none;
}

extern mp_obj_t ServoCluster_phase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 2) {
        enum { ARG_self, ARG_servo };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

        int servo = args[ARG_servo].u_int;
        int servo_count = (int)self->cluster->get_count();
        if(servo_count == 0)
            mp_raise_ValueError("this cluster does not have any servos");
        else if(servo < 0 || servo >= servo_count)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
        else
            return mp_obj_new_float(self->cluster->get_phase((uint)servo));
    }
    else {
        enum { ARG_self, ARG_servo, ARG_phase, ARG_load };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_phase, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

        int servo = args[ARG_servo].u_int;
        int servo_count = (int)self->cluster->get_count();
        if(servo_count == 0)
            mp_raise_ValueError("this cluster does not have any servos");
        else if(servo < 0 || servo >= servo_count)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
        else {
            float phase = mp_obj_get_float(args[ARG_phase].u_obj);
            self->cluster->set_phase((uint)servo, phase, args[ARG_load].u_bool);
        }
    }
    return mp_const_none;
}

extern mp_obj_t ServoCluster_frequency(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 1) {
        enum { ARG_self };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

        return mp_obj_new_float(self->cluster->get_frequency());
    }
    else {
        enum { ARG_self, ARG_freq };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_freq, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

        float freq = mp_obj_get_float(args[ARG_freq].u_obj);

        if(!self->cluster->set_frequency(freq))
            mp_raise_ValueError("freq out of range. Expected 10Hz to 350Hz");
        else
            return mp_const_none;
    }
}

extern mp_obj_t ServoCluster_min_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_servo };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

    int servo = args[ARG_servo].u_int;
    int servo_count = (int)self->cluster->get_count();
    if(servo_count == 0)
        mp_raise_ValueError("this cluster does not have any servos");
    else if(servo < 0 || servo >= servo_count)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
    else
        return mp_obj_new_float(self->cluster->get_min_value((uint)servo));

    return mp_const_none;
}

extern mp_obj_t ServoCluster_mid_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_servo };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

    int servo = args[ARG_servo].u_int;
    int servo_count = (int)self->cluster->get_count();
    if(servo_count == 0)
        mp_raise_ValueError("this cluster does not have any servos");
    else if(servo < 0 || servo >= servo_count)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
    else
        return mp_obj_new_float(self->cluster->get_mid_value((uint)servo));

    return mp_const_none;
}

extern mp_obj_t ServoCluster_max_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_servo };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

    int servo = args[ARG_servo].u_int;
    int servo_count = (int)self->cluster->get_count();
    if(servo_count == 0)
        mp_raise_ValueError("this cluster does not have any servos");
    else if(servo < 0 || servo >= servo_count)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
    else
        return mp_obj_new_float(self->cluster->get_max_value((uint)servo));

    return mp_const_none;
}

extern mp_obj_t ServoCluster_to_min(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_servo, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

    int servo = args[ARG_servo].u_int;
    int servo_count = (int)self->cluster->get_count();
    if(servo_count == 0)
        mp_raise_ValueError("this cluster does not have any servos");
    else if(servo < 0 || servo >= servo_count)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
    else
        self->cluster->to_min((uint)servo, args[ARG_load].u_bool);

    return mp_const_none;
}

extern mp_obj_t ServoCluster_to_mid(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_servo, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

    int servo = args[ARG_servo].u_int;
    int servo_count = (int)self->cluster->get_count();
    if(servo_count == 0)
        mp_raise_ValueError("this cluster does not have any servos");
    else if(servo < 0 || servo >= servo_count)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
    else
        self->cluster->to_mid((uint)servo, args[ARG_load].u_bool);

    return mp_const_none;
}

extern mp_obj_t ServoCluster_to_max(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_servo, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

    int servo = args[ARG_servo].u_int;
    int servo_count = (int)self->cluster->get_count();
    if(servo_count == 0)
        mp_raise_ValueError("this cluster does not have any servos");
    else if(servo < 0 || servo >= servo_count)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
    else
        self->cluster->to_max((uint)servo, args[ARG_load].u_bool);

    return mp_const_none;
}

extern mp_obj_t ServoCluster_to_percent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 4) {
        enum { ARG_self, ARG_servo, ARG_in, ARG_load };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_in, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

        int servo = args[ARG_servo].u_int;
        int servo_count = (int)self->cluster->get_count();
        if(servo_count == 0)
            mp_raise_ValueError("this cluster does not have any servos");
        else if(servo < 0 || servo >= servo_count)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
        else {
            float in = mp_obj_get_float(args[ARG_in].u_obj);
            self->cluster->to_percent((uint)servo, in, args[ARG_load].u_bool);
        }
    }
    else if(n_args <= 6) {
        enum { ARG_self, ARG_servo, ARG_in, ARG_in_min, ARG_in_max, ARG_load };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_in, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_min, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_max, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

        int servo = args[ARG_servo].u_int;
        int servo_count = (int)self->cluster->get_count();
        if(servo_count == 0)
            mp_raise_ValueError("this cluster does not have any servos");
        else if(servo < 0 || servo >= servo_count)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
        else {
            float in = mp_obj_get_float(args[ARG_in].u_obj);
            float in_min = mp_obj_get_float(args[ARG_in_min].u_obj);
            float in_max = mp_obj_get_float(args[ARG_in_max].u_obj);
            self->cluster->to_percent((uint)servo, in, in_min, in_max, args[ARG_load].u_bool);
        }
    }
    else {
        enum { ARG_self, ARG_servo, ARG_in, ARG_in_min, ARG_in_max, ARG_value_min, ARG_value_max, ARG_load };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_in, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_min, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_max, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_value_min, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_value_max, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

        int servo = args[ARG_servo].u_int;
        int servo_count = (int)self->cluster->get_count();
        if(servo_count == 0)
            mp_raise_ValueError("this cluster does not have any servos");
        else if(servo < 0 || servo >= servo_count)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
        else {
            float in = mp_obj_get_float(args[ARG_in].u_obj);
            float in_min = mp_obj_get_float(args[ARG_in_min].u_obj);
            float in_max = mp_obj_get_float(args[ARG_in_max].u_obj);
            float value_min = mp_obj_get_float(args[ARG_value_min].u_obj);
            float value_max = mp_obj_get_float(args[ARG_value_max].u_obj);
            self->cluster->to_percent((uint)servo, in, in_min, in_max, value_min, value_max, args[ARG_load].u_bool);
        }
    }
    return mp_const_none;
}

extern mp_obj_t ServoCluster_calibration(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 2) {
        enum { ARG_self, ARG_servo };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

        int servo = args[ARG_servo].u_int;
        int servo_count = (int)self->cluster->get_count();
        if(servo_count == 0)
            mp_raise_ValueError("this cluster does not have any servos");
        else if(servo < 0 || servo >= servo_count)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("servo out of range. Expected 0 to %d"), servo_count - 1);
        else {
            // Create a new MP Calibration instance and assign a copy of the servo's calibration to it
            _Calibration_obj_t *calib = m_new_obj_with_finaliser(_Calibration_obj_t);
            calib->base.type = &Calibration_type;

            calib->calibration = new Calibration(self->cluster->calibration((uint)servo));
            return MP_OBJ_FROM_PTR(calib);
        }
    }
    else {
        enum { ARG_self, ARG_servo, ARG_calibration };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_servo, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_calibration, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);

        int servo = args[ARG_servo].u_int;
        const mp_obj_t object = args[ARG_calibration].u_obj;
        if(mp_obj_is_type(object, &Calibration_type)) {
            _Calibration_obj_t *calib = MP_OBJ_TO_PTR2(object, _Calibration_obj_t);
            self->cluster->calibration((uint)servo) = *(calib->calibration);
        }
        else {
            mp_raise_TypeError("cannot convert object to a Calibration class instance");
        }
    }

    return mp_const_none;
}

extern mp_obj_t ServoCluster_load(mp_obj_t self_in) {
    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _ServoCluster_obj_t);
    self->cluster->load();
    return mp_const_none;
}
}
