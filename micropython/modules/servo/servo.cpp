#include "drivers/servo/servo.hpp"
#include "drivers/servo/servo_cluster.hpp"
#include <cstdio>

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))


using namespace servo;

extern "C" {
#include "servo.h"
#include "py/builtin.h"

typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;

const mp_obj_float_t const_float_1 = {{&mp_type_float}, 1.0f};

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
    mp_print_str(print, ", value = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->servo->get_value()), PRINT_REPR);
    mp_print_str(print, ", pulse = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->servo->get_pulse()), PRINT_REPR);
    mp_print_str(print, ", enabled = ");
    mp_obj_print_helper(print, self->servo->is_enabled() ? mp_const_true : mp_const_false, PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Destructor ******/
mp_obj_t Servo___del__(mp_obj_t self_in) {
    _Servo_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Servo_obj_t);
    delete self->servo;
    return mp_const_none;
}

/***** Constructor *****/
mp_obj_t Servo_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _Servo_obj_t *self = nullptr;

    enum { ARG_pin, ARG_type };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_type, MP_ARG_INT, {.u_int = (uint8_t)servo::CalibrationType::ANGULAR} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int pin = args[ARG_pin].u_int;
    servo::CalibrationType calibration_type = (servo::CalibrationType)args[ARG_type].u_int;

    self = m_new_obj_with_finaliser(_Servo_obj_t);
    self->base.type = &Servo_type;

    self->servo = new Servo(pin, calibration_type);
    self->servo->init();

    return MP_OBJ_FROM_PTR(self);
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


/********** ServoCluster **********/

/***** Variables Struct *****/
typedef struct _ServoCluster_obj_t {
    mp_obj_base_t base;
    ServoCluster* cluster;
} _ServoCluster_obj_t;


/***** Print *****/
void ServoCluster_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    //_ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _ServoCluster_obj_t);
    mp_print_str(print, "ServoCluster(");

    // TODO
    //mp_print_str(print, "num_leds = ");
    //mp_obj_print_helper(print, mp_obj_new_int(self->apa102->num_leds), PRINT_REPR);

    mp_print_str(print, ")");
}

/***** Destructor ******/
mp_obj_t ServoCluster___del__(mp_obj_t self_in) {
    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _ServoCluster_obj_t);
    delete self->cluster;
    return mp_const_none;
}

/***** Constructor *****/
mp_obj_t ServoCluster_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _ServoCluster_obj_t *self = nullptr;

    // TODO
    /*enum { 
        ARG_num_leds,
        ARG_pio,
        ARG_sm,
        ARG_dat,
        ARG_clk,
        ARG_freq,
        ARG_buffer
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_num_leds, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pio, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_sm, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_dat, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_clk, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = APA102::DEFAULT_SERIAL_FREQ} },
        { MP_QSTR_buffer, MP_ARG_OBJ, {.u_obj = nullptr} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int num_leds = args[ARG_num_leds].u_int;
    PIO pio = args[ARG_pio].u_int == 0 ? pio0 : pio1;
    int sm = args[ARG_sm].u_int;
    int dat = args[ARG_dat].u_int;
    int clk = args[ARG_clk].u_int;
    int freq = args[ARG_freq].u_int;

    APA102::RGB *buffer = nullptr;

    if (args[ARG_buffer].u_obj) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);
        buffer = (APA102::RGB *)bufinfo.buf;
        if(bufinfo.len < (size_t)(num_leds * 4)) {
            mp_raise_ValueError("Supplied buffer is too small for LED count!");
        }
        // If a bytearray is supplied it'll be raw, uninitialized bytes
        // iterate through the RGB elements and call "brightness"
        // to set up the SOF bytes, otherwise a flickery mess will happen!
        // Oh for such niceties as "placement new"...
        for(auto i = 0; i < num_leds; i++) {
            buffer[i].brightness(15);
        }
    }*/

    self = m_new_obj_with_finaliser(_ServoCluster_obj_t);
    self->base.type = &ServoCluster_type;

    self->cluster = new ServoCluster(pio1, 0, 0b11111100); //TODO Expose parameters

    return MP_OBJ_FROM_PTR(self);
}

/*
mp_obj_t ServoCluster_clear(mp_obj_t self_in) {
    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _ServoCluster_obj_t);
    self->apa102->clear();
    return mp_const_none;
}

mp_obj_t ServoCluster_update(mp_obj_t self_in) {
    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _ServoCluster_obj_t);
    self->apa102->update(true);
    return mp_const_none;
}

mp_obj_t ServoCluster_start(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_fps };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_fps, MP_ARG_INT, {.u_int = 60} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);
    self->apa102->start(args[ARG_fps].u_int);

    return mp_const_none;
}

mp_obj_t ServoCluster_set_brightness(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_brightness };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_brightness, MP_ARG_REQUIRED | MP_ARG_INT }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int brightness = args[ARG_brightness].u_int;

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);
    self->apa102->set_brightness(brightness);

    return mp_const_none;
}

mp_obj_t ServoCluster_set_rgb(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_index, ARG_r, ARG_g, ARG_b };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_g, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int index = args[ARG_index].u_int;
    int r = args[ARG_r].u_int;
    int g = args[ARG_g].u_int;
    int b = args[ARG_b].u_int;

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);
    self->apa102->set_rgb(index, r, g, b);

    return mp_const_none;
}

mp_obj_t ServoCluster_set_hsv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_index, ARG_h, ARG_s, ARG_v };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_hue, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sat, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&const_float_1)} },
        { MP_QSTR_val, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&const_float_1)} }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int index = args[ARG_index].u_int;
    float h = mp_obj_get_float(args[ARG_h].u_obj);
    float s = mp_obj_get_float(args[ARG_s].u_obj);
    float v = mp_obj_get_float(args[ARG_v].u_obj);

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);
    self->apa102->set_hsv(index, h, s, v);

    return mp_const_none;
}

mp_obj_t ServoCluster_get(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_index };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int index = args[ARG_index].u_int;

    _ServoCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _ServoCluster_obj_t);
    APA102::RGB rgb = self->apa102->get(index);

    mp_obj_t tuple[4];
    tuple[0] = mp_obj_new_int(rgb.r);
    tuple[1] = mp_obj_new_int(rgb.g);
    tuple[2] = mp_obj_new_int(rgb.b);
    tuple[3] = mp_obj_new_int(rgb.sof);
    return mp_obj_new_tuple(4, tuple);
}
*/
}
