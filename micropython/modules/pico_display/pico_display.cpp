#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "../../../pimoroni-pico/libraries/pico_display/pico_display.hpp"

using namespace pimoroni;

PicoDisplay *display;


extern "C" {
#include "pico_display.h"

mp_obj_t picodisplay_buf_obj;

mp_obj_t picodisplay_init(mp_obj_t buf_obj) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_obj, &bufinfo, MP_BUFFER_RW);
    picodisplay_buf_obj = buf_obj;
    display = new PicoDisplay((uint16_t *)bufinfo.buf);
    display->init();
    return mp_const_none;
}

mp_obj_t picodisplay_get_width() {
    return mp_obj_new_int(PicoDisplay::WIDTH);
}

mp_obj_t picodisplay_get_height() {
    return mp_obj_new_int(PicoDisplay::HEIGHT);
}

mp_obj_t picodisplay_update() {
    display->update();
    return mp_const_none;
}

mp_obj_t picodisplay_set_backlight(mp_obj_t brightness_obj) {
    float brightness = mp_obj_get_float(brightness_obj);

    if(brightness < 0 || brightness > 1.0f)
        mp_raise_ValueError("brightness out of range. Expected 0.0 to 1.0");
    else
        display->set_backlight((uint8_t)(brightness * 255.0f));

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
        display->set_led(r, g, b);

    return mp_const_none;
}

mp_obj_t picodisplay_is_pressed(mp_obj_t button_obj) {
    int buttonID = mp_obj_get_int(button_obj);

    bool buttonPressed = false;
    switch(buttonID)
    {
    case 0:
        buttonPressed = display->is_pressed(PicoDisplay::A);
        break;

    case 1:
        buttonPressed = display->is_pressed(PicoDisplay::B);
        break;

    case 2:
        buttonPressed = display->is_pressed(PicoDisplay::X);
        break;

    case 3:
        buttonPressed = display->is_pressed(PicoDisplay::Y);
        break;

    default:
        mp_raise_ValueError("button not valid. Expected 0 to 3");
        break;
    }

    return buttonPressed ? mp_const_true : mp_const_false;
}

mp_obj_t picodisplay_set_pen(mp_uint_t n_args, const mp_obj_t *args) {
    switch(n_args)
    {
    case 1: {
            int p = mp_obj_get_int(args[0]);

            if(p < 0 || p > 0xffff)
                mp_raise_ValueError("p is not a valid pen.");
            else
                display->set_pen(p);
        } break;

    case 3: {
            int r = mp_obj_get_int(args[0]);
            int g = mp_obj_get_int(args[1]);
            int b = mp_obj_get_int(args[2]);

            if(r < 0 || r > 255)
                mp_raise_ValueError("r out of range. Expected 0 to 255");
            else if(g < 0 || g > 255)
                mp_raise_ValueError("g out of range. Expected 0 to 255");
            else if(b < 0 || b > 255)
                mp_raise_ValueError("b out of range. Expected 0 to 255");
            else
                display->set_pen(r, g, b);
        } break;

    default: {
            char *buffer;
            buffer = (char*)malloc(100);
            sprintf(buffer, "function takes 1 or 3 (r,g,b) positional arguments but %d were given", n_args);
            mp_raise_TypeError(buffer);
        } break;
    }

    return mp_const_none;
}

mp_obj_t picodisplay_create_pen(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj) {
    int r = mp_obj_get_int(r_obj);
    int g = mp_obj_get_int(g_obj);
    int b = mp_obj_get_int(b_obj);

    int pen = 0;
    if(r < 0 || r > 255)
        mp_raise_ValueError("r out of range. Expected 0 to 255");
    else if(g < 0 || g > 255)
        mp_raise_ValueError("g out of range. Expected 0 to 255");
    else if(b < 0 || b > 255)
        mp_raise_ValueError("b out of range. Expected 0 to 255");
    else
        pen = display->create_pen(r, g, b);
    
    return mp_obj_new_int(pen);
}

mp_obj_t picodisplay_set_clip(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's 4

    int x = mp_obj_get_int(args[0]);
    int y = mp_obj_get_int(args[1]);
    int w = mp_obj_get_int(args[2]);
    int h = mp_obj_get_int(args[3]);

    rect r(x, y, w, h);
    display->set_clip(r);

    return mp_const_none;
}

mp_obj_t picodisplay_remove_clip() {
    display->remove_clip();
    return mp_const_none;
}

mp_obj_t picodisplay_clear() {
    display->clear();
    return mp_const_none;
}

mp_obj_t picodisplay_pixel(mp_obj_t x_obj, mp_obj_t y_obj) {
    int x = mp_obj_get_int(x_obj);
    int y = mp_obj_get_int(y_obj);

    point p(x, y);
    display->pixel(p);
 
    return mp_const_none;
}

mp_obj_t picodisplay_pixel_span(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t l_obj) {
    int x = mp_obj_get_int(x_obj);
    int y = mp_obj_get_int(y_obj);
    int l = mp_obj_get_int(l_obj);

    point p(x, y);
    display->pixel_span(p, l);

    return mp_const_none;
}

mp_obj_t picodisplay_rectangle(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's 4

    int x = mp_obj_get_int(args[0]);
    int y = mp_obj_get_int(args[1]);
    int w = mp_obj_get_int(args[2]);
    int h = mp_obj_get_int(args[3]);

    rect r(x, y, w, h);
    display->rectangle(r);

    return mp_const_none;
}

mp_obj_t picodisplay_circle(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t r_obj) {
    int x = mp_obj_get_int(x_obj);
    int y = mp_obj_get_int(y_obj);
    int r = mp_obj_get_int(r_obj);

    point p(x, y);
    display->circle(p, r);

    return mp_const_none;
}

mp_obj_t picodisplay_character(mp_uint_t n_args, const mp_obj_t *args) {
    int c = mp_obj_get_int(args[0]);
    int x = mp_obj_get_int(args[1]);
    int y = mp_obj_get_int(args[2]);

    point p(x, y);
    if(n_args == 4) {
        int scale = mp_obj_get_int(args[3]);
        display->character((char)c, p, scale);
    }
    else
        display->character((char)c, p);

    return mp_const_none;
}

mp_obj_t picodisplay_text(mp_uint_t n_args, const mp_obj_t *args) {
    mp_check_self(mp_obj_is_str_or_bytes(args[0]));
    GET_STR_DATA_LEN(args[0], str, str_len);

    std::string t((const char*)str);

    int x = mp_obj_get_int(args[1]);
    int y = mp_obj_get_int(args[2]);
    int wrap = mp_obj_get_int(args[3]);

    point p(x, y);
    if(n_args == 5) {
        int scale = mp_obj_get_int(args[4]);
        display->text(t, p, wrap, scale);
    }
    else
        display->text(t, p, wrap);

    return mp_const_none;
}
}
