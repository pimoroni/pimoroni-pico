#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "../../../pimoroni-pico/libraries/pico_display/pico_display.hpp"

using namespace pimoroni;

PicoDisplay display;


extern "C" {
#include "pico_display.h"

mp_obj_t picodisplay_init() {
    display.init();
    return mp_const_none;
}

mp_obj_t picodisplay_get_width() {
    return mp_obj_new_int(PicoDisplay::WIDTH);
}

mp_obj_t picodisplay_get_height() {
    return mp_obj_new_int(PicoDisplay::HEIGHT);
}

mp_obj_t picodisplay_update() {
    display.update();
    return mp_const_none;
}

mp_obj_t picodisplay_set_backlight(mp_obj_t brightness_obj) {
    float brightness = mp_obj_get_float(brightness_obj);

    if(brightness < 0 || brightness > 1.0f)
        mp_raise_ValueError("brightness out of range. Expected 0.0 to 1.0");
    else
        display.set_backlight((uint8_t)(brightness * 255.0f));

    return mp_const_none;
}

mp_obj_t picodisplay_set_led(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj) {
    int r = mp_obj_get_int(r_obj);
    int g = mp_obj_get_int(g_obj);
    int b = mp_obj_get_int(b_obj);

    if(r < 0 || r > 255)
        mp_raise_ValueError("r out of range. Expected 0 to 255");
    else if(g < 0 || g > 255)
        mp_raise_ValueError("g out of range. Expected 0 to 255");
    else if(b < 0 || b > 255)
        mp_raise_ValueError("b out of range. Expected 0 to 255");
    else
        display.set_led(r, g, b);

    return mp_const_none;
}

mp_obj_t picodisplay_is_pressed(mp_obj_t button_obj) {
    int buttonID = mp_obj_get_int(button_obj);

    bool buttonPressed = false;
    switch(buttonID)
    {
    case 0:
        buttonPressed = display.is_pressed(PicoDisplay::A);
        break;

    case 1:
        buttonPressed = display.is_pressed(PicoDisplay::B);
        break;

    case 2:
        buttonPressed = display.is_pressed(PicoDisplay::X);
        break;

    case 3:
        buttonPressed = display.is_pressed(PicoDisplay::Y);
        break;

    default:
        mp_raise_ValueError("button not valid. Expected 0 to 3");
        break;
    }

    return buttonPressed ? mp_const_true : mp_const_false;
}

mp_obj_t picodisplay_set_pen_rgb(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj) {
    return mp_const_none;
}

mp_obj_t picodisplay_set_pen(mp_obj_t p_obj) {
    return mp_const_none;
}

mp_obj_t picodisplay_create_pen(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj) {
    return mp_const_none;
}

mp_obj_t picodisplay_set_clip(mp_obj_t r_obj) {
    return mp_const_none;
}

mp_obj_t picodisplay_remove_clip() {
    return mp_const_none;
}

// mp_obj_t picodisplay_ptr(mp_obj_t p_obj) {
//     return mp_const_none;
// }

// mp_obj_t picodisplay_ptr(mp_obj_t r_obj) {
//     return mp_const_none;
// }

// mp_obj_t picodisplay_ptr(mp_obj_t x_obj, mp_obj_t y_obj) {
//     return mp_const_none;
// }

mp_obj_t picodisplay_clear() {
    return mp_const_none;
}

mp_obj_t picodisplay_pixel(mp_obj_t p_obj) {
    return mp_const_none;
}

mp_obj_t picodisplay_pixel_span(mp_obj_t p_obj, mp_obj_t l_obj) {
    return mp_const_none;
}

mp_obj_t picodisplay_rectangle(mp_obj_t r_obj) {
    return mp_const_none;
}

mp_obj_t picodisplay_circle(mp_obj_t p_obj, mp_obj_t r_obj) {
    return mp_const_none;
}

mp_obj_t picodisplay_text(mp_obj_t t_obj, mp_obj_t p_obj, mp_obj_t wrap_obj) {
    return mp_const_none;
}

mp_obj_t picodisplay_character(mp_obj_t c_obj, mp_obj_t p_obj, mp_obj_t scale_obj) {
    return mp_const_none;
}
}