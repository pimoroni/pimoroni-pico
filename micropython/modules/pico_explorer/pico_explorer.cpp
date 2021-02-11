#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "../../../pimoroni-pico/libraries/pico_explorer/pico_explorer.hpp"

using namespace pimoroni;

PicoExplorer *explorer = nullptr;


extern "C" {
#include "pico_explorer.h"

#define NOT_INITIALISED_MSG     "Cannot call this function, as picoexplorer is not initialised. Call picoexplorer.init(<bytearray>) first."

mp_obj_t picoexplorer_buf_obj;

mp_obj_t picoexplorer_init(mp_obj_t buf_obj) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_obj, &bufinfo, MP_BUFFER_RW);
    picoexplorer_buf_obj = buf_obj;
    if(explorer == nullptr)
        explorer = new PicoExplorer((uint16_t *)bufinfo.buf);
    explorer->init();
    return mp_const_none;
}

mp_obj_t picoexplorer_get_width() {
    return mp_obj_new_int(PicoExplorer::WIDTH);
}

mp_obj_t picoexplorer_get_height() {
    return mp_obj_new_int(PicoExplorer::HEIGHT);
}

mp_obj_t picoexplorer_update() {
    if(explorer != nullptr)
        explorer->update();
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picoexplorer_is_pressed(mp_obj_t button_obj) {
    bool buttonPressed = false;

    if(explorer != nullptr) {
        int buttonID = mp_obj_get_int(button_obj);
        switch(buttonID) {
        case 0:
            buttonPressed = explorer->is_pressed(PicoExplorer::A);
            break;

        case 1:
            buttonPressed = explorer->is_pressed(PicoExplorer::B);
            break;

        case 2:
            buttonPressed = explorer->is_pressed(PicoExplorer::X);
            break;

        case 3:
            buttonPressed = explorer->is_pressed(PicoExplorer::Y);
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

extern mp_obj_t picoexplorer_get_adc(mp_obj_t channel_obj) {
    float reading = 0.0f;

    if(explorer != nullptr) {
        int channel = mp_obj_get_int(channel_obj);
        if(channel < 0 || channel > 2)
            mp_raise_ValueError("adc channel not valid. Expected 0 to 2");
        else
            reading = explorer->get_adc(channel);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_obj_new_float(reading);
}

extern mp_obj_t picoexplorer_set_motor(mp_uint_t n_args, const mp_obj_t *args) {
    if(explorer != nullptr) {
        int channel = mp_obj_get_int(args[0]);
        int action = mp_obj_get_int(args[1]);

        if(channel < 0 || channel > 1)
            mp_raise_ValueError("motor channel not valid. Expected 0 to 1");
        else if(action < 0 || action > 2)
            mp_raise_ValueError("motor action not valid. Expected 0 to 2");
        else {
            if(n_args == 3) {
                float speed = mp_obj_get_float(args[2]);
                explorer->set_motor(channel, action, speed);
            }
            else
                explorer->set_motor(channel, action);
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

extern mp_obj_t picoexplorer_set_audio_pin(mp_obj_t pin_obj) {
    if(explorer != nullptr) {
        int pin = mp_obj_get_int(pin_obj);
        explorer->set_audio_pin(pin);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    return mp_const_none;
}

extern mp_obj_t picoexplorer_set_tone(mp_uint_t n_args, const mp_obj_t *args) {
    if(explorer != nullptr) {
        int frequency = mp_obj_get_int(args[0]);

        if(n_args == 2) {
            float duty = mp_obj_get_int(args[1]);
            explorer->set_tone(frequency, duty);
        }
        else
            explorer->set_tone(frequency);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picoexplorer_set_pen(mp_uint_t n_args, const mp_obj_t *args) {
    if(explorer != nullptr) {
        switch(n_args) {
        case 1: {
                int p = mp_obj_get_int(args[0]);

                if(p < 0 || p > 0xffff)
                    mp_raise_ValueError("p is not a valid pen.");
                else
                    explorer->set_pen(p);
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
                    explorer->set_pen(r, g, b);
            } break;

        default: {
                char *buffer;
                buffer = (char*)malloc(100);
                sprintf(buffer, "function takes 1 or 3 (r,g,b) positional arguments but %d were given", n_args);
                mp_raise_TypeError(buffer);
                free(buffer);
            } break;
        }
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picoexplorer_create_pen(mp_obj_t r_obj, mp_obj_t g_obj, mp_obj_t b_obj) {
    int pen = 0;
    
    if(explorer != nullptr) {
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
            pen = explorer->create_pen(r, g, b);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_obj_new_int(pen);
}

mp_obj_t picoexplorer_set_clip(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's 4

    if(explorer != nullptr) {
        int x = mp_obj_get_int(args[0]);
        int y = mp_obj_get_int(args[1]);
        int w = mp_obj_get_int(args[2]);
        int h = mp_obj_get_int(args[3]);

        Rect r(x, y, w, h);
        explorer->set_clip(r);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picoexplorer_remove_clip() {
    if(explorer != nullptr)
        explorer->remove_clip();
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    return mp_const_none;
}

mp_obj_t picoexplorer_clear() {
    if(explorer != nullptr)
        explorer->clear();
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);
    return mp_const_none;
}

mp_obj_t picoexplorer_pixel(mp_obj_t x_obj, mp_obj_t y_obj) {
    if(explorer != nullptr) {
        int x = mp_obj_get_int(x_obj);
        int y = mp_obj_get_int(y_obj);

        Point p(x, y);
        explorer->pixel(p);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picoexplorer_pixel_span(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t l_obj) {
    if(explorer != nullptr) {
        int x = mp_obj_get_int(x_obj);
        int y = mp_obj_get_int(y_obj);
        int l = mp_obj_get_int(l_obj);

        Point p(x, y);
        explorer->pixel_span(p, l);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picoexplorer_rectangle(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's 4

    if(explorer != nullptr) {
        int x = mp_obj_get_int(args[0]);
        int y = mp_obj_get_int(args[1]);
        int w = mp_obj_get_int(args[2]);
        int h = mp_obj_get_int(args[3]);

        Rect r(x, y, w, h);
        explorer->rectangle(r);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picoexplorer_circle(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t r_obj) {
    if(explorer != nullptr) {
        int x = mp_obj_get_int(x_obj);
        int y = mp_obj_get_int(y_obj);
        int r = mp_obj_get_int(r_obj);

        Point p(x, y);
        explorer->circle(p, r);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picoexplorer_character(mp_uint_t n_args, const mp_obj_t *args) {
    if(explorer != nullptr) {
        int c = mp_obj_get_int(args[0]);
        int x = mp_obj_get_int(args[1]);
        int y = mp_obj_get_int(args[2]);

        Point p(x, y);
        if(n_args == 4) {
            int scale = mp_obj_get_int(args[3]);
            explorer->character((char)c, p, scale);
        }
        else
            explorer->character((char)c, p);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}

mp_obj_t picoexplorer_text(mp_uint_t n_args, const mp_obj_t *args) {
    if(explorer != nullptr) {
        mp_check_self(mp_obj_is_str_or_bytes(args[0]));
        GET_STR_DATA_LEN(args[0], str, str_len);

        std::string t((const char*)str);

        int x = mp_obj_get_int(args[1]);
        int y = mp_obj_get_int(args[2]);
        int wrap = mp_obj_get_int(args[3]);

        Point p(x, y);
        if(n_args == 5) {
            int scale = mp_obj_get_int(args[4]);
            explorer->text(t, p, wrap, scale);
        }
        else
            explorer->text(t, p, wrap);
    }
    else
        mp_raise_msg(&mp_type_RuntimeError, NOT_INITIALISED_MSG);

    return mp_const_none;
}
}