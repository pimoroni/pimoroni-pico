#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "../../../pimoroni-pico/libraries/pico_unicorn/pico_unicorn.hpp"

using namespace pimoroni;

PicoUnicorn unicorn;


extern "C" {
#include "pico_unicorn.h"

mp_obj_t picounicorn_init() {
    unicorn.init();
    return mp_const_none;
}

mp_obj_t picounicorn_get_width() {
    return mp_obj_new_int(PicoUnicorn::WIDTH);
}

mp_obj_t picounicorn_get_height() {
    return mp_obj_new_int(PicoUnicorn::HEIGHT);
}

// mp_obj_t picounicorn_update() {
//     unicorn.update();
//     return mp_const_none;
// }

mp_obj_t picounicorn_set_pixel(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's 5

    int x = mp_obj_get_int(args[0]);
    int y = mp_obj_get_int(args[1]);
    int r = mp_obj_get_int(args[2]);
    int g = mp_obj_get_int(args[3]);
    int b = mp_obj_get_int(args[4]);

    if(x < 0 || x >= PicoUnicorn::WIDTH || y < 0 || y >= PicoUnicorn::HEIGHT)
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
            unicorn.set_pixel(x, y, r, g, b);
    }

    return mp_const_none;
}

mp_obj_t picounicorn_set_pixel_value(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t v_obj) {
    int x = mp_obj_get_int(x_obj);
    int y = mp_obj_get_int(y_obj);
    int val = mp_obj_get_int(v_obj);

    if(x < 0 || x >= PicoUnicorn::WIDTH || y < 0 || y >= PicoUnicorn::HEIGHT)
        mp_raise_ValueError("x or y out of range.");
    else
    {
        if(val < 0 || val > 255)
            mp_raise_ValueError("val out of range. Expected 0 to 255");
        else
            unicorn.set_pixel(x, y, val);
    }

    return mp_const_none;
}

mp_obj_t picounicorn_clear() {
    unicorn.clear();
    return mp_const_none;
}

mp_obj_t picounicorn_is_pressed(mp_obj_t button_obj) {
    int buttonID = mp_obj_get_int(button_obj);

    bool buttonPressed = false;
    switch(buttonID)
    {
    case 0:
        buttonPressed = unicorn.is_pressed(PicoUnicorn::A);
        break;

    case 1:
        buttonPressed = unicorn.is_pressed(PicoUnicorn::B);
        break;

    case 2:
        buttonPressed = unicorn.is_pressed(PicoUnicorn::X);
        break;

    case 3:
        buttonPressed = unicorn.is_pressed(PicoUnicorn::Y);
        break;

    default:
        mp_raise_ValueError("button not valid. Expected 0 to 3");
        break;
    }

    return buttonPressed ? mp_const_true : mp_const_false;
}
}