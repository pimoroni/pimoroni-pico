#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "libraries/pico_display_2/pico_display_2.hpp"

using namespace pimoroni;

PicoDisplay2 *display2 = nullptr;


extern "C" {
#include "pico_display_2.h"

#define NOT_INITIALISED_MSG     "Cannot call this function, as picodisplay2 is not initialised. Call picodisplay2.init(<bytearray>) first."

mp_obj_t picodisplay2_buf_obj;

mp_obj_t picodisplay2_init(mp_obj_t buf_obj) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_obj, &bufinfo, MP_BUFFER_RW);
    picodisplay2_buf_obj = buf_obj;

    // If a display already exists, delete it
    if(display2 != nullptr) {
        delete display2;
    }

    // Create a new display pointing to the newly provided buffer
    display2 = new PicoDisplay2((uint16_t *)bufinfo.buf);
    display2->init();

    return mp_const_none;
}

mp_obj_t picodisplay2_get_width() {
    return mp_obj_new_int(PicoDisplay2::WIDTH);
}

mp_obj_t picodisplay2_get_height() {
    return mp_obj_new_int(PicoDisplay2::HEIGHT);
}

mp_obj_t picodisplay2_update() {
    if(display2 != nullptr)
        display2->update();
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picodisplay2_flip() {
    if(display2 != nullptr)
        display2->flip();
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picodisplay2_set_backlight(mp_obj_t brightness_obj) {
    if(display2 != nullptr) {
        float brightness = mp_obj_get_float(brightness_obj);

        if(brightness < 0 || brightness > 1.0f)
            mp_raise_ValueError("brightness out of range. Expected 0.0 to 1.0");
        else
            display2->set_backlight((uint8_t)(brightness * 255.0f));
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picodisplay2_set_led(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj) {
    if(display2 != nullptr) {
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
            display2->set_led(r, g, b);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picodisplay2_is_pressed(mp_obj_t button_obj) {
    bool buttonPressed = false;
    
    if(display2 != nullptr) {
        int buttonID = mp_obj_get_int(button_obj);
        switch(buttonID) {
        case 0:
            buttonPressed = display2->is_pressed(PicoDisplay2::A);
            break;

        case 1:
            buttonPressed = display2->is_pressed(PicoDisplay2::B);
            break;

        case 2:
            buttonPressed = display2->is_pressed(PicoDisplay2::X);
            break;

        case 3:
            buttonPressed = display2->is_pressed(PicoDisplay2::Y);
            break;

        default:
            mp_raise_ValueError("button not valid. Expected 0 to 3");
            break;
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return buttonPressed ? mp_const_true : mp_const_false;
}

mp_obj_t picodisplay2_set_pen(mp_uint_t n_args, const mp_obj_t *args) {
    if(display2 != nullptr) {
        switch(n_args) {
        case 1: {
                int p = mp_obj_get_int(args[0]);

                if(p < 0 || p > 0xffff)
                    mp_raise_ValueError("p is not a valid pen.");
                else
                    display2->set_pen(p);
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
                    display2->set_pen(r, g, b);
            } break;

        default: {
                char *buffer;
                buffer = (char*)malloc(100);
                sprintf(buffer, "function takes 1 or 3 (r,g,b) positional arguments but %d were given", n_args);
                mp_raise_TypeError(buffer);
            } break;
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picodisplay2_create_pen(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj) {
    int pen = 0;
    
    if(display2 != nullptr) {
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
            pen = display2->create_pen(r, g, b);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_obj_new_int(pen);
}

mp_obj_t picodisplay2_set_clip(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's 4

    if(display2 != nullptr) {
        int x = mp_obj_get_int(args[0]);
        int y = mp_obj_get_int(args[1]);
        int w = mp_obj_get_int(args[2]);
        int h = mp_obj_get_int(args[3]);

        Rect r(x, y, w, h);
        display2->set_clip(r);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picodisplay2_remove_clip() {
    if(display2 != nullptr)
        display2->remove_clip();
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picodisplay2_clear() {
    if(display2 != nullptr)
        display2->clear();
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picodisplay2_pixel(mp_obj_t x_obj, mp_obj_t y_obj) {
    if(display2 != nullptr) {
        int x = mp_obj_get_int(x_obj);
        int y = mp_obj_get_int(y_obj);

        Point p(x, y);
        display2->pixel(p);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picodisplay2_pixel_span(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t l_obj) {
    if(display2 != nullptr) {
        int x = mp_obj_get_int(x_obj);
        int y = mp_obj_get_int(y_obj);
        int l = mp_obj_get_int(l_obj);

        Point p(x, y);
        display2->pixel_span(p, l);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picodisplay2_rectangle(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's 4

    if(display2 != nullptr) {
        int x = mp_obj_get_int(args[0]);
        int y = mp_obj_get_int(args[1]);
        int w = mp_obj_get_int(args[2]);
        int h = mp_obj_get_int(args[3]);

        Rect r(x, y, w, h);
        display2->rectangle(r);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picodisplay2_circle(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t r_obj) {
    if(display2 != nullptr) {
        int x = mp_obj_get_int(x_obj);
        int y = mp_obj_get_int(y_obj);
        int r = mp_obj_get_int(r_obj);

        Point p(x, y);
        display2->circle(p, r);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picodisplay2_character(mp_uint_t n_args, const mp_obj_t *args) {
    if(display2 != nullptr) {
        int c = mp_obj_get_int(args[0]);
        int x = mp_obj_get_int(args[1]);
        int y = mp_obj_get_int(args[2]);

        Point p(x, y);
        if(n_args == 4) {
            int scale = mp_obj_get_int(args[3]);
            display2->character((char)c, p, scale);
        }
        else
            display2->character((char)c, p);
    }

    return mp_const_none;
}

mp_obj_t picodisplay2_text(mp_uint_t n_args, const mp_obj_t *args) {
    if(display2 != nullptr) {
        if(mp_obj_is_str_or_bytes(args[0])) {
            GET_STR_DATA_LEN(args[0], str, str_len);

            std::string t((const char*)str);

            int x = mp_obj_get_int(args[1]);
            int y = mp_obj_get_int(args[2]);
            int wrap = mp_obj_get_int(args[3]);

            Point p(x, y);
            if(n_args == 5) {
                int scale = mp_obj_get_int(args[4]);
                display2->text(t, p, wrap, scale);
            }
            else
                display2->text(t, p, wrap);
        }
        else if(mp_obj_is_float(args[0])) {
            mp_raise_TypeError("can't convert 'float' object to str implicitly");
        }
        else if(mp_obj_is_int(args[0])) {
            mp_raise_TypeError("can't convert 'int' object to str implicitly");
        }
        else if(mp_obj_is_bool(args[0])) {
            mp_raise_TypeError("can't convert 'bool' object to str implicitly");
        }
        else {
            mp_raise_TypeError("can't convert object to str implicitly");
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}
}
