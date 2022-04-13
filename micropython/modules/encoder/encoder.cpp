#include "drivers/encoder-pio/capture.hpp"
#include "drivers/encoder-pio/encoder.hpp"
#include <cstdio>

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

using namespace pimoroni;
//using namespace encoder;

extern "C" {
#include "encoder.h"
#include "py/builtin.h"


/********** Encoder **********/

/***** Variables Struct *****/
typedef struct _Encoder_obj_t {
    mp_obj_base_t base;
    Encoder* encoder;
} _Encoder_obj_t;


/***** Print *****/
void Encoder_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    mp_print_str(print, "Encoder(");

    mp_print_str(print, "pins = (");
    pin_pair pins = self->encoder->pins();
    mp_obj_print_helper(print, mp_obj_new_int(pins.a), PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, mp_obj_new_int(pins.b), PRINT_REPR);
    mp_print_str(print, "), enabled = ");
    /*mp_obj_print_helper(print, self->encoder->is_enabled() ? mp_const_true : mp_const_false, PRINT_REPR);
    mp_print_str(print, ", pulse = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->encoder->pulse()), PRINT_REPR);
    mp_print_str(print, ", value = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->encoder->value()), PRINT_REPR);
    mp_print_str(print, ", freq = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->encoder->frequency()), PRINT_REPR);
*/
    mp_print_str(print, ")");
}


/***** Constructor *****/
mp_obj_t Encoder_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _Encoder_obj_t *self = nullptr;

    enum { ARG_pio, ARG_sm, ARG_pins, ARG_pin_c, ARG_calibration, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pio, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_sm, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pins, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_pin_c, MP_ARG_INT, {.u_int = PIN_UNUSED} },
        { MP_QSTR_calibration, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_freq, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    PIO pio = args[ARG_pio].u_int == 0 ? pio0 : pio1;
    int sm = args[ARG_sm].u_int;

    size_t pin_count = 0;
    pin_pair pins;

    // Determine what pair of pins this encoder will use
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
        int a = mp_obj_get_int(items[0]);
        int b = mp_obj_get_int(items[1]);
        if((a < 0 || a >= (int)NUM_BANK0_GPIOS) ||
           (b < 0 || b >= (int)NUM_BANK0_GPIOS)) {
            mp_raise_ValueError("a pin in the list or tuple is out of range. Expected 0 to 29");
        }
        else if(a == b) {
            mp_raise_ValueError("cannot use the same pin for encoder A and B");
        }

        pins.a = (uint8_t)a;
        pins.b = (uint8_t)b;
    }
/*
    encoder::Calibration *calib = nullptr;
    encoder::CalibrationType calibration_type = encoder::CalibrationType::ANGULAR;
    const mp_obj_t calib_object = args[ARG_calibration].u_obj;
    if(calib_object != mp_const_none) {
        if(mp_obj_is_int(calib_object)) {
            int type = mp_obj_get_int(calib_object);
            if(type < 0 || type >= 3) {
                mp_raise_ValueError("type out of range. Expected ANGULAR (0), LINEAR (1) or CONTINUOUS (2)");
            }
            calibration_type = (encoder::CalibrationType)type;
        }
        else if(mp_obj_is_type(calib_object, &Calibration_type)) {
            calib = (MP_OBJ_TO_PTR2(calib_object, _Calibration_obj_t)->calibration);
        }
        else {
            mp_raise_TypeError("cannot convert object to an integer or a Calibration class instance");
        }
    }

    float freq = encoder::EncoderState::DEFAULT_FREQUENCY;
    if(args[ARG_freq].u_obj != mp_const_none) {
        freq = mp_obj_get_float(args[ARG_freq].u_obj);
    }
*/

    Encoder *encoder = new Encoder(pio, sm, pins, args[ARG_pin_c].u_int);
    if(!encoder->init()) {
        delete encoder;
        mp_raise_msg(&mp_type_RuntimeError, "unable to allocate the hardware resources needed to initialise this Encoder. Try running `import gc` followed by `gc.collect()` before creating it");
    }

    self = m_new_obj_with_finaliser(_Encoder_obj_t);
    self->base.type = &Encoder_type;
    self->encoder = encoder;

    return MP_OBJ_FROM_PTR(self);
}


/***** Destructor ******/
mp_obj_t Encoder___del__(mp_obj_t self_in) {
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    delete self->encoder;
    return mp_const_none;
}


/***** Methods *****/
extern mp_obj_t Encoder_pins(mp_obj_t self_in) {
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    pin_pair pins = self->encoder->pins();

    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_int(pins.a);
    tuple[1] = mp_obj_new_int(pins.b);
    return mp_obj_new_tuple(2, tuple);
}

extern mp_obj_t Encoder_state(mp_obj_t self_in) {
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    bool_pair state = self->encoder->state();

    mp_obj_t tuple[2];
    tuple[0] = state.a ? mp_const_true : mp_const_false;
    tuple[1] = state.b ? mp_const_true : mp_const_false;
    return mp_obj_new_tuple(2, tuple);
}

extern mp_obj_t Encoder_count(mp_obj_t self_in) {
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    return mp_obj_new_int(self->encoder->count());
}

extern mp_obj_t Encoder_revolutions(mp_obj_t self_in) {
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    return mp_obj_new_float(self->encoder->revolutions());
}

extern mp_obj_t Encoder_angle_degrees(mp_obj_t self_in) {
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    return mp_obj_new_float(self->encoder->angle_degrees());
}

extern mp_obj_t Encoder_angle_radians(mp_obj_t self_in) {
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    return mp_obj_new_float(self->encoder->angle_radians());
}

extern mp_obj_t Encoder_frequency(mp_obj_t self_in) {
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    return mp_obj_new_float(self->encoder->frequency());
}

extern mp_obj_t Encoder_revolutions_per_second(mp_obj_t self_in) {
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    return mp_obj_new_float(self->encoder->revolutions_per_second());
}

extern mp_obj_t Encoder_revolutions_per_minute(mp_obj_t self_in) {
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    return mp_obj_new_float(self->encoder->revolutions_per_minute());
}

extern mp_obj_t Encoder_degrees_per_second(mp_obj_t self_in) {
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    return mp_obj_new_float(self->encoder->degrees_per_second());
}

extern mp_obj_t Encoder_radians_per_second(mp_obj_t self_in) {
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    return mp_obj_new_float(self->encoder->radians_per_second());
}

extern mp_obj_t Encoder_zero_count(mp_obj_t self_in) {
    _Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    self->encoder->zero_count();
    return mp_const_none;
}

extern mp_obj_t Encoder_take_snapshot(mp_obj_t self_in) {
    //_Encoder_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Encoder_obj_t);
    //Capture capture = self->encoder->perform_capture();
    return mp_const_none;
}
}
