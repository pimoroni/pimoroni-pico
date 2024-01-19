#include <cstdio>
#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include "micropython/modules/util.hpp"
#include "libraries/pico_scroll/pico_scroll.hpp"
#include "libraries/pico_scroll/pico_scroll_font.hpp"

using namespace pimoroni;

PicoScroll *scroll = nullptr;


extern "C" {
#include "pico_scroll.h"
#include "micropython/modules/pimoroni_i2c/pimoroni_i2c.h"
#include "py/builtin.h"

#define BUFFER_TOO_SMALL_MSG "bytearray too small: len(image) < width * height."
#define INCORRECT_SIZE_MSG "Scroll height wrong: > 8 pixels."

typedef struct _PicoScroll_obj_t {
    mp_obj_base_t base;
    PicoScroll* scroll;
} PicoScroll_obj_t;

// from picographics/picographics.cpp
// used to support accepting a PicoGraphics class
typedef struct _ModPicoGraphics_obj_t {
    mp_obj_base_t base;
    PicoGraphics *graphics;
    DisplayDriver *display;
    void *spritedata;
    void *buffer;
    _PimoroniI2C_obj_t *i2c;
} ModPicoGraphics_obj_t;

mp_obj_t picoscroll_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _PicoScroll_obj_t *self = nullptr;

    self = m_new_obj_with_finaliser(PicoScroll_obj_t);
    self->base.type = &PicoScroll_type;

    self->scroll = m_new_class(PicoScroll);
    self->scroll->init();

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t picoscroll___del__(mp_obj_t self_in) {
    PicoScroll_obj_t *self = MP_OBJ_TO_PTR2(self_in, PicoScroll_obj_t);
    m_del_class(PicoScroll, self->scroll);
    return mp_const_none;
}

mp_obj_t picoscroll_get_width(mp_obj_t self_in) {
    (void)self_in;
    return mp_obj_new_int(PicoScroll::WIDTH);
}

mp_obj_t picoscroll_get_height(mp_obj_t self_in) {
    (void)self_in;
    return mp_obj_new_int(PicoScroll::HEIGHT);
}

mp_obj_t picoscroll_show(mp_obj_t self_in) {
    PicoScroll_obj_t *self = MP_OBJ_TO_PTR2(self_in, PicoScroll_obj_t);
    self->scroll->update();
    return mp_const_none;
}

mp_obj_t picoscroll_set_pixel(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's self + 3
   
    PicoScroll_obj_t *self = MP_OBJ_TO_PTR2(args[0], PicoScroll_obj_t);

    int x = mp_obj_get_int(args[1]);
    int y = mp_obj_get_int(args[2]);
    int val = mp_obj_get_int(args[3]);

    if (x < 0 || x >= PicoScroll::WIDTH || y < 0 || y >= PicoScroll::HEIGHT)
        mp_raise_ValueError("x or y out of range.");
    if (val < 0 || val > 255)
        mp_raise_ValueError("val out of range. Expected 0 to 255");

    self->scroll->set_pixel(x, y, val);

    return mp_const_none;
}

mp_obj_t picoscroll_scroll_text(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's self + 3
   
    PicoScroll_obj_t *self = MP_OBJ_TO_PTR2(args[0], PicoScroll_obj_t);

    int brightness = mp_obj_get_int(args[2]);
    int delay_ms = mp_obj_get_int(args[3]);

    if(mp_obj_is_str_or_bytes(args[1])) {
        GET_STR_DATA_LEN(args[1], str, str_len);

        int draw_buffer_len = 6 * str_len;
        char *draw_buffer = m_new(char, draw_buffer_len);

        render_text((const char *)str, str_len, (unsigned char *)draw_buffer, draw_buffer_len);

        for (int offset = -PicoScroll::WIDTH; offset < draw_buffer_len; offset++) {
            self->scroll->clear();
            self->scroll->set_bitmap_1d((const char *)draw_buffer, draw_buffer_len, brightness, offset);
            self->scroll->update();
    mp_event_handle_nowait();
            sleep_ms(delay_ms);
        }

        m_free(draw_buffer);

        self->scroll->clear();
        self->scroll->update();
    }

    return mp_const_none;
}

mp_obj_t picoscroll_show_text(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's self + 3
   
    PicoScroll_obj_t *self = MP_OBJ_TO_PTR2(args[0], PicoScroll_obj_t);

    int brightness = mp_obj_get_int(args[2]);
    int offset = mp_obj_get_int(args[3]);

    if(mp_obj_is_str_or_bytes(args[1])) {
        GET_STR_DATA_LEN(args[1], str, str_len);

        int draw_buffer_len = 6 * str_len;
        char *draw_buffer = m_new(char, draw_buffer_len);

        render_text((const char *)str, str_len, (unsigned char *)draw_buffer, draw_buffer_len);
        self->scroll->set_bitmap_1d((const char *)draw_buffer, draw_buffer_len, brightness, offset);

        m_free(draw_buffer);
    }

    return mp_const_none;
}

mp_obj_t picoscroll_set_pixels(mp_obj_t self_in, mp_obj_t image_obj) {
    PicoScroll_obj_t *self = MP_OBJ_TO_PTR2(self_in, PicoScroll_obj_t);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(image_obj, &bufinfo, MP_BUFFER_RW);

    if (bufinfo.len < (PicoScroll::WIDTH * PicoScroll::HEIGHT)) {
        mp_raise_msg(&mp_type_IndexError, BUFFER_TOO_SMALL_MSG);
    }

    self->scroll->set_pixels((const char*)bufinfo.buf);

    return mp_const_none;
}

mp_obj_t picoscroll_show_bitmap_1d(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args; //Unused input parameter, we know it's self + 3
   
    PicoScroll_obj_t *self = MP_OBJ_TO_PTR2(args[0], PicoScroll_obj_t);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_RW);
    int offset = mp_obj_get_int(args[3]);
    int brightness = mp_obj_get_int(args[2]);
    int length = bufinfo.len;

    // clear the scroll, so only need to write visible bytes
    self->scroll->clear();

    if ((offset < -PicoScroll::WIDTH) || (offset > length)) {
        return mp_const_none;
    }

    self->scroll->set_bitmap_1d((const char *)bufinfo.buf, bufinfo.len, brightness, offset);

    return mp_const_none;
}

mp_obj_t picoscroll_clear(mp_obj_t self_in) {
    PicoScroll_obj_t *self = MP_OBJ_TO_PTR2(self_in, PicoScroll_obj_t);
    self->scroll->clear();
    return mp_const_none;
}

mp_obj_t picoscroll_is_pressed(mp_obj_t self_in, mp_obj_t button_obj) {
    PicoScroll_obj_t *self = MP_OBJ_TO_PTR2(self_in, PicoScroll_obj_t);

    bool buttonPressed = false;

    int buttonID = mp_obj_get_int(button_obj);    
    switch(buttonID) {
    case 0:
        buttonPressed = self->scroll->is_pressed(PicoScroll::A);
        break;

    case 1:
        buttonPressed = self->scroll->is_pressed(PicoScroll::B);
        break;

    case 2:
        buttonPressed = self->scroll->is_pressed(PicoScroll::X);
        break;

    case 3:
        buttonPressed = self->scroll->is_pressed(PicoScroll::Y);
        break;

    default:
        mp_raise_ValueError("button not valid. Expected 0 to 3");
        break;
    }

    return buttonPressed ? mp_const_true : mp_const_false;
}

mp_obj_t picoscroll_update(mp_obj_t self_in, mp_obj_t graphics_in) {
    PicoScroll_obj_t *self = MP_OBJ_TO_PTR2(self_in, PicoScroll_obj_t);
    ModPicoGraphics_obj_t *picographics = MP_OBJ_TO_PTR2(graphics_in, ModPicoGraphics_obj_t);

    if(picographics->base.type == &ModPicoGraphics_type) {
        self->scroll->update(picographics->graphics);
    }
    return mp_const_none;
}
}
