#include "libraries/breakout_sgp30/breakout_sgp30.hpp"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))


using namespace pimoroni;

extern "C" {
#include "breakout_sgp30.h"

/***** Variables Struct *****/
typedef struct _breakout_sgp30_BreakoutSGP30_obj_t {
    mp_obj_base_t base;
    BreakoutSGP30 *breakout;
} breakout_sgp30_BreakoutSGP30_obj_t;

/***** Print *****/
void BreakoutSGP30_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter    
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);
    BreakoutSGP30* breakout = self->breakout;
    mp_print_str(print, "BreakoutSGP30(");

    mp_print_str(print, "i2c = ");
    mp_obj_print_helper(print, mp_obj_new_int((breakout->get_i2c() == i2c0) ? 0 : 1), PRINT_REPR);

    mp_print_str(print, ", sda = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_sda()), PRINT_REPR);

    mp_print_str(print, ", scl = ");
    mp_obj_print_helper(print, mp_obj_new_int(breakout->get_scl()), PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Constructor *****/
mp_obj_t BreakoutSGP30_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = nullptr;

    enum { ARG_i2c, ARG_sda, ARG_scl };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_sda, MP_ARG_INT, {.u_int = I2C_DEFAULT_SDA} },
        { MP_QSTR_scl, MP_ARG_INT, {.u_int = I2C_DEFAULT_SCL} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get I2C bus.
    int i2c_id = args[ARG_i2c].u_int;
    int sda = args[ARG_sda].u_int;
    int scl = args[ARG_scl].u_int;

    if(i2c_id == -1) {
        i2c_id = (sda >> 1) & 0b1;  // If no i2c specified, choose the one for the given SDA pin
    }
    if(i2c_id < 0 || i2c_id > 1) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C(%d) doesn't exist"), i2c_id);
    }

    if(!IS_VALID_SDA(i2c_id, sda)) {
        mp_raise_ValueError(MP_ERROR_TEXT("bad SDA pin"));
    }

    if(!IS_VALID_SCL(i2c_id, scl)) {
        mp_raise_ValueError(MP_ERROR_TEXT("bad SCL pin"));
    }

    self = m_new_obj(breakout_sgp30_BreakoutSGP30_obj_t);
    self->base.type = &breakout_sgp30_BreakoutSGP30_type;
    
    i2c_inst_t *i2c = (i2c_id == 0) ? i2c0 : i2c1;
    self->breakout = new BreakoutSGP30(i2c, sda, scl);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "SGP30 breakout not found when initialising");
    }

    return MP_OBJ_FROM_PTR(self);
}

/***** Methods *****/
mp_obj_t BreakoutSGP30_retrieve_unique_id(mp_obj_t self_in) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);
    return mp_obj_new_bool(self->breakout->retrieve_unique_id());
}

mp_obj_t BreakoutSGP30_get_unique_id(mp_obj_t self_in) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);
    uint64_t id = self->breakout->get_unique_id();

    mp_obj_t tuple[3];
    tuple[0] = mp_obj_new_int((id >> 32) & 0xffff);
    tuple[1] = mp_obj_new_int((id >> 16) & 0xffff);
    tuple[2] = mp_obj_new_int(id & 0xffff);
    return mp_obj_new_tuple(3, tuple);
}

mp_obj_t BreakoutSGP30_start_measurement(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_wait_for_setup };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_wait_for_setup, MP_ARG_REQUIRED | MP_ARG_BOOL },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_sgp30_BreakoutSGP30_obj_t);
    self->breakout->start_measurement(args[ARG_wait_for_setup].u_bool);

    return mp_const_none;
}

mp_obj_t BreakoutSGP30_get_air_quality(mp_obj_t self_in) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);

    uint16_t eCO2 = 0, TVOC = 0;
    if(self->breakout->get_air_quality(&eCO2, &TVOC)) {
        mp_obj_t tuple[2];
        tuple[0] = mp_obj_new_int(eCO2);
        tuple[1] = mp_obj_new_int(TVOC);
        return mp_obj_new_tuple(2, tuple);
    }
    else
        return mp_const_none;
}

mp_obj_t BreakoutSGP30_get_air_quality_raw(mp_obj_t self_in) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);

    uint16_t rawH2 = 0, rawEthanol = 0;
    if(self->breakout->get_air_quality_raw(&rawH2, &rawEthanol)) {
        mp_obj_t tuple[2];
        tuple[0] = mp_obj_new_int(rawH2);
        tuple[1] = mp_obj_new_int(rawEthanol);
        return mp_obj_new_tuple(2, tuple);
    }
    else
        return mp_const_none;
}

mp_obj_t BreakoutSGP30_soft_reset(mp_obj_t self_in) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);
    self->breakout->soft_reset();

    return mp_const_none;
}

mp_obj_t BreakoutSGP30_get_baseline(mp_obj_t self_in) {
    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_sgp30_BreakoutSGP30_obj_t);
    
    uint16_t eCO2 = 0, TVOC = 0;
    if(self->breakout->get_baseline(&eCO2, &TVOC)) {
        mp_obj_t tuple[2];
        tuple[0] = mp_obj_new_int(eCO2);
        tuple[1] = mp_obj_new_int(TVOC);
        return mp_obj_new_tuple(2, tuple);
    }
    else
        return mp_const_none;

    return mp_const_none;
}

mp_obj_t BreakoutSGP30_set_baseline(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_eco2, ARG_tvoc };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_eco2, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_tvoc, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_sgp30_BreakoutSGP30_obj_t);
    self->breakout->set_baseline(args[ARG_eco2].u_int, args[ARG_tvoc].u_int);

    return mp_const_none;
}

mp_obj_t BreakoutSGP30_set_humidity(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_absolute_humidity };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_absolute_humidity, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    breakout_sgp30_BreakoutSGP30_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, breakout_sgp30_BreakoutSGP30_obj_t);
    self->breakout->set_humidity(args[ARG_absolute_humidity].u_int);

    return mp_const_none;
}
}