#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "scd4x_i2c.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"
#include "common/pimoroni_i2c.hpp"

using namespace pimoroni;

bool scd41_initialised = false;


extern "C" {
#include "breakout_scd41.h"
#include "pimoroni_i2c.h"

#define NOT_INITIALISED_MSG     "SCD41: Not initialised. Call scd41.init(<i2c instance>) first."
#define READ_FAIL_MSG           "SCD41: Reading failed."
#define FAIL_MSG                "SCD41: Error."
#define SAMPLE_FAIL_MSG         "SCD41: Read invalid sample."

mp_obj_t scd41_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum { ARG_i2c };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _PimoroniI2C_obj_t *i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    sensirion_i2c_hal_init((pimoroni::I2C*)i2c->i2c);
    scd4x_stop_periodic_measurement();
    scd4x_reinit();
    scd41_initialised = true;

    return mp_const_none;
}

mp_obj_t scd41_stop_periodic_measurement() {
    if(!scd41_initialised) {
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
        return mp_const_none;
    }
    scd4x_stop_periodic_measurement();
    return mp_const_none;
}

mp_obj_t scd41_start_periodic_measurement() {
    if(!scd41_initialised) {
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
        return mp_const_none;
    }
    int error = scd4x_start_periodic_measurement();
    if(error) {
        mp_raise_msg(&mp_type_RuntimeError, FAIL_MSG);
    }

    return mp_const_none;
}

mp_obj_t scd41_get_data_ready() {
    if(!scd41_initialised) {
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
        return mp_const_none;
    }
    uint16_t data_ready = 0;
    int error = scd4x_get_data_ready_status(&data_ready);
    if(error) {
        mp_raise_msg(&mp_type_RuntimeError, READ_FAIL_MSG);
        return mp_const_none;
    }
    // The datasheet doesn't really say *which* bit might be 1 if data is ready...
    // so check if the least significant eleven bits are != 0
    return (data_ready & 0x7ff) ? mp_const_true : mp_const_false;
}

mp_obj_t scd41_set_temperature_offset(mp_obj_t offset) {
    if(!scd41_initialised) {
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
        return mp_const_none;
    }
    float o = mp_obj_get_float(offset);
    int error = scd4x_set_temperature_offset(o);
    if(error) {
        mp_raise_msg(&mp_type_RuntimeError, FAIL_MSG);
    }

    return mp_const_none;
}

mp_obj_t scd41_get_temperature_offset() {
    if(!scd41_initialised) {
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
        return mp_const_none;
    }

    int32_t t_offset;
    int error = scd4x_get_temperature_offset(&t_offset);
    if(error) {
        mp_raise_msg(&mp_type_RuntimeError, FAIL_MSG);
        return mp_const_none;
    }

    return mp_obj_new_int(t_offset);
}

mp_obj_t scd41_set_sensor_altitude(mp_obj_t altitude) {
    if(!scd41_initialised) {
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
        return mp_const_none;
    }
    int a = mp_obj_get_int(altitude);
    int error = scd4x_set_sensor_altitude(a);
    if(error) {
        mp_raise_msg(&mp_type_RuntimeError, FAIL_MSG);
    }

    return mp_const_none;
}

mp_obj_t scd41_set_ambient_pressure(mp_obj_t pressure) {
    if(!scd41_initialised) {
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
        return mp_const_none;
    }
    int p = mp_obj_get_int(pressure);
    int error = scd4x_set_ambient_pressure(p);
    if(error) {
        mp_raise_msg(&mp_type_RuntimeError, FAIL_MSG);
    }

    return mp_const_none;
}


mp_obj_t scd41_read_measurement() {
    uint16_t co2;
    int32_t temperature;
    int32_t humidity;
    if(!scd41_initialised) {
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
        return mp_const_none;
    }
    int  error = scd4x_read_measurement(&co2, &temperature, &humidity);
    if(error) {
        mp_raise_msg(&mp_type_RuntimeError, READ_FAIL_MSG);
        return mp_const_none;
    }

    if(co2 == 0) {
        mp_raise_msg(&mp_type_RuntimeError, SAMPLE_FAIL_MSG);
        return mp_const_none;
    }

    mp_obj_t tuple[3];
    tuple[0] = mp_obj_new_float(co2);
    tuple[1] = mp_obj_new_float(temperature / 1000.0f);
    tuple[2] = mp_obj_new_float(humidity / 1000.0f);
    return mp_obj_new_tuple(3, tuple);
}
}
