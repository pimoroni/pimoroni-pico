#include "drivers/servo/servo.hpp"
#include "common/pimoroni_common.hpp"
#include "micropython/modules/util.hpp"
#include "servo_common.hpp"
#include <cstdio>


using namespace pimoroni;
using namespace servo;

extern "C" {
#include "py/builtin.h"


/********** Calibration **********/


/***** Print *****/
void Calibration_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _Calibtration_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Calibtration_obj_t);
    Calibration* calib = self->calibration;
    mp_print_str(print, "Calibration(");

    uint size = calib->size();
    mp_print_str(print, "size = ");
    mp_obj_print_helper(print, mp_obj_new_int(size), PRINT_REPR);

    mp_print_str(print, ", pairs = {");
    for(uint i = 0; i < size; i++) {
        const Calibration::Pair &pair = calib->pair(i);
        mp_print_str(print, "{");
        mp_obj_print_helper(print, mp_obj_new_float(pair.pulse), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_float(pair.value), PRINT_REPR);
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
        { MP_QSTR_type, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t object = args[ARG_type].u_obj;
    if(object != mp_const_none) {
        if(mp_obj_is_int(object)) {
            int type = mp_obj_get_int(object);
            if(type < 0 || type >= 3) {
                mp_raise_ValueError(MP_ERROR_TEXT("type out of range. Expected ANGULAR (0), LINEAR (1) or CONTINUOUS (2)"));
            }
            servo::CalibrationType calibration_type = (servo::CalibrationType)type;

            self = mp_obj_malloc_with_finaliser(_Calibtration_obj_t, &Calibration_type);
            self->calibration = m_new_class(Calibration, calibration_type);
        }
        else {
            mp_raise_TypeError(MP_ERROR_TEXT("cannot convert object to an integer"));
        }
    }
    else {
        self = mp_obj_malloc_with_finaliser(_Calibtration_obj_t, &Calibration_type);
        self->calibration = m_new_class(Calibration);
    }

    return MP_OBJ_FROM_PTR(self);
}


/***** Destructor ******/
mp_obj_t Calibration___del__(mp_obj_t self_in) {
    _Calibtration_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Calibtration_obj_t);
    m_del_class(Calibration, self->calibration);
    return mp_const_none;
}


/***** Methods *****/
mp_obj_t Calibration_apply_blank_pairs(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
        mp_raise_ValueError(MP_ERROR_TEXT("size out of range. Expected 0 or greater"));
    else
        self->calibration->apply_blank_pairs((uint)size);

    return mp_const_none;
}

mp_obj_t Calibration_apply_two_pairs(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
    self->calibration->apply_two_pairs(min_pulse, max_pulse, min_value, max_value);

    return mp_const_none;
}

mp_obj_t Calibration_apply_three_pairs(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
    self->calibration->apply_three_pairs(min_pulse, mid_pulse, max_pulse, min_value, mid_value, max_value);

    return mp_const_none;
}

mp_obj_t Calibration_apply_uniform_pairs(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
        mp_raise_ValueError(MP_ERROR_TEXT("size out of range. Expected 0 or greater"));
    else {
        float min_pulse = mp_obj_get_float(args[ARG_min_pulse].u_obj);
        float max_pulse = mp_obj_get_float(args[ARG_max_pulse].u_obj);
        float min_value = mp_obj_get_float(args[ARG_min_value].u_obj);
        float max_value = mp_obj_get_float(args[ARG_max_value].u_obj);
        self->calibration->apply_uniform_pairs((uint)size, min_pulse, max_pulse, min_value, max_value);
    }

    return mp_const_none;
}

mp_obj_t Calibration_apply_default_pairs(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_type };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_type, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

    int type = args[ARG_type].u_int;
    if(type < 0 || type >= 3) {
        mp_raise_ValueError(MP_ERROR_TEXT("type out of range. Expected ANGULAR (0), LINEAR (1) or CONTINUOUS (2)"));
    }
    servo::CalibrationType calibration_type = (servo::CalibrationType)type;
    self->calibration->apply_default_pairs(calibration_type);

    return mp_const_none;
}

mp_obj_t Calibration_size(mp_obj_t self_in) {
    _Calibration_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Calibration_obj_t);
    return mp_obj_new_int(self->calibration->size());
}

mp_obj_t Calibration_pair(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        if(index < 0 || index >= calibration_size)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("index out of range. Expected 0 to %d"), calibration_size - 1);
        else {
            const Calibration::Pair &pair = self->calibration->pair((uint)index);

            mp_obj_t list = mp_obj_new_list(0, NULL);
            mp_obj_list_append(list, mp_obj_new_float(pair.pulse));
            mp_obj_list_append(list, mp_obj_new_float(pair.value));
            return list;
        }
    }
    else {
        enum { ARG_self, ARG_index, ARG_pair };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_pair, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        int index = args[ARG_index].u_int;
        int calibration_size = (int)self->calibration->size();
        if(calibration_size == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        if(index < 0 || index >= calibration_size)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("index out of range. Expected 0 to %d"), calibration_size - 1);
        else {
            Calibration::Pair &pair = self->calibration->pair((uint)index);

            const mp_obj_t object = args[ARG_pair].u_obj;
            if(mp_obj_is_type(object, &mp_type_list)) {
                mp_obj_list_t *list = MP_OBJ_TO_PTR2(object, mp_obj_list_t);
                if(list->len == 2) {
                    pair.pulse = mp_obj_get_float(list->items[0]);
                    pair.value = mp_obj_get_float(list->items[1]);
                }
                else {
                    mp_raise_ValueError(MP_ERROR_TEXT("list must contain two numbers"));
                }
            }
            else if(mp_obj_is_type(object, &mp_type_tuple)) {
                mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(object, mp_obj_tuple_t);
                if(tuple->len == 2) {
                    pair.pulse = mp_obj_get_float(tuple->items[0]);
                    pair.value = mp_obj_get_float(tuple->items[1]);
                }
                else {
                    mp_raise_ValueError(MP_ERROR_TEXT("tuple must contain two numbers"));
                }
            }
            else {
                mp_raise_TypeError(MP_ERROR_TEXT("can't convert object to list or tuple"));
            }
        }
    }

    return mp_const_none;
}

mp_obj_t Calibration_pulse(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        if(index < 0 || index >= calibration_size)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("index out of range. Expected 0 to %d"), calibration_size - 1);
        else {
            return mp_obj_new_float(self->calibration->pulse((uint)index));
        }
    }
    else {
        enum { ARG_self, ARG_index, ARG_pulse };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_pulse, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        int index = args[ARG_index].u_int;
        int calibration_size = (int)self->calibration->size();
        if(calibration_size == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        if(index < 0 || index >= calibration_size)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("index out of range. Expected 0 to %d"), calibration_size - 1);
        else {
            self->calibration->pulse((uint)index, mp_obj_get_float(args[ARG_pulse].u_obj));
        }
    }

    return mp_const_none;
}

mp_obj_t Calibration_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        if(index < 0 || index >= calibration_size)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("index out of range. Expected 0 to %d"), calibration_size - 1);
        else {
            return mp_obj_new_float(self->calibration->value((uint)index));
        }
    }
    else {
        enum { ARG_self, ARG_index, ARG_value };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
            { MP_QSTR_value, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        int index = args[ARG_index].u_int;
        int calibration_size = (int)self->calibration->size();
        if(calibration_size == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        if(index < 0 || index >= calibration_size)
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("index out of range. Expected 0 to %d"), calibration_size - 1);
        else {
            self->calibration->value((uint)index, mp_obj_get_float(args[ARG_value].u_obj));
        }
    }

    return mp_const_none;
}

mp_obj_t Calibration_first(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 1) {
        enum { ARG_self };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        if(self->calibration->size() == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        else {
            const Calibration::Pair &pair = self->calibration->first();

            mp_obj_t list = mp_obj_new_list(0, NULL);
            mp_obj_list_append(list, mp_obj_new_float(pair.pulse));
            mp_obj_list_append(list, mp_obj_new_float(pair.value));
            return list;
        }
    }
    else {
        enum { ARG_self, ARG_pair };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_pair, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        if(self->calibration->size() == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        else {
            Calibration::Pair &pair = self->calibration->first();

            const mp_obj_t object = args[ARG_pair].u_obj;
            if(mp_obj_is_type(object, &mp_type_list)) {
                mp_obj_list_t *list = MP_OBJ_TO_PTR2(object, mp_obj_list_t);
                if(list->len == 2) {
                    pair.pulse = mp_obj_get_float(list->items[0]);
                    pair.value = mp_obj_get_float(list->items[1]);
                }
                else {
                    mp_raise_ValueError(MP_ERROR_TEXT("list must contain two numbers"));
                }
            }
            else if(mp_obj_is_type(object, &mp_type_tuple)) {
                mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(object, mp_obj_tuple_t);
                if(tuple->len == 2) {
                    pair.pulse = mp_obj_get_float(tuple->items[0]);
                    pair.value = mp_obj_get_float(tuple->items[1]);
                }
                else {
                    mp_raise_ValueError(MP_ERROR_TEXT("tuple must contain two numbers"));
                }
            }
            else {
                mp_raise_TypeError(MP_ERROR_TEXT("can't convert object to list or tuple"));
            }
        }
    }

    return mp_const_none;
}

mp_obj_t Calibration_first_pulse(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 1) {
        enum { ARG_self };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        if(self->calibration->size() == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        else {
            return mp_obj_new_float(self->calibration->first_pulse());
        }
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

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        if(self->calibration->size() == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        else {
            self->calibration->first_pulse(mp_obj_get_float(args[ARG_pulse].u_obj));
        }
    }

    return mp_const_none;
}

mp_obj_t Calibration_first_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 1) {
        enum { ARG_self };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        if(self->calibration->size() == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        else {
            return mp_obj_new_float(self->calibration->first_value());
        }
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

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        if(self->calibration->size() == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        else {
            self->calibration->first_value(mp_obj_get_float(args[ARG_value].u_obj));
        }
    }

    return mp_const_none;
}

mp_obj_t Calibration_last(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 1) {
        enum { ARG_self };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        if(self->calibration->size() == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        else {
            const Calibration::Pair &pair = self->calibration->last();

            mp_obj_t list = mp_obj_new_list(0, NULL);
            mp_obj_list_append(list, mp_obj_new_float(pair.pulse));
            mp_obj_list_append(list, mp_obj_new_float(pair.value));
            return list;
        }
    }
    else {
        enum { ARG_self, ARG_pair };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_pair, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        if(self->calibration->size() == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        else {
            Calibration::Pair &pair = self->calibration->last();

            const mp_obj_t object = args[ARG_pair].u_obj;
            if(mp_obj_is_type(object, &mp_type_list)) {
                mp_obj_list_t *list = MP_OBJ_TO_PTR2(object, mp_obj_list_t);
                if(list->len == 2) {
                    pair.pulse = mp_obj_get_float(list->items[0]);
                    pair.value = mp_obj_get_float(list->items[1]);
                }
                else {
                    mp_raise_ValueError(MP_ERROR_TEXT("list must contain two numbers"));
                }
            }
            else if(mp_obj_is_type(object, &mp_type_tuple)) {
                mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(object, mp_obj_tuple_t);
                if(tuple->len == 2) {
                    pair.pulse = mp_obj_get_float(tuple->items[0]);
                    pair.value = mp_obj_get_float(tuple->items[1]);
                }
                else {
                    mp_raise_ValueError(MP_ERROR_TEXT("tuple must contain two numbers"));
                }
            }
            else {
                mp_raise_TypeError(MP_ERROR_TEXT("can't convert object to list or tuple"));
            }
        }
    }

    return mp_const_none;
}

mp_obj_t Calibration_last_pulse(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 1) {
        enum { ARG_self };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        if(self->calibration->size() == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        else {
            return mp_obj_new_float(self->calibration->last_pulse());
        }
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

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        if(self->calibration->size() == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        else {
            self->calibration->last_pulse(mp_obj_get_float(args[ARG_pulse].u_obj));
        }
    }

    return mp_const_none;
}

mp_obj_t Calibration_last_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 1) {
        enum { ARG_self };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        if(self->calibration->size() == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        else {
            return mp_obj_new_float(self->calibration->last_value());
        }
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

        _Calibration_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Calibration_obj_t);

        if(self->calibration->size() == 0)
            mp_raise_ValueError(MP_ERROR_TEXT("this calibration does not have any pairs"));
        else {
            self->calibration->last_value(mp_obj_get_float(args[ARG_value].u_obj));
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
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Unable to convert value to pulse. Calibration needs at least 2 pairs"));
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
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Unable to convert pulse to value. Calibration needs at least 2 pairs"));
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
    mp_obj_print_helper(print, mp_obj_new_int(self->servo->pin()), PRINT_REPR);
    mp_print_str(print, ", enabled = ");
    mp_obj_print_helper(print, self->servo->is_enabled() ? mp_const_true : mp_const_false, PRINT_REPR);
    mp_print_str(print, ", pulse = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->servo->pulse()), PRINT_REPR);
    mp_print_str(print, ", value = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->servo->value()), PRINT_REPR);
    mp_print_str(print, ", freq = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->servo->frequency()), PRINT_REPR);

    mp_print_str(print, ")");
}


/***** Constructor *****/
mp_obj_t Servo_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _Servo_obj_t *self = nullptr;

    enum { ARG_pin, ARG_calibration, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_calibration, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_freq, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int pin = args[ARG_pin].u_int;

    servo::Calibration *calib = nullptr;
    servo::CalibrationType calibration_type = servo::CalibrationType::ANGULAR;
    const mp_obj_t calib_object = args[ARG_calibration].u_obj;
    if(calib_object != mp_const_none) {
        if(mp_obj_is_int(calib_object)) {
            int type = mp_obj_get_int(calib_object);
            if(type < 0 || type >= 3) {
                mp_raise_ValueError(MP_ERROR_TEXT("type out of range. Expected ANGULAR (0), LINEAR (1) or CONTINUOUS (2)"));
            }
            calibration_type = (servo::CalibrationType)type;
        }
        else if(mp_obj_is_type(calib_object, &Calibration_type)) {
            calib = (MP_OBJ_TO_PTR2(calib_object, _Calibration_obj_t)->calibration);
        }
        else {
            mp_raise_TypeError(MP_ERROR_TEXT("cannot convert object to an integer or a Calibration class instance"));
        }
    }

    float freq = servo::ServoState::DEFAULT_FREQUENCY;
    if(args[ARG_freq].u_obj != mp_const_none) {
        freq = mp_obj_get_float(args[ARG_freq].u_obj);
    }

    self = mp_obj_malloc_with_finaliser(_Servo_obj_t, &Servo_type);

    void *servo_target = m_new(Servo, 1);

    if(calib != nullptr)
        self->servo = new(servo_target) Servo(pin, *calib, freq);
    else
        self->servo = new(servo_target) Servo(pin, calibration_type, freq);

    self->servo->init();

    return MP_OBJ_FROM_PTR(self);
}


/***** Destructor ******/
mp_obj_t Servo___del__(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    m_del_class(Servo, self->servo);
    return mp_const_none;
}


/***** Methods *****/
extern mp_obj_t Servo_pin(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    return mp_obj_new_int(self->servo->pin());
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

        return mp_obj_new_float(self->servo->pulse());
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

        self->servo->pulse(pulse);
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

        return mp_obj_new_float(self->servo->value());
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

        self->servo->value(value);
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

        return mp_obj_new_float(self->servo->frequency());
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

        if(!self->servo->frequency(freq)) {
            mp_raise_ValueError(MP_ERROR_TEXT("freq out of range. Expected 10Hz to 350Hz"));
        }
        return mp_const_none;
    }
}

extern mp_obj_t Servo_min_value(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    return mp_obj_new_float(self->servo->min_value());
}

extern mp_obj_t Servo_mid_value(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    return mp_obj_new_float(self->servo->mid_value());
}

extern mp_obj_t Servo_max_value(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    return mp_obj_new_float(self->servo->max_value());
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
        _Calibration_obj_t *calib = mp_obj_malloc_with_finaliser(_Calibration_obj_t, &Calibration_type);

        calib->calibration = m_new_class(Calibration, self->servo->calibration());
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
            mp_raise_TypeError(MP_ERROR_TEXT("cannot convert object to a Calibration class instance"));
        }
    }

    return mp_const_none;


}


}
