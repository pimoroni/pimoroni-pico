#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "libraries/pico_rgb_keypad/pico_rgb_keypad.hpp"

using namespace pimoroni;

PicoRGBKeypad *keypad = nullptr;


extern "C" {
#include "pico_rgb_keypad.h"

#define NOT_INITIALISED_MSG     "Cannot call this function, as picokeypad is not initialised. Call picokeypad.init() first."

mp_obj_t picokeypad_init() {
    if(keypad == nullptr) {
        keypad = new PicoRGBKeypad();
        keypad->init();
    }
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
    if(keypad != nullptr)
        keypad->update();
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    return mp_const_none;
}

mp_obj_t picokeypad_set_brightness(mp_obj_t brightness_obj) {
    if(keypad != nullptr) {
        float brightness = mp_obj_get_float(brightness_obj);

        if(brightness < 0 || brightness > 1.0f)
            mp_raise_ValueError("brightness out of range. Expected 0.0 to 1.0");
        else
            keypad->set_brightness(brightness);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picokeypad_illuminate_xy(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's 5

    if(keypad != nullptr) {    
        int x = mp_obj_get_int(args[0]);
        int y = mp_obj_get_int(args[1]);
        int r = mp_obj_get_int(args[2]);
        int g = mp_obj_get_int(args[3]);
        int b = mp_obj_get_int(args[4]);

        if(x < 0 || x >= PicoRGBKeypad::WIDTH || y < 0 || y >= PicoRGBKeypad::HEIGHT)
            mp_raise_ValueError("x or y out of range.");
        else {
            if(r < 0 || r > 255)
                mp_raise_ValueError("r out of range. Expected 0 to 255");
            else if(g < 0 || g > 255)
                mp_raise_ValueError("g out of range. Expected 0 to 255");
            else if(b < 0 || b > 255)
                mp_raise_ValueError("b out of range. Expected 0 to 255");
            else
                keypad->illuminate(x, y, r, g, b);
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picokeypad_illuminate(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's 5

    if(keypad != nullptr) {
        int i = mp_obj_get_int(args[0]);
        int r = mp_obj_get_int(args[1]);
        int g = mp_obj_get_int(args[2]);
        int b = mp_obj_get_int(args[3]);

        if(i < 0 || i >= PicoRGBKeypad::NUM_PADS)
            mp_raise_ValueError("x or y out of range.");
        else {
            if(r < 0 || r > 255)
                mp_raise_ValueError("r out of range. Expected 0 to 255");
            else if(g < 0 || g > 255)
                mp_raise_ValueError("g out of range. Expected 0 to 255");
            else if(b < 0 || b > 255)
                mp_raise_ValueError("b out of range. Expected 0 to 255");
            else
                keypad->illuminate(i, r, g, b);
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picokeypad_clear() {
    if(keypad != nullptr)
        keypad->clear();
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picokeypad_get_button_states() {
    uint16_t states = 0;
    if(keypad != nullptr)
        states = keypad->get_button_states();
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    
    return mp_obj_new_int(states);
}
}