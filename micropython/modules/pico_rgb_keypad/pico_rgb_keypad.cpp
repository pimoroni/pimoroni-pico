#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "micropython/modules/util.hpp"
#include "libraries/pico_rgb_keypad/pico_rgb_keypad.hpp"

using namespace pimoroni;

extern "C" {
#include "pico_rgb_keypad.h"

typedef struct _PicoKeypad_obj_t {
    mp_obj_base_t base;
    PicoRGBKeypad* keypad;
} PicoKeypad_obj_t;

mp_obj_t picokeypad_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _PicoKeypad_obj_t *self = nullptr;

    self = m_new_obj_with_finaliser(PicoKeypad_obj_t);
    self->base.type = &PicoKeypad_type;

    self->keypad = m_new_class(PicoRGBKeypad);
    self->keypad->init();

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t picokeypad___del__(mp_obj_t self_in) {
    PicoKeypad_obj_t *self = MP_OBJ_TO_PTR2(self_in, PicoKeypad_obj_t);
    m_del_class(PicoRGBKeypad, self->keypad);
    return mp_const_none;
}

mp_obj_t picokeypad_get_width(mp_obj_t self_in) {
    (void)self_in;
    return mp_obj_new_int(PicoRGBKeypad::WIDTH);
}

mp_obj_t picokeypad_get_height(mp_obj_t self_in) {
    (void)self_in;
    return mp_obj_new_int(PicoRGBKeypad::HEIGHT);
}

mp_obj_t picokeypad_get_num_pads(mp_obj_t self_in) {
    (void)self_in;
    return mp_obj_new_int(PicoRGBKeypad::NUM_PADS);
}

mp_obj_t picokeypad_update(mp_obj_t self_in) {
    PicoKeypad_obj_t *self = MP_OBJ_TO_PTR2(self_in, PicoKeypad_obj_t);
    self->keypad->update();
    return mp_const_none;
}

mp_obj_t picokeypad_set_brightness(mp_obj_t self_in, mp_obj_t brightness_obj) {
    PicoKeypad_obj_t *self = MP_OBJ_TO_PTR2(self_in, PicoKeypad_obj_t);

    float brightness = mp_obj_get_float(brightness_obj);

    if(brightness < 0 || brightness > 1.0f)
        mp_raise_ValueError("brightness out of range. Expected 0.0 to 1.0");

    self->keypad->set_brightness(brightness);

    return mp_const_none;
}

mp_obj_t picokeypad_illuminate_xy(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's self + 5
   
    PicoKeypad_obj_t *self = MP_OBJ_TO_PTR2(args[0], PicoKeypad_obj_t);

    int x = mp_obj_get_int(args[1]);
    int y = mp_obj_get_int(args[2]);
    int r = mp_obj_get_int(args[3]);
    int g = mp_obj_get_int(args[4]);
    int b = mp_obj_get_int(args[5]);

    if(x < 0 || x >= PicoRGBKeypad::WIDTH || y < 0 || y >= PicoRGBKeypad::HEIGHT)
        mp_raise_ValueError("x or y out of range.");
    if(r < 0 || r > 255)
        mp_raise_ValueError("r out of range. Expected 0 to 255");
    if(g < 0 || g > 255)
        mp_raise_ValueError("g out of range. Expected 0 to 255");
    if(b < 0 || b > 255)
        mp_raise_ValueError("b out of range. Expected 0 to 255");

    self->keypad->illuminate(x, y, r, g, b);
    
    return mp_const_none;
}

mp_obj_t picokeypad_illuminate(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's self + 5
   
    PicoKeypad_obj_t *self = MP_OBJ_TO_PTR2(args[0], PicoKeypad_obj_t);

    int i = mp_obj_get_int(args[1]);
    int r = mp_obj_get_int(args[2]);
    int g = mp_obj_get_int(args[3]);
    int b = mp_obj_get_int(args[4]);

    if(i < 0 || i >= PicoRGBKeypad::NUM_PADS)
        mp_raise_ValueError("x or y out of range.");   
    if(r < 0 || r > 255)
        mp_raise_ValueError("r out of range. Expected 0 to 255");
    if(g < 0 || g > 255)
        mp_raise_ValueError("g out of range. Expected 0 to 255");
    if(b < 0 || b > 255)
        mp_raise_ValueError("b out of range. Expected 0 to 255");

    self->keypad->illuminate(i, r, g, b);

    return mp_const_none;
}

mp_obj_t picokeypad_clear(mp_obj_t self_in) {
    PicoKeypad_obj_t *self = MP_OBJ_TO_PTR2(self_in, PicoKeypad_obj_t);
    self->keypad->clear();
    return mp_const_none;
}

mp_obj_t picokeypad_get_button_states(mp_obj_t self_in) {
    PicoKeypad_obj_t *self = MP_OBJ_TO_PTR2(self_in, PicoKeypad_obj_t);
    uint16_t states = 0;
    states = self->keypad->get_button_states();
    return mp_obj_new_int(states);
}
}