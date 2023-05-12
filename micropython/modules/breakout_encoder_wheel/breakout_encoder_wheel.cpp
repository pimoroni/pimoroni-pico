#include "libraries/breakout_encoder_wheel/breakout_encoder_wheel.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>


using namespace pimoroni;
using namespace encoderwheel;

extern "C" {
#include "breakout_encoder_wheel.h"
#include "pimoroni_i2c.h"

/***** Variables Struct *****/
typedef struct _breakout_encoder_wheel_BreakoutEncoderWheel_obj_t {
    mp_obj_base_t base;
    BreakoutEncoderWheel *breakout;
    _PimoroniI2C_obj_t *i2c;
} breakout_encoder_wheel_BreakoutEncoderWheel_obj_t;


/***** Constructor *****/
mp_obj_t BreakoutEncoderWheel_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_ioe_address, ARG_led_address, ARG_interrupt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_ioe_address, MP_ARG_INT, {.u_int = BreakoutEncoderWheel::DEFAULT_IOE_I2C_ADDRESS} },
        { MP_QSTR_led_address, MP_ARG_INT, {.u_int = BreakoutEncoderWheel::DEFAULT_LED_I2C_ADDRESS} },
        { MP_QSTR_interrupt, MP_ARG_INT, {.u_int = PIN_UNUSED} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj(breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    self->base.type = &breakout_encoder_wheel_BreakoutEncoderWheel_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->breakout = m_new_class(BreakoutEncoderWheel, (pimoroni::I2C *)(self->i2c->i2c), args[ARG_ioe_address].u_int, args[ARG_led_address].u_int, args[ARG_interrupt].u_int);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "BreakoutEncoderWheel: breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t BreakoutEncoderWheel_set_ioe_address(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_address };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);

    self->breakout->set_ioe_address(args[ARG_address].u_int);

    return mp_const_none;
}

mp_obj_t BreakoutEncoderWheel_get_interrupt_flag(mp_obj_t self_in) {
    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    return mp_obj_new_bool(self->breakout->get_interrupt_flag());
}

mp_obj_t BreakoutEncoderWheel_clear_interrupt_flag(mp_obj_t self_in) {
    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    self->breakout->clear_interrupt_flag();

    return mp_const_none;
}

extern mp_obj_t BreakoutEncoderWheel_pressed(mp_obj_t self_in, mp_obj_t button_in) {
    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    int button = mp_obj_get_int(button_in);

    if(button < 0 || button >= 5) {
        mp_raise_ValueError("button out of range. Expected 0 to 4");
    }

    return mp_obj_new_bool(self->breakout->pressed(button));
}

extern mp_obj_t BreakoutEncoderWheel_count(mp_obj_t self_in) {
    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    return mp_obj_new_int(self->breakout->count());
}

extern mp_obj_t BreakoutEncoderWheel_delta(mp_obj_t self_in) {
    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    return mp_obj_new_int(self->breakout->delta());
}

extern mp_obj_t BreakoutEncoderWheel_step(mp_obj_t self_in) {
    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    return mp_obj_new_int(self->breakout->step());
}

extern mp_obj_t BreakoutEncoderWheel_turn(mp_obj_t self_in) {
    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    return mp_obj_new_int(self->breakout->turn());
}

extern mp_obj_t BreakoutEncoderWheel_zero(mp_obj_t self_in) {
    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    self->breakout->zero();

    return mp_const_none;
}

extern mp_obj_t BreakoutEncoderWheel_revolutions(mp_obj_t self_in) {
    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    return mp_obj_new_float(self->breakout->revolutions());
}

extern mp_obj_t BreakoutEncoderWheel_degrees(mp_obj_t self_in) {
    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    return mp_obj_new_float(self->breakout->degrees());
}

extern mp_obj_t BreakoutEncoderWheel_radians(mp_obj_t self_in) {
    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    return mp_obj_new_float(self->breakout->radians());
}

extern mp_obj_t BreakoutEncoderWheel_direction(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_direction };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_direction, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);

    if(n_args <= 1) {
        return mp_obj_new_int(self->breakout->direction());
    }
    else {
        int direction = mp_obj_get_int(args[ARG_direction].u_obj);
        if(direction < 0 || direction > 1) {
            mp_raise_ValueError("direction out of range. Expected NORMAL_DIR (0) or REVERSED_DIR (1)");
        }
        self->breakout->direction((Direction)direction);
        return mp_const_none;
    }
}

extern mp_obj_t BreakoutEncoderWheel_set_rgb(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_index, ARG_r, ARG_g, ARG_b };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_g, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);

    int index = args[ARG_index].u_int;
    int r = args[ARG_r].u_int;
    int g = args[ARG_g].u_int;
    int b = args[ARG_b].u_int;

    if(index < 0 || index >= 24)
        mp_raise_ValueError("index out of range. Expected 0 to 23");
    else if(r < 0 || r > 255)
        mp_raise_ValueError("r out of range. Expected 0 to 255");
    else if(g < 0 || g > 255)
        mp_raise_ValueError("g out of range. Expected 0 to 255");
    else if(b < 0 || b > 255)
        mp_raise_ValueError("b out of range. Expected 0 to 255");
    else
        self->breakout->set_rgb(index, r, g, b);

    return mp_const_none;
}

extern mp_obj_t BreakoutEncoderWheel_set_hsv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_index, ARG_h, ARG_s, ARG_v };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_h, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_s, MP_ARG_OBJ, { .u_obj = mp_const_none }},
        { MP_QSTR_v, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);

    int index = args[ARG_index].u_int;
    float h = mp_obj_get_float(args[ARG_h].u_obj);

    float s = 1.0f;
    if (args[ARG_s].u_obj != mp_const_none) {
        s = mp_obj_get_float(args[ARG_s].u_obj);
    }

    float v = 1.0f;
    if (args[ARG_v].u_obj != mp_const_none) {
        v = mp_obj_get_float(args[ARG_v].u_obj);
    }

    if(index < 0 || index >= 24)
        mp_raise_ValueError("index out of range. Expected 0 to 23");
    else
        self->breakout->set_hsv(index, h, s, v);

    return mp_const_none;
}

extern mp_obj_t BreakoutEncoderWheel_clear(mp_obj_t self_in) {
    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    self->breakout->clear();

    return mp_const_none;
}

extern mp_obj_t BreakoutEncoderWheel_show(mp_obj_t self_in) {
    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    self->breakout->show();

    return mp_const_none;
}

extern mp_obj_t BreakoutEncoderWheel_gpio_pin_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
enum { ARG_self, ARG_gpio, ARG_mode };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_gpio, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_mode, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    int gpio = args[ARG_gpio].u_int;
    if(gpio < 7 || gpio > 9) {
        mp_raise_ValueError("gpio out of range. Expected GP7 (7), GP8 (8), or GP9 (9)");
    }

    if(args[ARG_mode].u_obj == mp_const_none) {
        return mp_obj_new_int(self->breakout->gpio_pin_mode(gpio));
    }
    else {
        int mode = mp_obj_get_int(args[ARG_mode].u_obj);
        self->breakout->gpio_pin_mode(gpio, mode);

        return mp_const_none;
    }
}

extern mp_obj_t BreakoutEncoderWheel_gpio_pin_value(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_gpio, ARG_value, ARG_load, ARG_wait_for_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_gpio, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_value, MP_ARG_OBJ, { .u_obj = mp_const_none }},
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
        { MP_QSTR_wait_for_load, MP_ARG_BOOL, { .u_bool = false }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    int gpio = args[ARG_gpio].u_int;
    if(gpio < 7 || gpio > 9) {
        mp_raise_ValueError("gpio out of range. Expected GP7 (7), GP8 (8), or GP9 (9)");
    }

    if(args[ARG_value].u_obj == mp_const_none) {
        if(self->breakout->gpio_pin_mode(gpio) == IOExpander::PIN_ADC) {
            return mp_obj_new_float(self->breakout->gpio_pin_value_as_voltage(gpio));
        }
        else {
            return mp_obj_new_int(self->breakout->gpio_pin_value(gpio));
        }
    }
    else {
        int value = mp_obj_get_int(args[ARG_value].u_obj);
        bool load = args[ARG_load].u_bool;
        bool wait_for_load = args[ARG_wait_for_load].u_bool;
        self->breakout->gpio_pin_value(gpio, value, load, wait_for_load);

        return mp_const_none;
    }
}

extern mp_obj_t BreakoutEncoderWheel_gpio_pwm_load(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_wait_for_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_wait_for_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);

    bool wait_for_load = args[ARG_wait_for_load].u_bool;
    self->breakout->gpio_pwm_load(wait_for_load);

    return mp_const_none;
}

extern mp_obj_t BreakoutEncoderWheel_gpio_pwm_frequency(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_frequency, ARG_load, ARG_wait_for_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_frequency, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
        { MP_QSTR_wait_for_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_encoder_wheel_BreakoutEncoderWheel_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_encoder_wheel_BreakoutEncoderWheel_obj_t);
    float frequency = mp_obj_get_float(args[ARG_frequency].u_obj);
    uint32_t period = (uint32_t)(IOExpander::CLOCK_FREQ / frequency);
    if (period / 128 > IOExpander::MAX_PERIOD) {
      mp_raise_ValueError("The provided frequency is too low");
    }
    if (period < 2) {
      mp_raise_ValueError("The provided frequency is too high");
    }

    bool load = args[ARG_load].u_bool;
    bool wait_for_load = args[ARG_wait_for_load].u_bool;
    period = self->breakout->gpio_pwm_frequency(frequency, load, wait_for_load);

    return mp_obj_new_int(period);
}
}