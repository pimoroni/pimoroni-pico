#include "drivers/motor/motor.hpp"
#include "common/pimoroni_common.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>


using namespace pimoroni;
using namespace motor;

extern "C" {
#include "motor.h"
#include "py/builtin.h"
#include "float.h"



/********** Motor **********/

/***** Variables Struct *****/
typedef struct _Motor_obj_t {
    mp_obj_base_t base;
    Motor* motor;
} _Motor_obj_t;


/***** Print *****/
void Motor_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _Motor_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Motor_obj_t);
    mp_print_str(print, "Motor(");

    mp_print_str(print, "pins = (");
    pin_pair pins = self->motor->pins();
    mp_obj_print_helper(print, mp_obj_new_int(pins.positive), PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, mp_obj_new_int(pins.negative), PRINT_REPR);
    mp_print_str(print, "), enabled = ");
    mp_obj_print_helper(print, self->motor->is_enabled() ? mp_const_true : mp_const_false, PRINT_REPR);
    mp_print_str(print, ", duty = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->motor->duty()), PRINT_REPR);
    mp_print_str(print, ", speed = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->motor->speed()), PRINT_REPR);
    mp_print_str(print, ", freq = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->motor->frequency()), PRINT_REPR);
    if(self->motor->direction() == NORMAL_DIR)
        mp_print_str(print, ", direction = NORMAL_DIR");
    else
        mp_print_str(print, ", direction = REVERSED_DIR");
    mp_print_str(print, ", speed_scale = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->motor->speed_scale()), PRINT_REPR);
    mp_print_str(print, ", zeropoint = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->motor->zeropoint()), PRINT_REPR);
    mp_print_str(print, ", deadzone = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->motor->deadzone()), PRINT_REPR);
    if(self->motor->decay_mode() == SLOW_DECAY)
        mp_print_str(print, ", decay_mode = SLOW_DECAY");
    else
        mp_print_str(print, ", decay_mode = FAST_DECAY");

    mp_print_str(print, ")");
}


/***** Constructor *****/
mp_obj_t Motor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _Motor_obj_t *self = nullptr;

    enum { ARG_pins, ARG_direction, ARG_speed_scale, ARG_zeropoint, ARG_deadzone, ARG_freq, ARG_mode, ARG_ph_en_driver };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pins, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_direction, MP_ARG_INT, {.u_int = NORMAL_DIR} },
        { MP_QSTR_speed_scale, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_zeropoint, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_deadzone, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_freq, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_mode, MP_ARG_INT, {.u_int = MotorState::DEFAULT_DECAY_MODE} },
        { MP_QSTR_ph_en_driver, MP_ARG_BOOL, {.u_bool = false} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t pin_count = 0;
    pin_pair pins;

    // Determine what pair of pins this motor will use
    const mp_obj_t object = args[ARG_pins].u_obj;
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
        mp_raise_TypeError(MP_ERROR_TEXT("cannot convert object to a list or tuple of pins"));
    else if(pin_count != 2)
        mp_raise_TypeError(MP_ERROR_TEXT("list or tuple must only contain two integers"));
    else {
        int pos = mp_obj_get_int(items[0]);
        int neg = mp_obj_get_int(items[1]);
        if((pos < 0 || pos >= (int)NUM_BANK0_GPIOS) ||
           (neg < 0 || neg >= (int)NUM_BANK0_GPIOS)) {
            mp_raise_ValueError(MP_ERROR_TEXT("a pin in the list or tuple is out of range. Expected 0 to 29"));
        }
        else if(pos == neg) {
            mp_raise_ValueError(MP_ERROR_TEXT("cannot use the same pin for motor positive and negative"));
        }

        pins.positive = (uint8_t)pos;
        pins.negative = (uint8_t)neg;
    }

    int direction = args[ARG_direction].u_int;
    if(direction < 0 || direction > 1) {
        mp_raise_ValueError(MP_ERROR_TEXT("direction out of range. Expected NORMAL_DIR (0) or REVERSED_DIR (1)"));
    }

    float speed_scale = MotorState::DEFAULT_SPEED_SCALE;
    if(args[ARG_speed_scale].u_obj != mp_const_none) {
        speed_scale = mp_obj_get_float(args[ARG_speed_scale].u_obj);
        if(speed_scale < FLT_EPSILON) {
            mp_raise_ValueError(MP_ERROR_TEXT("speed_scale out of range. Expected greater than 0.0"));
        }
    }

    float zeropoint = MotorState::DEFAULT_ZEROPOINT;
    if(args[ARG_zeropoint].u_obj != mp_const_none) {
        zeropoint = mp_obj_get_float(args[ARG_zeropoint].u_obj);
        if(zeropoint < 0.0f || zeropoint > 1.0f - FLT_EPSILON) {
            mp_raise_ValueError(MP_ERROR_TEXT("zeropoint out of range. Expected 0.0 to less than 1.0"));
        }
    }

    float deadzone = MotorState::DEFAULT_DEADZONE;
    if(args[ARG_deadzone].u_obj != mp_const_none) {
        deadzone = mp_obj_get_float(args[ARG_deadzone].u_obj);
        if(deadzone < 0.0f || deadzone > 1.0f) {
            mp_raise_ValueError(MP_ERROR_TEXT("deadzone out of range. Expected 0.0 to 1.0"));
        }
    }

    float freq = MotorState::DEFAULT_FREQUENCY;
    if(args[ARG_freq].u_obj != mp_const_none) {
        freq = mp_obj_get_float(args[ARG_freq].u_obj);
    }

    int mode = args[ARG_mode].u_int;
    if(mode < 0 || mode > 1) {
        mp_raise_ValueError(MP_ERROR_TEXT("mode out of range. Expected FAST_DECAY (0) or SLOW_DECAY (1)"));
    }

    self = mp_obj_malloc_with_finaliser(_Motor_obj_t, &Motor_type);

    self->motor = m_new_class(Motor, pins, (Direction)direction, speed_scale, zeropoint, deadzone, freq, (DecayMode)mode, args[ARG_ph_en_driver].u_bool);
    self->motor->init();

    return MP_OBJ_FROM_PTR(self);
}


/***** Destructor ******/
mp_obj_t Motor___del__(mp_obj_t self_in) {
    _Motor_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Motor_obj_t);
    m_del_class(Motor, self->motor);
    return mp_const_none;
}


/***** Methods *****/
extern mp_obj_t Motor_pins(mp_obj_t self_in) {
    _Motor_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Motor_obj_t);
    pin_pair pins = self->motor->pins();

    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_int(pins.positive);
    tuple[1] = mp_obj_new_int(pins.negative);
    return mp_obj_new_tuple(2, tuple);
}

extern mp_obj_t Motor_enable(mp_obj_t self_in) {
    _Motor_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Motor_obj_t);
    self->motor->enable();
    return mp_const_none;
}

extern mp_obj_t Motor_disable(mp_obj_t self_in) {
    _Motor_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Motor_obj_t);
    self->motor->disable();
    return mp_const_none;
}

extern mp_obj_t Motor_is_enabled(mp_obj_t self_in) {
    _Motor_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Motor_obj_t);
    return self->motor->is_enabled() ? mp_const_true : mp_const_false;
}

extern mp_obj_t Motor_duty(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_duty };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_duty, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Motor_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Motor_obj_t);

    if(n_args <= 1) {
        return mp_obj_new_float(self->motor->duty());
    }
    else {
        self->motor->duty(mp_obj_get_float(args[ARG_duty].u_obj));
        return mp_const_none;
    }
}

extern mp_obj_t Motor_speed(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_speed };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_speed, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Motor_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Motor_obj_t);

    if(n_args <= 1) {
        return mp_obj_new_float(self->motor->speed());
    }
    else {
        self->motor->speed(mp_obj_get_float(args[ARG_speed].u_obj));
        return mp_const_none;
    }
}

extern mp_obj_t Motor_frequency(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_freq, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Motor_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Motor_obj_t);

    if(n_args <= 1) {
        return mp_obj_new_float(self->motor->frequency());
    }
    else {
        if(!self->motor->frequency(mp_obj_get_float(args[ARG_freq].u_obj))) {
            mp_raise_ValueError(MP_ERROR_TEXT("freq out of range. Expected 10Hz to 400KHz"));
        }
        return mp_const_none;
    }
}

extern mp_obj_t Motor_stop(mp_obj_t self_in) {
    _Motor_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Motor_obj_t);
    self->motor->stop();
    return mp_const_none;
}

extern mp_obj_t Motor_coast(mp_obj_t self_in) {
    _Motor_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Motor_obj_t);
    self->motor->coast();
    return mp_const_none;
}

extern mp_obj_t Motor_brake(mp_obj_t self_in) {
    _Motor_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Motor_obj_t);
    self->motor->brake();
    return mp_const_none;
}

extern mp_obj_t Motor_full_negative(mp_obj_t self_in) {
    _Motor_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Motor_obj_t);
    self->motor->full_negative();
    return mp_const_none;
}

extern mp_obj_t Motor_full_positive(mp_obj_t self_in) {
    _Motor_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Motor_obj_t);
    self->motor->full_positive();
    return mp_const_none;
}

extern mp_obj_t Motor_to_percent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 2) {
        enum { ARG_self, ARG_in };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in, MP_ARG_REQUIRED | MP_ARG_OBJ },
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Motor_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Motor_obj_t);

        float in = mp_obj_get_float(args[ARG_in].u_obj);

        self->motor->to_percent(in);
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

        _Motor_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Motor_obj_t);

        float in = mp_obj_get_float(args[ARG_in].u_obj);
        float in_min = mp_obj_get_float(args[ARG_in_min].u_obj);
        float in_max = mp_obj_get_float(args[ARG_in_max].u_obj);

        self->motor->to_percent(in, in_min, in_max);
    }
    else {
        enum { ARG_self, ARG_in, ARG_in_min, ARG_in_max, ARG_speed_min, ARG_speed_max };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_min, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_max, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_speed_min, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_speed_max, MP_ARG_REQUIRED | MP_ARG_OBJ }
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _Motor_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Motor_obj_t);

        float in = mp_obj_get_float(args[ARG_in].u_obj);
        float in_min = mp_obj_get_float(args[ARG_in_min].u_obj);
        float in_max = mp_obj_get_float(args[ARG_in_max].u_obj);
        float speed_min = mp_obj_get_float(args[ARG_speed_min].u_obj);
        float speed_max = mp_obj_get_float(args[ARG_speed_max].u_obj);

        self->motor->to_percent(in, in_min, in_max, speed_min, speed_max);
    }

    return mp_const_none;
}

extern mp_obj_t Motor_direction(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        enum { ARG_self, ARG_direction };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_direction, MP_ARG_OBJ, { .u_obj = mp_const_none }},
        };


    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Motor_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Motor_obj_t);

    if(n_args <= 1) {
        return mp_obj_new_int(self->motor->direction());
    }
    else {
        int direction = mp_obj_get_int(args[ARG_direction].u_obj);
        if(direction < 0 || direction > 1) {
            mp_raise_ValueError(MP_ERROR_TEXT("direction out of range. Expected NORMAL_DIR (0) or REVERSED_DIR (1)"));
        }
        self->motor->direction((Direction)direction);
        return mp_const_none;
    }
}

extern mp_obj_t Motor_speed_scale(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_speed_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_speed_scale, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Motor_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Motor_obj_t);

    if(n_args <= 1) {
        return mp_obj_new_float(self->motor->speed_scale());
    }
    else {
        float speed_scale = mp_obj_get_float(args[ARG_speed_scale].u_obj);
        if(speed_scale < FLT_EPSILON) {
            mp_raise_ValueError(MP_ERROR_TEXT("speed_scale out of range. Expected greater than 0.0"));
        }
        self->motor->speed_scale(speed_scale);
        return mp_const_none;
    }
}

extern mp_obj_t Motor_zeropoint(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_zeropoint };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_zeropoint, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Motor_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Motor_obj_t);

    if(n_args <= 1) {
        return mp_obj_new_float(self->motor->zeropoint());
    }
    else {
        float zeropoint = mp_obj_get_float(args[ARG_zeropoint].u_obj);
        if(zeropoint < 0.0f || zeropoint > 1.0f - FLT_EPSILON) {
            mp_raise_ValueError(MP_ERROR_TEXT("zeropoint out of range. Expected 0.0 to less than 1.0"));
        }
        self->motor->zeropoint(zeropoint);
        return mp_const_none;
    }
}

extern mp_obj_t Motor_deadzone(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_deadzone };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_deadzone, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Motor_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Motor_obj_t);

    if(n_args <= 1) {
        return mp_obj_new_float(self->motor->deadzone());
    }
    else {
        float deadzone = mp_obj_get_float(args[ARG_deadzone].u_obj);
        if(deadzone < 0.0f || deadzone > 1.0f) {
            mp_raise_ValueError(MP_ERROR_TEXT("deadzone out of range. Expected 0.0 to 1.0"));
        }
        self->motor->deadzone(deadzone);
        return mp_const_none;
    }
}

extern mp_obj_t Motor_decay_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_mode };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_mode, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Motor_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Motor_obj_t);

    if(n_args <= 1) {
        return mp_obj_new_int(self->motor->decay_mode());
    }
    else {
        int mode = mp_obj_get_int(args[ARG_mode].u_obj);
        if(mode < 0 || mode > 1) {
            mp_raise_ValueError(MP_ERROR_TEXT("mode out of range. Expected FAST_DECAY (0) or SLOW_DECAY (1)"));
        }
        self->motor->decay_mode((DecayMode)mode);
        return mp_const_none;
    }
}


}
