#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "../../../pimoroni-pico/libraries/pico_rgb_keypad/pico_rgb_keypad.hpp"

using namespace pimoroni;

PicoRGBKeypad keypad;


extern "C" {
#include "pico_rgb_keypad.h"

mp_obj_t picokeypad_init() {
    keypad.init();
    return mp_const_none;
}

mp_obj_t picokeypad_get_width() {
    return mp_obj_new_int(PicoRGBKeypad::WIDTH);
}

mp_obj_t picokeypad_get_height() {
    return mp_obj_new_int(PicoRGBKeypad::HEIGHT);
}

mp_obj_t picokeypad_get_num_pads() {
    return mp_obj_new_int(PicoRGBKeypad::NUM_PADS);
}

mp_obj_t picokeypad_update() {
    keypad.update();
    return mp_const_none;
}

mp_obj_t picokeypad_set_brightness(mp_obj_t brightness_obj) {
    float brightness = mp_obj_get_float(brightness_obj);

    if(brightness < 0 || brightness > 1.0f)
        mp_raise_ValueError("brightness out of range. Expected 0.0 to 1.0");
    else
        keypad.set_brightness(brightness);

    return mp_const_none;
}

mp_obj_t picokeypad_illuminate_xy(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's 5

    int x = mp_obj_get_int(args[0]);
    int y = mp_obj_get_int(args[1]);
    int r = mp_obj_get_int(args[2]);
    int g = mp_obj_get_int(args[3]);
    int b = mp_obj_get_int(args[4]);

    if(x < 0 || x >= PicoRGBKeypad::WIDTH || y < 0 || y >= PicoRGBKeypad::HEIGHT)
        mp_raise_ValueError("x or y out of range.");
    else
    {
        if(r < 0 || r > 255)
            mp_raise_ValueError("r out of range. Expected 0 to 255");
        else if(g < 0 || g > 255)
            mp_raise_ValueError("g out of range. Expected 0 to 255");
        else if(b < 0 || b > 255)
            mp_raise_ValueError("b out of range. Expected 0 to 255");
        else
            keypad.illuminate(x, y, r, g, b);
    }

    return mp_const_none;
}

mp_obj_t picokeypad_illuminate(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's 5

    int i = mp_obj_get_int(args[0]);
    int r = mp_obj_get_int(args[1]);
    int g = mp_obj_get_int(args[2]);
    int b = mp_obj_get_int(args[3]);

    if(i < 0 || i >= PicoRGBKeypad::NUM_PADS)
        mp_raise_ValueError("x or y out of range.");
    else
    {
        if(r < 0 || r > 255)
            mp_raise_ValueError("r out of range. Expected 0 to 255");
        else if(g < 0 || g > 255)
            mp_raise_ValueError("g out of range. Expected 0 to 255");
        else if(b < 0 || b > 255)
            mp_raise_ValueError("b out of range. Expected 0 to 255");
        else
            keypad.illuminate(i, r, g, b);
    }

    return mp_const_none;
}

mp_obj_t picokeypad_clear() {
    keypad.clear();
    return mp_const_none;
}

mp_obj_t picokeypad_get_button_states() {
    return mp_obj_new_int(keypad.get_button_states());
}
}