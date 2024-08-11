#include "drivers/motor/motor.hpp"
#include "drivers/motor/motor_cluster.hpp"
#include "common/pimoroni_common.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>


using namespace pimoroni;
using namespace motor;

extern "C" {
#include "motor.h"
#include "py/builtin.h"
#include "float.h"

void pimoroni_tuple_or_list(const mp_obj_t &object, mp_obj_t **items, size_t *length) {
    if(mp_obj_is_type(object, &mp_type_list)) {
        mp_obj_list_t *list = MP_OBJ_TO_PTR2(object, mp_obj_list_t);
        *length = list->len;
        *items = list->items;
    }
    else if(mp_obj_is_type(object, &mp_type_tuple)) {
        mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(object, mp_obj_tuple_t);
        *length = tuple->len;
        *items = tuple->items;
    }
    if(*items == nullptr) {
        mp_raise_TypeError("cannot convert object to a list or tuple of integers, or a single integer");
    } else if(*length == 0) {
        mp_raise_TypeError("list or tuple must contain at least one integer");
    }
}

uint8_t* pimoroni_motors_from_items(mp_obj_t *items, size_t length, int motor_count) {
    uint8_t *motors = m_new(uint8_t, length);
    for(size_t i = 0; i < length; i++) {
        int motor = mp_obj_get_int(items[i]);
        if(motor < 0 || motor >= motor_count) {
            m_free(motors);
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("a motor in the list or tuple is out of range. Expected 0 to %d"), motor_count - 1);
        }
        else {
            motors[i] = (uint8_t)motor;
        }
    }
    return motors;
}


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
        mp_raise_TypeError("cannot convert object to a list or tuple of pins");
    else if(pin_count != 2)
        mp_raise_TypeError("list or tuple must only contain two integers");
    else {
        int pos = mp_obj_get_int(items[0]);
        int neg = mp_obj_get_int(items[1]);
        if((pos < 0 || pos >= (int)NUM_BANK0_GPIOS) ||
           (neg < 0 || neg >= (int)NUM_BANK0_GPIOS)) {
            mp_raise_ValueError("a pin in the list or tuple is out of range. Expected 0 to 29");
        }
        else if(pos == neg) {
            mp_raise_ValueError("cannot use the same pin for motor positive and negative");
        }

        pins.positive = (uint8_t)pos;
        pins.negative = (uint8_t)neg;
    }

    int direction = args[ARG_direction].u_int;
    if(direction < 0 || direction > 1) {
        mp_raise_ValueError("direction out of range. Expected NORMAL_DIR (0) or REVERSED_DIR (1)");
    }

    float speed_scale = MotorState::DEFAULT_SPEED_SCALE;
    if(args[ARG_speed_scale].u_obj != mp_const_none) {
        speed_scale = mp_obj_get_float(args[ARG_speed_scale].u_obj);
        if(speed_scale < FLT_EPSILON) {
            mp_raise_ValueError("speed_scale out of range. Expected greater than 0.0");
        }
    }

    float zeropoint = MotorState::DEFAULT_ZEROPOINT;
    if(args[ARG_zeropoint].u_obj != mp_const_none) {
        zeropoint = mp_obj_get_float(args[ARG_zeropoint].u_obj);
        if(zeropoint < 0.0f || zeropoint > 1.0f - FLT_EPSILON) {
            mp_raise_ValueError("zeropoint out of range. Expected 0.0 to less than 1.0");
        }
    }

    float deadzone = MotorState::DEFAULT_DEADZONE;
    if(args[ARG_deadzone].u_obj != mp_const_none) {
        deadzone = mp_obj_get_float(args[ARG_deadzone].u_obj);
        if(deadzone < 0.0f || deadzone > 1.0f) {
            mp_raise_ValueError("deadzone out of range. Expected 0.0 to 1.0");
        }
    }

    float freq = MotorState::DEFAULT_FREQUENCY;
    if(args[ARG_freq].u_obj != mp_const_none) {
        freq = mp_obj_get_float(args[ARG_freq].u_obj);
    }

    int mode = args[ARG_mode].u_int;
    if(mode < 0 || mode > 1) {
        mp_raise_ValueError("mode out of range. Expected FAST_DECAY (0) or SLOW_DECAY (1)");
    }

    self = m_new_obj_with_finaliser(_Motor_obj_t);
    self->base.type = &Motor_type;

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
            mp_raise_ValueError("freq out of range. Expected 10Hz to 400KHz");
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
            mp_raise_ValueError("direction out of range. Expected NORMAL_DIR (0) or REVERSED_DIR (1)");
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
            mp_raise_ValueError("speed_scale out of range. Expected greater than 0.0");
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
            mp_raise_ValueError("zeropoint out of range. Expected 0.0 to less than 1.0");
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
            mp_raise_ValueError("deadzone out of range. Expected 0.0 to 1.0");
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
            mp_raise_ValueError("mode out of range. Expected FAST_DECAY (0) or SLOW_DECAY (1)");
        }
        self->motor->decay_mode((DecayMode)mode);
        return mp_const_none;
    }
}


/********** MotorCluster **********/

/***** Variables Struct *****/
typedef struct _MotorCluster_obj_t {
    mp_obj_base_t base;
    MotorCluster* cluster;
} _MotorCluster_obj_t;


/***** Print *****/
void MotorCluster_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _MotorCluster_obj_t);
    mp_print_str(print, "MotorCluster(");

    mp_print_str(print, "motors = {");

    uint8_t motor_count = self->cluster->count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
        mp_print_str(print, "\n\t{pins = (");
        pin_pair pins = self->cluster->pins(motor);
        mp_obj_print_helper(print, mp_obj_new_int(pins.positive), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_int(pins.negative), PRINT_REPR);
        mp_print_str(print, "), enabled = ");
        mp_obj_print_helper(print, self->cluster->is_enabled(motor) ? mp_const_true : mp_const_false, PRINT_REPR);
        mp_print_str(print, ", duty = ");
        mp_obj_print_helper(print, mp_obj_new_float(self->cluster->duty(motor)), PRINT_REPR);
        mp_print_str(print, ", speed = ");
        mp_obj_print_helper(print, mp_obj_new_float(self->cluster->speed(motor)), PRINT_REPR);
        mp_print_str(print, ", phase = ");
        mp_obj_print_helper(print, mp_obj_new_float(self->cluster->phase(motor)), PRINT_REPR);
        mp_print_str(print, "}");
        if(motor < motor_count - 1)
            mp_print_str(print, ", ");
    }
    if(motor_count > 0) {
        mp_print_str(print, "\n");
    }
    mp_print_str(print, "}, freq = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->cluster->frequency()), PRINT_REPR);

    mp_print_str(print, ")");
}


/***** Constructor *****/
mp_obj_t MotorCluster_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _MotorCluster_obj_t *self = nullptr;

    enum { ARG_pio, ARG_sm, ARG_pins, ARG_direction, ARG_speed_scale, ARG_zeropoint, ARG_deadzone, ARG_freq, ARG_mode, ARG_auto_phase };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pio, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_sm, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pins, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_direction, MP_ARG_INT, {.u_int = NORMAL_DIR} },
        { MP_QSTR_speed_scale, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_zeropoint, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_deadzone, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_freq, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_mode, MP_ARG_INT, {.u_int = MotorState::DEFAULT_DECAY_MODE} },
        { MP_QSTR_auto_phase, MP_ARG_BOOL, {.u_bool = true} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int pio_int = args[ARG_pio].u_int;
    if(pio_int < 0 || pio_int > (int)NUM_PIOS) {
        mp_raise_ValueError("pio out of range. Expected 0 to 1");
    }
    PIO pio = pio_int == 0 ? pio0 : pio1;

    int sm = args[ARG_sm].u_int;
    if(sm < 0 || sm > (int)NUM_PIO_STATE_MACHINES) {
        mp_raise_ValueError("sm out of range. Expected 0 to 3");
    }

    size_t pair_count = 0;
    pin_pair *pins = nullptr;

    // Determine what pair of pins this motor will use
    const mp_obj_t object = args[ARG_pins].u_obj;
    mp_obj_t *items = nullptr;
    if(mp_obj_is_type(object, &mp_type_list)) {
        mp_obj_list_t *list = MP_OBJ_TO_PTR2(object, mp_obj_list_t);
        pair_count = list->len;
        items = list->items;
    }
    else if(mp_obj_is_type(object, &mp_type_tuple)) {
        mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(object, mp_obj_tuple_t);
        pair_count = tuple->len;
        items = tuple->items;
    }

    if(items == nullptr)
        mp_raise_TypeError("cannot convert object to a list or tuple of pins");
    else if(pair_count == 0)
        mp_raise_TypeError("list or tuple must contain at least one pair tuple");
    else {
        // Specific check for is a single 2 pin list/tuple was provided
        if(pair_count == 2 && mp_obj_is_int(items[0]) && mp_obj_is_int(items[1])) {
            pins = m_new(pin_pair, 1);
            pair_count = 1;

            int pos = mp_obj_get_int(items[0]);
            int neg = mp_obj_get_int(items[1]);
            if((pos < 0 || pos >= (int)NUM_BANK0_GPIOS) ||
               (neg < 0 || neg >= (int)NUM_BANK0_GPIOS)) {
                delete[] pins;
                mp_raise_ValueError("a pin in the list or tuple is out of range. Expected 0 to 29");
            }
            else if(pos == neg) {
                delete[] pins;
                mp_raise_ValueError("cannot use the same pin for motor positive and negative");
            }

            pins[0].positive = (uint8_t)pos;
            pins[0].negative = (uint8_t)neg;
        }
        else {
            // Create and populate a local array of pins
            pins = m_new(pin_pair, pair_count);
            for(size_t i = 0; i < pair_count; i++) {
                mp_obj_t obj = items[i];
                if(!mp_obj_is_type(obj, &mp_type_tuple)) {
                    delete[] pins;
                    mp_raise_ValueError("cannot convert item to a pair tuple");
                }
                else {
                    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(obj, mp_obj_tuple_t);
                    if(tuple->len != 2) {
                        delete[] pins;
                        mp_raise_ValueError("pair tuple must only contain two integers");
                    }
                    int pos = mp_obj_get_int(tuple->items[0]);
                    int neg = mp_obj_get_int(tuple->items[1]);
                    if((pos < 0 || pos >= (int)NUM_BANK0_GPIOS) ||
                       (neg < 0 || neg >= (int)NUM_BANK0_GPIOS)) {
                        delete[] pins;
                        mp_raise_ValueError("a pin in the pair tuple is out of range. Expected 0 to 29");
                    }
                    else if(pos == neg) {
                        delete[] pins;
                        mp_raise_ValueError("cannot use the same pin for motor positive and negative");
                    }

                    pins[i].positive = (uint8_t)pos;
                    pins[i].negative = (uint8_t)neg;
                }
            }
        }
    }

    int direction = args[ARG_direction].u_int;
    if(direction < 0 || direction > 1) {
        mp_raise_ValueError("direction out of range. Expected NORMAL_DIR (0) or REVERSED_DIR (1)");
    }

    float speed_scale = MotorState::DEFAULT_SPEED_SCALE;
    if(args[ARG_speed_scale].u_obj != mp_const_none) {
        speed_scale = mp_obj_get_float(args[ARG_speed_scale].u_obj);
        if(speed_scale < FLT_EPSILON) {
            mp_raise_ValueError("speed_scale out of range. Expected greater than 0.0");
        }
    }

    float zeropoint = MotorState::DEFAULT_ZEROPOINT;
    if(args[ARG_zeropoint].u_obj != mp_const_none) {
        zeropoint = mp_obj_get_float(args[ARG_zeropoint].u_obj);
        if(zeropoint < 0.0f || zeropoint > 1.0f - FLT_EPSILON) {
            mp_raise_ValueError("zeropoint out of range. Expected 0.0 to less than 1.0");
        }
    }

    float deadzone = MotorState::DEFAULT_DEADZONE;
    if(args[ARG_deadzone].u_obj != mp_const_none) {
        deadzone = mp_obj_get_float(args[ARG_deadzone].u_obj);
        if(deadzone < 0.0f || deadzone > 1.0f) {
            mp_raise_ValueError("deadzone out of range. Expected 0.0 to 1.0");
        }
    }

    float freq = MotorState::DEFAULT_FREQUENCY;
    if(args[ARG_freq].u_obj != mp_const_none) {
        freq = mp_obj_get_float(args[ARG_freq].u_obj);
    }

    int mode = args[ARG_mode].u_int;
    if(mode < 0 || mode > 1) {
        mp_raise_ValueError("mode out of range. Expected FAST_DECAY (0) or SLOW_DECAY (1)");
    }

    bool auto_phase = args[ARG_auto_phase].u_bool;

    MotorCluster *cluster = m_new_class(MotorCluster, pio, sm, pins, pair_count, (Direction)direction, speed_scale, zeropoint, deadzone,
                               freq, (DecayMode)mode, auto_phase);

    // Cleanup the pins array
    if(pins != nullptr)
        delete[] pins;

    if(!cluster->init()) {
        m_del_class(MotorCluster, cluster);
        mp_raise_msg(&mp_type_RuntimeError, "unable to allocate the hardware resources needed to initialise this MotorCluster. Try running `import gc` followed by `gc.collect()` before creating it");
    }

    self = m_new_obj_with_finaliser(_MotorCluster_obj_t);
    self->base.type = &MotorCluster_type;
    self->cluster = cluster;

    return MP_OBJ_FROM_PTR(self);
}


/***** Destructor ******/
mp_obj_t MotorCluster___del__(mp_obj_t self_in) {
    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _MotorCluster_obj_t);
    m_del_class(MotorCluster, self->cluster);
    return mp_const_none;
}


/***** Methods *****/
extern mp_obj_t MotorCluster_count(mp_obj_t self_in) {
    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _MotorCluster_obj_t);
    return mp_obj_new_int(self->cluster->count());
}

extern mp_obj_t MotorCluster_pins(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motor };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motor, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor = args[ARG_motor].u_int;
    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else if(motor < 0 || motor >= motor_count)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
    else {
        pin_pair pins = self->cluster->pins((uint)motor);

        mp_obj_t tuple[2];
        tuple[0] = mp_obj_new_int(pins.positive);
        tuple[1] = mp_obj_new_int(pins.negative);
        return mp_obj_new_tuple(2, tuple);
    }

    return mp_const_none;
}

extern mp_obj_t MotorCluster_enable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motors, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motors, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        // Determine what motor(s) to enable
        const mp_obj_t object = args[ARG_motors].u_obj;
        if(mp_obj_is_int(object)) {
            int motor = mp_obj_get_int(object);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                self->cluster->enable((uint)motor, args[ARG_load].u_bool);
        }
        else {
            size_t length = 0;
            mp_obj_t *items = nullptr;
            pimoroni_tuple_or_list(object, &items, &length);

            // Create and populate a local array of motor indices
            uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

            self->cluster->enable(motors, length, args[ARG_load].u_bool);
            delete[] motors;
        }
    }

    return mp_const_none;
}

extern mp_obj_t MotorCluster_enable_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);
    self->cluster->enable_all(args[ARG_load].u_bool);

    return mp_const_none;
}

extern mp_obj_t MotorCluster_disable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motors, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motors, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        // Determine what motor(s) to disable
        const mp_obj_t object = args[ARG_motors].u_obj;
        if(mp_obj_is_int(object)) {
            int motor = mp_obj_get_int(object);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                self->cluster->disable((uint)motor, args[ARG_load].u_bool);
        }
        else {
            size_t length = 0;
            mp_obj_t *items = nullptr;
            pimoroni_tuple_or_list(object, &items, &length);

            // Create and populate a local array of motor indices
            uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

            self->cluster->disable(motors, length, args[ARG_load].u_bool);
            delete[] motors;
        }
    }

    return mp_const_none;
}

extern mp_obj_t MotorCluster_disable_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);
    self->cluster->disable_all(args[ARG_load].u_bool);

    return mp_const_none;
}

extern mp_obj_t MotorCluster_is_enabled(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motor };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motor, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor = args[ARG_motor].u_int;
    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else if(motor < 0 || motor >= motor_count)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
    else
        return self->cluster->is_enabled((uint)motor) ? mp_const_true : mp_const_false;

    return mp_const_none;
}

extern mp_obj_t MotorCluster_duty(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motor, ARG_duty, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motor, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_duty, MP_ARG_OBJ, { .u_obj = mp_const_none }},
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        if(n_args <= 2) {
            int motor = mp_obj_get_int(args[ARG_motor].u_obj);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                return mp_obj_new_float(self->cluster->duty((uint)motor));
        }
        else {
            // Determine what motor(s) to modify
            const mp_obj_t object = args[ARG_motor].u_obj;
            if(mp_obj_is_int(object)) {
                int motor = mp_obj_get_int(object);
                if(motor < 0 || motor >= motor_count)
                    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
                else {
                    float duty = mp_obj_get_float(args[ARG_duty].u_obj);
                    self->cluster->duty((uint)motor, duty, args[ARG_load].u_bool);
                }
            }
            else {
                size_t length = 0;
                mp_obj_t *items = nullptr;
                pimoroni_tuple_or_list(object, &items, &length);

                // Create and populate a local array of motor indices
                uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

                float duty = mp_obj_get_float(args[ARG_duty].u_obj);
                self->cluster->duty(motors, length, duty, args[ARG_load].u_bool);
                delete[] motors;
            }
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_all_to_duty(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_duty, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_duty, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        float duty = mp_obj_get_float(args[ARG_duty].u_obj);
        self->cluster->all_to_duty(duty, args[ARG_load].u_bool);
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_speed(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motor, ARG_speed, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motor, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_speed, MP_ARG_OBJ, { .u_obj = mp_const_none }},
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        if(n_args <= 2) {
            int motor = mp_obj_get_int(args[ARG_motor].u_obj);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                return mp_obj_new_float(self->cluster->speed((uint)motor));
        }
        else {
            // Determine what motor(s) to modify
            const mp_obj_t object = args[ARG_motor].u_obj;
            if(mp_obj_is_int(object)) {
                int motor = mp_obj_get_int(object);
                if(motor < 0 || motor >= motor_count)
                    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
                else {
                    float speed = mp_obj_get_float(args[ARG_speed].u_obj);
                    self->cluster->speed((uint)motor, speed, args[ARG_load].u_bool);
                }
            }
            else {
                size_t length = 0;
                mp_obj_t *items = nullptr;
                pimoroni_tuple_or_list(object, &items, &length);

                // Create and populate a local array of motor indices
                uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

                float speed = mp_obj_get_float(args[ARG_speed].u_obj);
                self->cluster->speed(motors, length, speed, args[ARG_load].u_bool);
                delete[] motors;
            }
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_all_to_speed(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_speed, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_speed, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        float speed = mp_obj_get_float(args[ARG_speed].u_obj);
        self->cluster->all_to_speed(speed, args[ARG_load].u_bool);
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_phase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    
    enum { ARG_self, ARG_motor, ARG_phase, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motor, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_phase, MP_ARG_OBJ, { .u_obj = mp_const_none }},
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        if(n_args <= 2) {
            int motor = mp_obj_get_int(args[ARG_motor].u_obj);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                return mp_obj_new_float(self->cluster->phase((uint)motor));
        }
        else {
            // Determine what motor(s) to modify
            const mp_obj_t object = args[ARG_motor].u_obj;
            if(mp_obj_is_int(object)) {
                int motor = mp_obj_get_int(object);
                if(motor < 0 || motor >= motor_count)
                    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
                else {
                    float phase = mp_obj_get_float(args[ARG_phase].u_obj);
                    self->cluster->phase((uint)motor, phase, args[ARG_load].u_bool);
                }
            }
            else {
                size_t length = 0;
                mp_obj_t *items = nullptr;
                pimoroni_tuple_or_list(object, &items, &length);

                // Create and populate a local array of motor indices
                uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

                float phase = mp_obj_get_float(args[ARG_phase].u_obj);
                self->cluster->phase(motors, length, phase, args[ARG_load].u_bool);
                delete[] motors;
            }
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_all_to_phase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_phase, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_phase, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        float phase = mp_obj_get_float(args[ARG_phase].u_obj);
        self->cluster->all_to_phase(phase, args[ARG_load].u_bool);
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_frequency(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_freq, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    if(n_args <= 1) {
        return mp_obj_new_float(self->cluster->frequency());
    }
    else {
        float freq = mp_obj_get_float(args[ARG_freq].u_obj);

        if(!self->cluster->frequency(freq))
            mp_raise_ValueError("freq out of range. Expected 10Hz to 400KHz");
        else
            return mp_const_none;
    }
}

extern mp_obj_t MotorCluster_stop(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motors, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motors, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        // Determine what motor(s) to stop
        const mp_obj_t object = args[ARG_motors].u_obj;
        if(mp_obj_is_int(object)) {
            int motor = mp_obj_get_int(object);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                self->cluster->stop((uint)motor, args[ARG_load].u_bool);
        }
        else {
            size_t length = 0;
            mp_obj_t *items = nullptr;
            pimoroni_tuple_or_list(object, &items, &length);

            // Create and populate a local array of motor indices
            uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

            self->cluster->stop(motors, length, args[ARG_load].u_bool);
            delete[] motors;
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_stop_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        self->cluster->stop_all(args[ARG_load].u_bool);
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_coast(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motors, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motors, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        // Determine what motor(s) to coast
        const mp_obj_t object = args[ARG_motors].u_obj;
        if(mp_obj_is_int(object)) {
            int motor = mp_obj_get_int(object);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                self->cluster->coast((uint)motor, args[ARG_load].u_bool);
        }
        else {
            size_t length = 0;
            mp_obj_t *items = nullptr;
            pimoroni_tuple_or_list(object, &items, &length);

            // Create and populate a local array of motor indices
            uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

            self->cluster->coast(motors, length, args[ARG_load].u_bool);
            delete[] motors;
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_coast_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        self->cluster->coast_all(args[ARG_load].u_bool);
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_brake(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motors, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motors, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        // Determine what motor(s) to brake
        const mp_obj_t object = args[ARG_motors].u_obj;
        if(mp_obj_is_int(object)) {
            int motor = mp_obj_get_int(object);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                self->cluster->brake((uint)motor, args[ARG_load].u_bool);
        }
        else {
            size_t length = 0;
            mp_obj_t *items = nullptr;
            pimoroni_tuple_or_list(object, &items, &length);

            // Create and populate a local array of motor indices
            uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

            self->cluster->brake(motors, length, args[ARG_load].u_bool);
            delete[] motors;
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_brake_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        self->cluster->brake_all(args[ARG_load].u_bool);
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_full_negative(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motors, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motors, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        // Determine what motor(s) to set to full negative
        const mp_obj_t object = args[ARG_motors].u_obj;
        if(mp_obj_is_int(object)) {
            int motor = mp_obj_get_int(object);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                self->cluster->full_negative((uint)motor, args[ARG_load].u_bool);
        }
        else {
            size_t length = 0;
            mp_obj_t *items = nullptr;
            pimoroni_tuple_or_list(object, &items, &length);

            // Create and populate a local array of motor indices
            uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

            self->cluster->full_negative(motors, length, args[ARG_load].u_bool);
            delete[] motors;
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_all_full_negative(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        self->cluster->all_full_negative(args[ARG_load].u_bool);
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_full_positive(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motors, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motors, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        // Determine what motor(s) to set to full positive
        const mp_obj_t object = args[ARG_motors].u_obj;
        if(mp_obj_is_int(object)) {
            int motor = mp_obj_get_int(object);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                self->cluster->full_positive((uint)motor, args[ARG_load].u_bool);
        }
        else {
            size_t length = 0;
            mp_obj_t *items = nullptr;
            pimoroni_tuple_or_list(object, &items, &length);

            // Create and populate a local array of motor indices
            uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

            self->cluster->full_positive(motors, length, args[ARG_load].u_bool);
            delete[] motors;
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_all_full_positive(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        self->cluster->all_full_positive(args[ARG_load].u_bool);
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_to_percent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 4) {
        enum { ARG_self, ARG_motors, ARG_in, ARG_load };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_motors, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

        int motor_count = (int)self->cluster->count();
        if(motor_count == 0)
            mp_raise_ValueError("this cluster does not have any motors");
        else {
            // Determine what motor(s) to modify
            const mp_obj_t object = args[ARG_motors].u_obj;
            if(mp_obj_is_int(object)) {
                int motor = mp_obj_get_int(object);
                if(motor < 0 || motor >= motor_count)
                    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
                else {
                    float in = mp_obj_get_float(args[ARG_in].u_obj);
                    self->cluster->to_percent((uint)motor, in, args[ARG_load].u_bool);
                }
            }
            else {
                size_t length = 0;
                mp_obj_t *items = nullptr;
                pimoroni_tuple_or_list(object, &items, &length);

                // Create and populate a local array of motor indices
                uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

                float in = mp_obj_get_float(args[ARG_in].u_obj);
                self->cluster->to_percent(motors, length, in, args[ARG_load].u_bool);
                delete[] motors;
            }
        }
    }
    else if(n_args <= 6) {
        enum { ARG_self, ARG_motors, ARG_in, ARG_in_min, ARG_in_max, ARG_load };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_motors, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_min, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_max, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

        int motor_count = (int)self->cluster->count();
        if(motor_count == 0)
            mp_raise_ValueError("this cluster does not have any motors");
        else {
            // Determine what motor(s) to modify
            const mp_obj_t object = args[ARG_motors].u_obj;
            if(mp_obj_is_int(object)) {
                int motor = mp_obj_get_int(object);
                if(motor < 0 || motor >= motor_count)
                    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
                else {
                    float in = mp_obj_get_float(args[ARG_in].u_obj);
                    float in_min = mp_obj_get_float(args[ARG_in_min].u_obj);
                    float in_max = mp_obj_get_float(args[ARG_in_max].u_obj);
                    self->cluster->to_percent((uint)motor, in, in_min, in_max, args[ARG_load].u_bool);
                }
            }
            else {
                size_t length = 0;
                mp_obj_t *items = nullptr;
                pimoroni_tuple_or_list(object, &items, &length);

                // Create and populate a local array of motor indices
                uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

                float in = mp_obj_get_float(args[ARG_in].u_obj);
                float in_min = mp_obj_get_float(args[ARG_in_min].u_obj);
                float in_max = mp_obj_get_float(args[ARG_in_max].u_obj);
                self->cluster->to_percent(motors, length, in, in_min, in_max, args[ARG_load].u_bool);
                delete[] motors;
            }
        }
    }
    else {
        enum { ARG_self, ARG_motors, ARG_in, ARG_in_min, ARG_in_max, ARG_speed_min, ARG_speed_max, ARG_load };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_motors, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_min, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_max, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_speed_min, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_speed_max, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

        int motor_count = (int)self->cluster->count();
        if(motor_count == 0)
            mp_raise_ValueError("this cluster does not have any motors");
        else {
            // Determine what motor(s) to modify
            const mp_obj_t object = args[ARG_motors].u_obj;
            if(mp_obj_is_int(object)) {
                int motor = mp_obj_get_int(object);
                if(motor < 0 || motor >= motor_count)
                    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
                else {
                    float in = mp_obj_get_float(args[ARG_in].u_obj);
                    float in_min = mp_obj_get_float(args[ARG_in_min].u_obj);
                    float in_max = mp_obj_get_float(args[ARG_in_max].u_obj);
                    float speed_min = mp_obj_get_float(args[ARG_speed_min].u_obj);
                    float speed_max = mp_obj_get_float(args[ARG_speed_max].u_obj);
                    self->cluster->to_percent((uint)motor, in, in_min, in_max, speed_min, speed_max, args[ARG_load].u_bool);
                }
            }
            else {
                size_t length = 0;
                mp_obj_t *items = nullptr;
                pimoroni_tuple_or_list(object, &items, &length);

                // Create and populate a local array of motor indices
                uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

                float in = mp_obj_get_float(args[ARG_in].u_obj);
                float in_min = mp_obj_get_float(args[ARG_in_min].u_obj);
                float in_max = mp_obj_get_float(args[ARG_in_max].u_obj);
                float speed_min = mp_obj_get_float(args[ARG_speed_min].u_obj);
                float speed_max = mp_obj_get_float(args[ARG_speed_max].u_obj);
                self->cluster->to_percent(motors, length, in, in_min, in_max, speed_min, speed_max, args[ARG_load].u_bool);
                delete[] motors;
            }
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_all_to_percent(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if(n_args <= 3) {
        enum { ARG_self, ARG_in, ARG_load };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

        int motor_count = (int)self->cluster->count();
        if(motor_count == 0)
            mp_raise_ValueError("this cluster does not have any motors");
        else {
            float in = mp_obj_get_float(args[ARG_in].u_obj);
            self->cluster->all_to_percent(in, args[ARG_load].u_bool);
        }
    }
    else if(n_args <= 5) {
        enum { ARG_self, ARG_in, ARG_in_min, ARG_in_max, ARG_load };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_min, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_max, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

        int motor_count = (int)self->cluster->count();
        if(motor_count == 0)
            mp_raise_ValueError("this cluster does not have any motors");
        else {
            float in = mp_obj_get_float(args[ARG_in].u_obj);
            float in_min = mp_obj_get_float(args[ARG_in_min].u_obj);
            float in_max = mp_obj_get_float(args[ARG_in_max].u_obj);
            self->cluster->all_to_percent(in, in_min, in_max, args[ARG_load].u_bool);
        }
    }
    else {
        enum { ARG_self, ARG_in, ARG_in_min, ARG_in_max, ARG_speed_min, ARG_speed_max, ARG_load };
        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_min, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_in_max, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_speed_min, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_speed_max, MP_ARG_REQUIRED | MP_ARG_OBJ },
            { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
        };

        // Parse args.
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

        int motor_count = (int)self->cluster->count();
        if(motor_count == 0)
            mp_raise_ValueError("this cluster does not have any motors");
        else {
            float in = mp_obj_get_float(args[ARG_in].u_obj);
            float in_min = mp_obj_get_float(args[ARG_in_min].u_obj);
            float in_max = mp_obj_get_float(args[ARG_in_max].u_obj);
            float speed_min = mp_obj_get_float(args[ARG_speed_min].u_obj);
            float speed_max = mp_obj_get_float(args[ARG_speed_max].u_obj);
            self->cluster->all_to_percent(in, in_min, in_max, speed_min, speed_max, args[ARG_load].u_bool);
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_load(mp_obj_t self_in) {
    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _MotorCluster_obj_t);
    self->cluster->load();
    return mp_const_none;
}

extern mp_obj_t MotorCluster_direction(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motor, ARG_direction };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motor, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_direction, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        if(n_args <= 2) {
            int motor = mp_obj_get_int(args[ARG_motor].u_obj);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                return mp_obj_new_int((int)self->cluster->direction((uint)motor));
        }
        else {
            // Determine what motor(s) to modify
            const mp_obj_t object = args[ARG_motor].u_obj;
            if(mp_obj_is_int(object)) {
                int motor = mp_obj_get_int(object);
                if(motor < 0 || motor >= motor_count)
                    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
                else {
                    int direction = mp_obj_get_int(args[ARG_direction].u_obj);
                    if(direction < 0 || direction > 1) {
                        mp_raise_ValueError("direction out of range. Expected NORMAL_DIR (0) or REVERSED_DIR (1)");
                    }
                    self->cluster->direction((uint)motor, (Direction)direction);
                }
            }
            else {
                size_t length = 0;
                mp_obj_t *items = nullptr;
                pimoroni_tuple_or_list(object, &items, &length);

                // Create and populate a local array of motor indices
                uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

                int direction = mp_obj_get_int(args[ARG_direction].u_obj);
                if(direction < 0 || direction > 1) {
                    delete[] motors;
                    mp_raise_ValueError("direction out of range. Expected NORMAL_DIR (0) or REVERSED_DIR (1)");
                }
                self->cluster->direction(motors, length, (Direction)direction);
                delete[] motors;
            }
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_all_directions(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_direction };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_direction, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        int direction = args[ARG_direction].u_int;
        if(direction < 0 || direction > 1) {
            mp_raise_ValueError("direction out of range. Expected NORMAL_DIR (0) or REVERSED_DIR (1)");
        }
        self->cluster->all_directions((Direction)direction);
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_speed_scale(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motor, ARG_speed_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motor, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_speed_scale, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        if(n_args <= 2) {
            int motor = mp_obj_get_int(args[ARG_motor].u_obj);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                return mp_obj_new_float(self->cluster->speed_scale((uint)motor));
        }
        else {
            // Determine what motor(s) to modify
            const mp_obj_t object = args[ARG_motor].u_obj;
            if(mp_obj_is_int(object)) {
                int motor = mp_obj_get_int(object);
                if(motor < 0 || motor >= motor_count)
                    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
                else {
                    float speed_scale = mp_obj_get_float(args[ARG_speed_scale].u_obj);
                    if(speed_scale < FLT_EPSILON) {
                        mp_raise_ValueError("speed_scale out of range. Expected greater than 0.0");
                    }
                    self->cluster->speed_scale((uint)motor, speed_scale);
                }
            }
            else {
                size_t length = 0;
                mp_obj_t *items = nullptr;
                pimoroni_tuple_or_list(object, &items, &length);

                // Create and populate a local array of motor indices
                uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

                float speed_scale = mp_obj_get_float(args[ARG_speed_scale].u_obj);
                if(speed_scale < FLT_EPSILON) {
                    delete[] motors;
                    mp_raise_ValueError("speed_scale out of range. Expected greater than 0.0");
                }

                self->cluster->speed_scale(motors, length, speed_scale);
                delete[] motors;
            }
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_all_speed_scales(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_speed_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_speed_scale, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        float speed_scale = mp_obj_get_float(args[ARG_speed_scale].u_obj);
        if(speed_scale < FLT_EPSILON) {
            mp_raise_ValueError("speed_scale out of range. Expected greater than 0.0");
        }
        self->cluster->all_speed_scales(speed_scale);
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_zeropoint(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motor, ARG_zeropoint };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motor, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_zeropoint, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        if(n_args <= 2) {
            int motor = mp_obj_get_int(args[ARG_motor].u_obj);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                return mp_obj_new_float(self->cluster->zeropoint((uint)motor));
        }
        else {
            // Determine what motor(s) to modify
            const mp_obj_t object = args[ARG_motor].u_obj;
            if(mp_obj_is_int(object)) {
                int motor = mp_obj_get_int(object);
                if(motor < 0 || motor >= motor_count)
                    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
                else {
                    float zeropoint = mp_obj_get_float(args[ARG_zeropoint].u_obj);
                    if(zeropoint < 0.0f || zeropoint > 1.0f - FLT_EPSILON) {
                        mp_raise_ValueError("zeropoint out of range. Expected 0.0 to less than 1.0");
                    }
                    self->cluster->zeropoint((uint)motor, zeropoint);
                }
            }
            else {
                size_t length = 0;
                mp_obj_t *items = nullptr;
                pimoroni_tuple_or_list(object, &items, &length);

                // Create and populate a local array of motor indices
                uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

                float zeropoint = mp_obj_get_float(args[ARG_zeropoint].u_obj);
                if(zeropoint < 0.0f || zeropoint > 1.0f - FLT_EPSILON) {
                    delete[] motors;
                    mp_raise_ValueError("zeropoint out of range. Expected 0.0 to less than 1.0");
                }

                self->cluster->zeropoint(motors, length, zeropoint);
                delete[] motors;
            }
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_all_zeropoints(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_zeropoint };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_zeropoint, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        float zeropoint = mp_obj_get_float(args[ARG_zeropoint].u_obj);
        if(zeropoint < 0.0f || zeropoint > 1.0f - FLT_EPSILON) {
            mp_raise_ValueError("zeropoint out of range. Expected 0.0 to less than 1.0");
        }
        self->cluster->all_zeropoints(zeropoint);
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_deadzone(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motor, ARG_deadzone, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motor, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_deadzone, MP_ARG_OBJ, { .u_obj = mp_const_none }},
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        if(n_args <= 2) {
            int motor = mp_obj_get_int(args[ARG_motor].u_obj);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                return mp_obj_new_float(self->cluster->deadzone((uint)motor));
        }
        else {
            // Determine what motor(s) to modify
            const mp_obj_t object = args[ARG_motor].u_obj;
            if(mp_obj_is_int(object)) {
                int motor = mp_obj_get_int(object);
                if(motor < 0 || motor >= motor_count)
                    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
                else {
                    float deadzone = mp_obj_get_float(args[ARG_deadzone].u_obj);
                    if(deadzone < 0.0f || deadzone > 1.0f) {
                        mp_raise_ValueError("deadzone out of range. Expected 0.0 to 1.0");
                    }
                    self->cluster->deadzone((uint)motor, deadzone, args[ARG_load].u_bool);
                }
            }
            else {
                size_t length = 0;
                mp_obj_t *items = nullptr;
                pimoroni_tuple_or_list(object, &items, &length);

                // Create and populate a local array of motor indices
                uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

                float deadzone = mp_obj_get_float(args[ARG_deadzone].u_obj);
                if(deadzone < 0.0f || deadzone > 1.0f) {
                    delete[] motors;
                    mp_raise_ValueError("deadzone out of range. Expected 0.0 to 1.0");
                }
                self->cluster->deadzone(motors, length, deadzone, args[ARG_load].u_bool);
                delete[] motors;
            }
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_all_deadzones(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
enum { ARG_self, ARG_deadzone, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_deadzone, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        float deadzone = mp_obj_get_float(args[ARG_deadzone].u_obj);
        if(deadzone < 0.0f || deadzone > 1.0f) {
            mp_raise_ValueError("deadzone out of range. Expected 0.0 to 1.0");
        }
        self->cluster->all_deadzones(deadzone, args[ARG_load].u_bool);
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_decay_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motor, ARG_mode, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motor, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_mode, MP_ARG_OBJ, { .u_obj = mp_const_none }},
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        if(n_args <= 2) {
            int motor = mp_obj_get_int(args[ARG_motor].u_obj);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                return mp_obj_new_int((int)self->cluster->decay_mode((uint)motor));
        }
        else {
            // Determine what motor(s) to modify
            const mp_obj_t object = args[ARG_motor].u_obj;
            if(mp_obj_is_int(object)) {
                int motor = mp_obj_get_int(object);
                if(motor < 0 || motor >= motor_count)
                    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
                else {
                    int mode = args[ARG_mode].u_int;
                    if(mode < 0 || mode > 1) {
                        mp_raise_ValueError("mode out of range. Expected FAST_DECAY (0) or SLOW_DECAY (1)");
                    }
                    self->cluster->decay_mode((uint)motor, (DecayMode)mode, args[ARG_load].u_bool);
                }
            }
            else {
                size_t length = 0;
                mp_obj_t *items = nullptr;
                pimoroni_tuple_or_list(object, &items, &length);

                // Create and populate a local array of motor indices
                uint8_t *motors = pimoroni_motors_from_items(items, length, motor_count);

                int mode = args[ARG_mode].u_int;
                if(mode < 0 || mode > 1) {
                    delete[] motors;
                    mp_raise_ValueError("mode out of range. Expected FAST_DECAY (0) or SLOW_DECAY (1)");
                }
                self->cluster->decay_mode(motors, length, (DecayMode)mode, args[ARG_load].u_bool);
                delete[] motors;
            }
        }
    }
    return mp_const_none;
}

extern mp_obj_t MotorCluster_all_decay_modes(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_mode, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _MotorCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _MotorCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError("this cluster does not have any motors");
    else {
        int mode = args[ARG_mode].u_int;
        if(mode < 0 || mode > 1) {
            mp_raise_ValueError("mode out of range. Expected FAST_DECAY (0) or SLOW_DECAY (1)");
        }
        self->cluster->all_decay_modes((DecayMode)mode, args[ARG_load].u_bool);
    }
    return mp_const_none;
}
}
