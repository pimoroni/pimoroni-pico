#include <cstdio>
#include "vl53l5cx.hpp"
#include "pico/multicore.h"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))


extern "C" {
#include "vl53l5cx.h"
#include "pimoroni_i2c.h"

typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;

const mp_obj_float_t const_float_1 = {{&mp_type_float}, 1.0f};

/***** I2C Struct *****/
typedef struct _PimoroniI2C_obj_t {
    mp_obj_base_t base;
    pimoroni::I2C *i2c;
} _PimoroniI2C_obj_t;


/***** Variables Struct *****/
typedef struct _VL53L5CX_obj_t {
    mp_obj_base_t base;
    _PimoroniI2C_obj_t *i2c;
    pimoroni::VL53L5CX* breakout;
} _VL53L5CX_obj_t;


/***** Print *****/
void VL53L5CX_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; // Unused input parameter
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    mp_print_str(print, "VL53L5CX( ");


    mp_print_str(print, "i2c = ");
    mp_obj_print_helper(print, mp_obj_new_int((self->breakout->get_configuration()->platform.i2c == i2c0) ? 0 : 1), PRINT_REPR);

    mp_print_str(print, " addr = ");
    mp_obj_print_helper(print, mp_obj_new_int(self->breakout->get_configuration()->platform.address), PRINT_REPR);

    mp_print_str(print, " )");
}

/***** Destructor ******/
mp_obj_t VL53L5CX___del__(mp_obj_t self_in) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    self->breakout->stop_ranging();
    delete self->breakout;
    return mp_const_none;
}

/***** Constructor *****/
mp_obj_t VL53L5CX_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _VL53L5CX_obj_t *self = nullptr;

    enum { 
        ARG_i2c,
        ARG_addr
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_addr, MP_ARG_INT, {.u_int = pimoroni::VL53L5CX::DEFAULT_ADDRESS} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!MP_OBJ_IS_TYPE(args[ARG_i2c].u_obj, &PimoroniI2C_type)) {
        mp_raise_ValueError(MP_ERROR_TEXT("VL53L5CX: Bad i2C object"));
        return mp_const_none;
    }

    _PimoroniI2C_obj_t *i2c = (_PimoroniI2C_obj_t *)MP_OBJ_TO_PTR(args[ARG_i2c].u_obj);
    int addr = args[ARG_addr].u_int;

    self = m_new_obj_with_finaliser(_VL53L5CX_obj_t);
    self->base.type = &VL53L5CX_type;
    self->i2c = i2c;
    self->breakout = new pimoroni::VL53L5CX(i2c->i2c, addr);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: error initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t VL53L5CX_start_ranging(mp_obj_t self_in) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    self->breakout->start_ranging();
    return mp_const_none;
}

mp_obj_t VL53L5CX_stop_ranging(mp_obj_t self_in) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    self->breakout->stop_ranging();
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_i2c_address(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    self->breakout->set_i2c_address(mp_obj_get_int(value));
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_ranging_mode(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    self->breakout->set_ranging_mode((pimoroni::VL53L5CX::RangingMode)mp_obj_get_int(value));
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_ranging_frequency_hz(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    self->breakout->set_ranging_frequency_hz(mp_obj_get_int(value));
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_resolution(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    self->breakout->set_resolution((pimoroni::VL53L5CX::Resolution)mp_obj_get_int(value));
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_integration_time_ms(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    self->breakout->set_integration_time_ms(mp_obj_get_int(value));
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_sharpener_percent(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    self->breakout->set_sharpener_percent(mp_obj_get_int(value));
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_target_order(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    self->breakout->set_target_order((pimoroni::VL53L5CX::TargetOrder)mp_obj_get_int(value));
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_power_mode(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    self->breakout->set_power_mode((pimoroni::VL53L5CX::PowerMode)mp_obj_get_int(value));
    return mp_const_none;
}

mp_obj_t VL53L5CX_data_ready(mp_obj_t self_in) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    return self->breakout->data_ready() ? mp_const_true : mp_const_false;
}

mp_obj_t VL53L5CX_get_data(mp_obj_t self_in) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    pimoroni::VL53L5CX::ResultsData results;
    self->breakout->get_data(&results);

    static const int TUPLE_SIZE = VL53L5CX_RESOLUTION_8X8 * VL53L5CX_NB_TARGET_PER_ZONE;

    mp_obj_t tuple_distance_mm[TUPLE_SIZE];
    mp_obj_t tuple_reflectance[TUPLE_SIZE];

    // Build a tuple of motion data
    for(int i = 0u; i < TUPLE_SIZE; i++) {
        tuple_distance_mm[i] = mp_obj_new_int(results.distance_mm[i]);
        tuple_reflectance[i] = mp_obj_new_int(results.reflectance[i]);
    }

    mp_obj_t tuple[3];
    tuple[0] = mp_obj_new_int(results.silicon_temp_degc);
    tuple[1] = mp_obj_new_tuple(TUPLE_SIZE, tuple_distance_mm);
    tuple[2] = mp_obj_new_tuple(TUPLE_SIZE, tuple_reflectance);

    return mp_obj_new_tuple(3, tuple);
}

}