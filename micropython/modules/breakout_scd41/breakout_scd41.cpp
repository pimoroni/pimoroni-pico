#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "scd4x_i2c.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"
#include "common/pimoroni_i2c.hpp"
#include "micropython/modules/util.hpp"

using namespace pimoroni;

extern "C" {
#include "breakout_scd41.h"
#include "pimoroni_i2c.h"

#define READ_FAIL_MSG           MP_ERROR_TEXT("SCD41: Reading failed.")
#define FAIL_MSG                MP_ERROR_TEXT("SCD41: Error.")
#define SAMPLE_FAIL_MSG         MP_ERROR_TEXT("SCD41: Read invalid sample.")

/***** Variables Struct *****/
typedef struct _breakout_scd41_BreakoutSCD41_obj_t {
    mp_obj_base_t base;
    _PimoroniI2C_obj_t *i2c;
} breakout_scd41_BreakoutSCD41_obj_t;

// The Sensirion driver keeps the bus in a global, so point it at this instance's
// I2C before every command. Holding i2c on the instance is also what keeps it
// reachable for the garbage collector.
static void bind(mp_obj_t self_in) {
    breakout_scd41_BreakoutSCD41_obj_t *self = MP_OBJ_TO_PTR2(self_in, breakout_scd41_BreakoutSCD41_obj_t);
    sensirion_i2c_hal_init((pimoroni::I2C *)(self->i2c->i2c));
}

/***** Constructor *****/
mp_obj_t BreakoutSCD41_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    breakout_scd41_BreakoutSCD41_obj_t *self = nullptr;

    enum { ARG_i2c };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = mp_obj_malloc(breakout_scd41_BreakoutSCD41_obj_t, &breakout_scd41_BreakoutSCD41_type);

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    sensirion_i2c_hal_init((pimoroni::I2C *)(self->i2c->i2c));
    scd4x_init(SCD41_I2C_ADDR_62);
    scd4x_stop_periodic_measurement();
    if(scd4x_reinit()) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("BreakoutSCD41: breakout not found when initialising"));
    }

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t BreakoutSCD41_stop_periodic_measurement(mp_obj_t self_in) {
    bind(self_in);
    scd4x_stop_periodic_measurement();
    return mp_const_none;
}

mp_obj_t BreakoutSCD41_start_periodic_measurement(mp_obj_t self_in) {
    bind(self_in);
    if(scd4x_start_periodic_measurement()) {
        mp_raise_msg(&mp_type_RuntimeError, FAIL_MSG);
    }

    return mp_const_none;
}

mp_obj_t BreakoutSCD41_start_low_power_periodic_measurement(mp_obj_t self_in) {
    bind(self_in);
    if(scd4x_start_low_power_periodic_measurement()) {
        mp_raise_msg(&mp_type_RuntimeError, FAIL_MSG);
    }

    return mp_const_none;
}

mp_obj_t BreakoutSCD41_get_data_ready(mp_obj_t self_in) {
    bind(self_in);
    bool data_ready = false;
    if(scd4x_get_data_ready_status(&data_ready)) {
        mp_raise_msg(&mp_type_RuntimeError, READ_FAIL_MSG);
    }
    return data_ready ? mp_const_true : mp_const_false;
}

/*
    To successfully conduct an accurate forced recalibration, the following
    steps need to be carried out:
    1. Operate the SCD4x in a periodic measurement mode for > 3 minutes in an
    environment with homogenous and constant CO₂ concentration.
    2. Stop periodic measurement. Wait 500 ms.
    3. Subsequently issue the perform_forced_recalibration command and
    optionally read out the baseline correction. A return value of 0xffff
    indicates that the forced recalibration failed.
 */
mp_obj_t BreakoutSCD41_perform_forced_recalibration(mp_obj_t self_in, mp_obj_t target_co2_concentration_in) {
    bind(self_in);
    uint16_t frc_correction;
    uint16_t target_co2_concentration = mp_obj_get_int(target_co2_concentration_in);
    if(scd4x_perform_forced_recalibration(target_co2_concentration, &frc_correction)) {
        mp_raise_msg(&mp_type_RuntimeError, FAIL_MSG);
    }
    // FRC correction value in CO₂ ppm or 0xFFFF if the
    // command failed. Convert value to CO₂ ppm with: value - 0x8000
    return frc_correction == 0xffff ? mp_const_none : mp_obj_new_int(frc_correction - 0x8000);
}

mp_obj_t BreakoutSCD41_set_automatic_self_calibration(mp_obj_t self_in, mp_obj_t asc_enabled) {
    bind(self_in);
    if(scd4x_set_automatic_self_calibration_enabled(asc_enabled == mp_const_true ? 1u : 0u)) {
        mp_raise_msg(&mp_type_RuntimeError, FAIL_MSG);
    }

    return mp_const_none;
}

mp_obj_t BreakoutSCD41_get_automatic_self_calibration(mp_obj_t self_in) {
    bind(self_in);
    uint16_t asc_enabled;
    if(scd4x_get_automatic_self_calibration_enabled(&asc_enabled)) {
        mp_raise_msg(&mp_type_RuntimeError, FAIL_MSG);
    }

    return asc_enabled ? mp_const_true : mp_const_false;
}

mp_obj_t BreakoutSCD41_set_temperature_offset(mp_obj_t self_in, mp_obj_t offset) {
    bind(self_in);
    float o = mp_obj_get_float(offset);
    if(o < 0.0f || o > 175.0f) {
        mp_raise_ValueError(MP_ERROR_TEXT("offset out of range. Expected 0.0 to 175.0 degrees C"));
    }
    if(scd4x_set_temperature_offset_raw((uint16_t)(((o * 65535.0f) / 175.0f) + 0.5f))) {
        mp_raise_msg(&mp_type_RuntimeError, FAIL_MSG);
    }

    return mp_const_none;
}

mp_obj_t BreakoutSCD41_get_temperature_offset(mp_obj_t self_in) {
    bind(self_in);
    uint16_t t_offset_raw;
    if(scd4x_get_temperature_offset_raw(&t_offset_raw)) {
        mp_raise_msg(&mp_type_RuntimeError, FAIL_MSG);
    }

    return mp_obj_new_float(((float)t_offset_raw * 175.0f) / 65535.0f);
}

mp_obj_t BreakoutSCD41_set_sensor_altitude(mp_obj_t self_in, mp_obj_t altitude) {
    bind(self_in);
    if(scd4x_set_sensor_altitude(mp_obj_get_int(altitude))) {
        mp_raise_msg(&mp_type_RuntimeError, FAIL_MSG);
    }

    return mp_const_none;
}

mp_obj_t BreakoutSCD41_set_ambient_pressure(mp_obj_t self_in, mp_obj_t pressure) {
    bind(self_in);
    if(scd4x_set_ambient_pressure_raw(mp_obj_get_int(pressure))) {
        mp_raise_msg(&mp_type_RuntimeError, FAIL_MSG);
    }

    return mp_const_none;
}

mp_obj_t BreakoutSCD41_read_measurement(mp_obj_t self_in) {
    bind(self_in);
    uint16_t co2;
    int32_t temperature;
    int32_t humidity;
    if(scd4x_read_measurement(&co2, &temperature, &humidity)) {
        mp_raise_msg(&mp_type_RuntimeError, READ_FAIL_MSG);
    }

    if(co2 == 0) {
        mp_raise_msg(&mp_type_RuntimeError, SAMPLE_FAIL_MSG);
    }

    mp_obj_t tuple[3];
    tuple[0] = mp_obj_new_float(co2);
    tuple[1] = mp_obj_new_float(temperature / 1000.0f);
    tuple[2] = mp_obj_new_float(humidity / 1000.0f);
    return mp_obj_new_tuple(3, tuple);
}
}
