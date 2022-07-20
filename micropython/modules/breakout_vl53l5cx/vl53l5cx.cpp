#include <cstdio>
#include "vl53l5cx.hpp"
#include "pico/multicore.h"
#include "micropython/modules/util.hpp"



extern "C" {
#include "vl53l5cx.h"
#include "pimoroni_i2c.h"
#include "py/stream.h"
#include "py/reader.h"
#include "extmod/vfs.h"

typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;

const mp_obj_float_t const_float_1 = {{&mp_type_float}, 1.0f};

/***** Variables Struct *****/
typedef struct _VL53L5CX_obj_t {
    mp_obj_base_t base;
    _PimoroniI2C_obj_t *i2c;
    pimoroni::VL53L5CX* breakout;
    void *configuration;
    void *motion_configuration;
    void *results;
} _VL53L5CX_obj_t;


/***** Destructor ******/
mp_obj_t VL53L5CX___del__(mp_obj_t self_in) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    //self->breakout->stop_ranging();  // i2c object might have been deleted already?
    // TODO: Since we're now holding a pointer to the I2C object it *should* still be available here?
    m_del_class(VL53L5CX, self->breakout);
    return mp_const_none;
}

/***** Constructor *****/
mp_obj_t VL53L5CX_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _VL53L5CX_obj_t *self = nullptr;

    enum { 
        ARG_i2c,
        ARG_addr,
        ARG_firmware,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_addr, MP_ARG_INT, {.u_int = pimoroni::VL53L5CX::DEFAULT_ADDRESS} },
        { MP_QSTR_firmware, MP_ARG_OBJ, {.u_obj = nullptr} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int addr = args[ARG_addr].u_int;

    self = m_new_obj_with_finaliser(_VL53L5CX_obj_t);
    self->base.type = &VL53L5CX_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);

    self->configuration = m_new(VL53L5CX_Configuration, 1);
    self->motion_configuration = m_new(VL53L5CX_Motion_Configuration, 1);
    self->results = m_new(pimoroni::VL53L5CX::ResultsData, 1);

    mp_buffer_info_t bufinfo;
    const size_t firmware_size = 84 * 1024;

    if(args[ARG_firmware].u_obj == nullptr) {
        mp_obj_t args[2] = {
            mp_obj_new_str("vl53l5cx_firmware.bin", strlen("vl53l5cx_firmware.bin")),
            MP_OBJ_NEW_QSTR(MP_QSTR_r),
        };
        bufinfo.buf = (void *)m_new(uint8_t, firmware_size);
        mp_obj_t file = mp_vfs_open(MP_ARRAY_SIZE(args), &args[0], (mp_map_t *)&mp_const_empty_map);
        int errcode;
        bufinfo.len = mp_stream_rw(file, bufinfo.buf, firmware_size, &errcode, MP_STREAM_RW_READ | MP_STREAM_RW_ONCE);
        if (errcode != 0) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("OSError reading vl53l5cx_firmware.bin"));
        }
    } else {
        mp_get_buffer_raise(args[ARG_firmware].u_obj, &bufinfo, MP_BUFFER_READ);
    }

    if(bufinfo.len != (size_t)(firmware_size)) {  // firmware blob is always 84K
        mp_raise_ValueError("Firmware must be 84k bytes!");
    }

    self->breakout = m_new_class(pimoroni::VL53L5CX, (pimoroni::I2C*)self->i2c->i2c, (uint8_t *)bufinfo.buf, addr, self->configuration, self->motion_configuration);

    if(!self->breakout->init()) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: init error");
    }

    if(args[ARG_firmware].u_obj == nullptr) {
        m_free(bufinfo.buf);
    }

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t VL53L5CX_start_ranging(mp_obj_t self_in) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    bool status = self->breakout->start_ranging();
    if(!status) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: start_ranging error");
    }
    return mp_const_none;
}

mp_obj_t VL53L5CX_stop_ranging(mp_obj_t self_in) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    bool status = self->breakout->stop_ranging();
    if(!status) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: stop_ranging error");
    }
    return mp_const_none;
}

mp_obj_t VL53L5CX_enable_motion_indicator(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    bool status = self->breakout->enable_motion_indicator((pimoroni::VL53L5CX::Resolution)mp_obj_get_int(value));
    if(!status) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: enable_motion_indicator error");
    }
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_motion_distance(mp_obj_t self_in, mp_obj_t distance_min, mp_obj_t distance_max) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    bool status = self->breakout->set_motion_distance(mp_obj_get_int(distance_min), mp_obj_get_int(distance_max));
    if(!status) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: set_motion_distance error");
    }
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_i2c_address(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    bool status = self->breakout->set_i2c_address(mp_obj_get_int(value));
    if(!status) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: set_i2c_address error");
    }
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_ranging_mode(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    bool status = self->breakout->set_ranging_mode((pimoroni::VL53L5CX::RangingMode)mp_obj_get_int(value));
    if(!status) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: set_ranging_mode error");
    }
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_ranging_frequency_hz(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    bool status = self->breakout->set_ranging_frequency_hz(mp_obj_get_int(value));
    if(!status) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: set_ranging_frequency_hz error");
    }
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_resolution(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    bool status = self->breakout->set_resolution((pimoroni::VL53L5CX::Resolution)mp_obj_get_int(value));
    if(!status) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: set_resolution error");
    }
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_integration_time_ms(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    bool status = self->breakout->set_integration_time_ms(mp_obj_get_int(value));
    if(!status) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: set_integration_time_ms error");
    }
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_sharpener_percent(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    bool status = self->breakout->set_sharpener_percent(mp_obj_get_int(value));
    if(!status) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: set_sharpener_percent error");
    }
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_target_order(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    bool status = self->breakout->set_target_order((pimoroni::VL53L5CX::TargetOrder)mp_obj_get_int(value));
    if(!status) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: set_target_order error");
    }
    return mp_const_none;
}

mp_obj_t VL53L5CX_set_power_mode(mp_obj_t self_in, mp_obj_t value) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    bool status = self->breakout->set_power_mode((pimoroni::VL53L5CX::PowerMode)mp_obj_get_int(value));
    if(!status) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: set_power_mode error");
    }
    return mp_const_none;
}

mp_obj_t VL53L5CX_data_ready(mp_obj_t self_in) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    return self->breakout->data_ready() ? mp_const_true : mp_const_false;
}

mp_obj_t VL53L5CX_get_data(mp_obj_t self_in) {
    _VL53L5CX_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VL53L5CX_obj_t);
    pimoroni::VL53L5CX::ResultsData *results = (pimoroni::VL53L5CX::ResultsData *)self->results;
    bool status = self->breakout->get_data(results);
    if(!status) {
        mp_raise_msg(&mp_type_RuntimeError, "VL53L5CX: get_data error");
    }

    // Get the current resolution so we only look at valid results.
    // This is so that our average distance works out and doesn't include bogus data.
    int scale = (uint8_t)self->breakout->get_resolution();
    int tuple_size = scale * VL53L5CX_NB_TARGET_PER_ZONE;

    // TODO This doesn't really handle VL53L5CX_NB_TARGET_PER_ZONE > 1 gracefully
    // the zone data should be split into separate tuples

    mp_obj_t tuple_distance_mm[tuple_size];
    mp_obj_t tuple_reflectance[tuple_size];

    int32_t average_distance = 0;
    int32_t average_reflectance = 0;

    // Build a tuple of motion data
    for(int i = 0u; i < tuple_size; i++) {
        tuple_distance_mm[i] = mp_obj_new_int(results->distance_mm[i]);
        tuple_reflectance[i] = mp_obj_new_int(results->reflectance[i]);
        average_distance += results->distance_mm[i];
        average_reflectance += results->reflectance[i];
    }

    average_distance /= tuple_size;
    average_reflectance /= tuple_size;

    mp_obj_t tuple_motion_data[32];

    for(int i = 0u; i < 32; i++) {
        tuple_motion_data[i] = mp_obj_new_int(results->motion_indicator.motion[i]);
    }

    STATIC const qstr tuple_motion_fields[] = {MP_QSTR_global_indicator_1, MP_QSTR_global_indicator_2, MP_QSTR_motion};

    mp_obj_t tuple_motion[] = {
        mp_obj_new_int(results->motion_indicator.global_indicator_1),
        mp_obj_new_int(results->motion_indicator.global_indicator_2),
        mp_obj_new_tuple(sizeof(tuple_motion_data) / sizeof(mp_obj_t), tuple_motion_data)
    };

    mp_obj_t tuple[] = {
        mp_obj_new_int(average_distance), // Average distance
        mp_obj_new_int(average_reflectance), // Average reflectance
        mp_obj_new_attrtuple(tuple_motion_fields, sizeof(tuple_motion) / sizeof(mp_obj_t), tuple_motion), // Motion data
        mp_obj_new_int(tuple_size), // Number of results
        mp_obj_new_tuple(tuple_size, tuple_distance_mm), // Full distance results
        mp_obj_new_tuple(tuple_size, tuple_reflectance)  // Full reflectange results
    };

    STATIC const qstr tuple_fields[] = {MP_QSTR_distance_avg, MP_QSTR_reflectance_avg, MP_QSTR_motion_indicator, MP_QSTR_results, MP_QSTR_distance, MP_QSTR_reflectance};

    return mp_obj_new_attrtuple(tuple_fields, sizeof(tuple) / sizeof(mp_obj_t), tuple);
}

}