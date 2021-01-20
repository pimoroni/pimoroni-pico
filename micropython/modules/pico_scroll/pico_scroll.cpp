#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "../../../pimoroni-pico/libraries/pico_scroll/pico_scroll.hpp"

using namespace pimoroni;

PicoScroll scroll;


extern "C" {
#include "pico_scroll.h"

mp_obj_t picoscroll_init() {
    scroll.init();
    return mp_const_none;
}

mp_obj_t picoscroll_get_width() {
    return mp_obj_new_int(PicoScroll::WIDTH);
}

mp_obj_t picoscroll_get_height() {
    return mp_obj_new_int(PicoScroll::HEIGHT);
}

mp_obj_t picoscroll_update() {
    scroll.update();
    return mp_const_none;
}

mp_obj_t picoscroll_set_pixel(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t v_obj) {
    int x = mp_obj_get_int(x_obj);
    int y = mp_obj_get_int(y_obj);
    int val = mp_obj_get_int(v_obj);

    if(x < 0 || x >= PicoScroll::WIDTH || y < 0 || y >= PicoScroll::HEIGHT)
        mp_raise_ValueError("x or y out of range.");
    else
    {
        if(val < 0 || val > 255)
            mp_raise_ValueError("val out of range. Expected 0 to 255");
        else
            scroll.set_pixel(x, y, val);
    }

    return mp_const_none;
}

mp_obj_t picoscroll_clear() {
    scroll.clear();
    return mp_const_none;
}

mp_obj_t picoscroll_is_pressed(mp_obj_t button_obj) {
    int buttonID = mp_obj_get_int(button_obj);

    bool buttonPressed = false;
    switch(buttonID)
    {
    case 0:
        buttonPressed = scroll.is_pressed(PicoScroll::A);
        break;

    case 1:
        buttonPressed = scroll.is_pressed(PicoScroll::B);
        break;

    case 2:
        buttonPressed = scroll.is_pressed(PicoScroll::X);
        break;

    case 3:
        buttonPressed = scroll.is_pressed(PicoScroll::Y);
        break;

    default:
        mp_raise_ValueError("button not valid. Expected 0 to 3");
        break;
    }

    return buttonPressed ? mp_const_true : mp_const_false;
}
}