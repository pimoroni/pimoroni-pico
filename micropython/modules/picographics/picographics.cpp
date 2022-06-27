#include "drivers/st7789/st7789.hpp"
#include "drivers/st7735/st7735.hpp"
#include "drivers/sh1107/sh1107.hpp"
#include "drivers/uc8151/uc8151.hpp"
#include "drivers/uc8159/uc8159.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_bus.hpp"
#include "common/pimoroni_i2c.hpp"

#include "micropython/modules/util.hpp"

using namespace pimoroni;

extern "C" {
#include "picographics.h"
#include "micropython/modules/pimoroni_bus/pimoroni_bus.h"
#include "pimoroni_i2c.h"
#include "py/stream.h"
#include "py/reader.h"
#include "extmod/vfs.h"

std::string mp_obj_to_string_r(const mp_obj_t &obj) {
    if(mp_obj_is_str_or_bytes(obj)) {
        GET_STR_DATA_LEN(obj, str, str_len);
        return (const char*)str;
    }
    mp_raise_TypeError("can't convert object to str implicitly");
}

typedef struct _ModPicoGraphics_obj_t {
    mp_obj_base_t base;
    PicoGraphics *graphics;
    DisplayDriver *display;
    void *spritedata;
    void *buffer;
    //mp_obj_t scanline_callback; // Not really feasible in MicroPython
} ModPicoGraphics_obj_t;

bool get_display_settings(PicoGraphicsDisplay display, int &width, int &height, int &rotate, int &pen_type) {
    switch(display) {
        case DISPLAY_PICO_DISPLAY:
            width = 240;
            height = 135;
            // Portrait to match labelling
            if(rotate == -1) rotate = (int)Rotation::ROTATE_270;
            if(pen_type == -1) pen_type = PEN_RGB332;
            break;
        case DISPLAY_PICO_DISPLAY_2:
        case DISPLAY_TUFTY_2040:
            width = 320;
            height = 240;
            // Tufty display is upside-down
            if(rotate == -1) rotate = (int)Rotation::ROTATE_180;
            if(pen_type == -1) pen_type = PEN_RGB332;
            break;
        case DISPLAY_PICO_EXPLORER:
        case DISPLAY_LCD_240X240:
        case DISPLAY_ENVIRO_PLUS:
            width = 240;
            height = 240;
            if(pen_type == -1) pen_type = PEN_RGB332;
            break;
        case DISPLAY_ROUND_LCD_240X240:
            width = 240;
            height = 240;
            if(pen_type == -1) pen_type = PEN_RGB332;
            break;
        case DISPLAY_LCD_160X80:
            width = 160;
            height = 80;
            if(pen_type == -1) pen_type = PEN_RGB332;
            break;
        case DISPLAY_I2C_OLED_128X128:
            width = 128;
            height = 128;
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_1BIT;
            break;
        case DISPLAY_INKY_PACK:
            width = 296;
            height = 128;
            if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
            if(pen_type == -1) pen_type = PEN_1BIT;
            break;
        case DISPLAY_INKY_FRAME:
            width = 600;
            height = 448;
	    if(rotate == -1) rotate = (int)Rotation::ROTATE_0;
	    if(pen_type == -1) pen_type = PEN_P4;
            break;
        default:
            return false;
    }
    return true;
}

size_t get_required_buffer_size(PicoGraphicsPenType pen_type, uint width, uint height) {
    switch(pen_type) {
        case PEN_1BIT:
            return PicoGraphics_Pen1Bit::buffer_size(width, height);
        case PEN_P4:
            return PicoGraphics_PenP4::buffer_size(width, height);
        case PEN_P8:
            return PicoGraphics_PenP8::buffer_size(width, height);
        case PEN_RGB332:
            return PicoGraphics_PenRGB332::buffer_size(width, height);
        case PEN_RGB565:
            return PicoGraphics_PenRGB565::buffer_size(width, height);
        default:
            return 0;
    }
}

mp_obj_t ModPicoGraphics_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    ModPicoGraphics_obj_t *self = nullptr;

    enum { ARG_display, ARG_rotate, ARG_bus, ARG_buffer, ARG_pen_type, ARG_extra_pins, ARG_i2c_address };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_display, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_rotate, MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_bus, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_buffer, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_pen_type, MP_ARG_INT, { .u_int = -1 } },
        { MP_QSTR_extra_pins, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_i2c_address, MP_ARG_INT, { .u_int = -1 } },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self = m_new_obj_with_finaliser(ModPicoGraphics_obj_t);
    self->base.type = &ModPicoGraphics_type;

    PicoGraphicsDisplay display = (PicoGraphicsDisplay)args[ARG_display].u_int;

    bool round = display == DISPLAY_ROUND_LCD_240X240;
    int width = 0;
    int height = 0;
    int pen_type = args[ARG_pen_type].u_int;
    int rotate = args[ARG_rotate].u_int;
    if(!get_display_settings(display, width, height, rotate, pen_type)) mp_raise_ValueError("Unsupported display!");
    if(rotate == -1) rotate = (int)Rotation::ROTATE_0;

    // Try to create an appropriate display driver
    if (display == DISPLAY_INKY_FRAME) {
        pen_type = PEN_P4; // FORCE to P4 since it's the only supported mode
        // TODO grab BUSY and RESET from ARG_extra_pins
        if (args[ARG_bus].u_obj == mp_const_none) {
            self->display = m_new_class(UC8159, width, height);
        } else if (mp_obj_is_type(args[ARG_bus].u_obj, &SPIPins_type)) {
            _PimoroniBus_obj_t *bus = (_PimoroniBus_obj_t *)MP_OBJ_TO_PTR(args[ARG_bus].u_obj);
            self->display = m_new_class(UC8159, width, height, *(SPIPins *)(bus->pins));
        } else {
            mp_raise_ValueError("SPIBus expected!");
        }
    }
    else if (display == DISPLAY_TUFTY_2040) {
        if (args[ARG_bus].u_obj == mp_const_none) {
            self->display = m_new_class(ST7789, width, height, (Rotation)rotate, {10, 11, 12, 13, 14, 2});
        } else if (mp_obj_is_type(args[ARG_bus].u_obj, &ParallelPins_type)) {
            _PimoroniBus_obj_t *bus = (_PimoroniBus_obj_t *)MP_OBJ_TO_PTR(args[ARG_bus].u_obj);
            self->display = m_new_class(ST7789, width, height, (Rotation)rotate, *(ParallelPins *)(bus->pins));
        } else {
            mp_raise_ValueError("ParallelBus expected!");
        }
    } else if (display == DISPLAY_LCD_160X80) {
        if (args[ARG_bus].u_obj == mp_const_none) {
            self->display = m_new_class(ST7735, width, height, get_spi_pins(BG_SPI_FRONT));
        } else if (mp_obj_is_type(args[ARG_bus].u_obj, &SPIPins_type)) {
            _PimoroniBus_obj_t *bus = (_PimoroniBus_obj_t *)MP_OBJ_TO_PTR(args[ARG_bus].u_obj);
            self->display = m_new_class(ST7735, width, height, *(SPIPins *)(bus->pins));
        } else {
            mp_raise_ValueError("SPIBus expected!");
        }
    } else if (display == DISPLAY_I2C_OLED_128X128) {
        if (mp_obj_is_type(args[ARG_bus].u_obj, &PimoroniI2C_type)) {
            _PimoroniI2C_obj_t *i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_bus].u_obj);

            int i2c_address = args[ARG_i2c_address].u_int;
            if(i2c_address == -1) i2c_address = SH1107::DEFAULT_I2C_ADDRESS;
            self->display = m_new_class(SH1107, width, height, *(pimoroni::I2C *)(i2c->i2c), (uint8_t)i2c_address);
        } else {
            mp_raise_ValueError("I2C bus required!");
        }
    } else if (display == DISPLAY_INKY_PACK) {
        if (args[ARG_bus].u_obj == mp_const_none) {
            self->display = m_new_class(UC8151, width, height, (Rotation)rotate);
        } else if (mp_obj_is_type(args[ARG_bus].u_obj, &SPIPins_type)) {
            _PimoroniBus_obj_t *bus = (_PimoroniBus_obj_t *)MP_OBJ_TO_PTR(args[ARG_bus].u_obj);
            self->display = m_new_class(UC8151, width, height, (Rotation)rotate, *(SPIPins *)(bus->pins));
        } else {
            mp_raise_ValueError("SPIBus expected!");
        }
    } else {
        if (args[ARG_bus].u_obj == mp_const_none) {
            self->display = m_new_class(ST7789, width, height, (Rotation)rotate, round, get_spi_pins(BG_SPI_FRONT));
        } else if (mp_obj_is_type(args[ARG_bus].u_obj, &SPIPins_type)) {
            _PimoroniBus_obj_t *bus = (_PimoroniBus_obj_t *)MP_OBJ_TO_PTR(args[ARG_bus].u_obj);
            self->display = m_new_class(ST7789, width, height, (Rotation)rotate, round, *(SPIPins *)(bus->pins));
        } else {
            mp_raise_ValueError("SPIBus expected!");
        }
    }

    // Create or fetch buffer
    size_t required_size = get_required_buffer_size((PicoGraphicsPenType)pen_type, width, height);
    if(required_size == 0) mp_raise_ValueError("Unsupported pen type!");

    if (args[ARG_buffer].u_obj != mp_const_none) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);
        self->buffer = bufinfo.buf;
        if(bufinfo.len < (size_t)(required_size)) {
            mp_raise_ValueError("Supplied buffer is too small!");
        }
    } else {
        self->buffer = m_new(uint8_t, required_size);
    }

    // Create an instance of the graphics library
    // use the *driver* width/height because they may have been swapped due to rotation
    switch((PicoGraphicsPenType)pen_type) {
        case PEN_1BIT:
            if (display == DISPLAY_INKY_PACK) {
                self->graphics = m_new_class(PicoGraphics_Pen1BitY, self->display->width, self->display->height, self->buffer);
            } else {
                self->graphics = m_new_class(PicoGraphics_Pen1Bit, self->display->width, self->display->height, self->buffer);
            }
            break;
        case PEN_P4:
            self->graphics = m_new_class(PicoGraphics_PenP4, self->display->width, self->display->height, self->buffer);
            break;
        case PEN_P8:
            self->graphics = m_new_class(PicoGraphics_PenP8, self->display->width, self->display->height, self->buffer);
            break;
        case PEN_RGB332:
            self->graphics = m_new_class(PicoGraphics_PenRGB332, self->display->width, self->display->height, self->buffer);
            break;
        case PEN_RGB565:
            self->graphics = m_new_class(PicoGraphics_PenRGB565, self->display->width, self->display->height, self->buffer);
            break;
        default:
            break;
    }

    //self->scanline_callback = mp_const_none;

    self->spritedata = nullptr;

    // Clear the buffer
    self->graphics->set_pen(0);
    self->graphics->clear();

    // Update the LCD from the graphics library
    if (display != DISPLAY_INKY_FRAME) {
        self->display->update(self->graphics);
    }

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t ModPicoGraphics__del__(mp_obj_t self_in) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);
    self->display->cleanup();
    return mp_const_none;
}

mp_obj_t ModPicoGraphics_set_spritesheet(mp_obj_t self_in, mp_obj_t spritedata) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);
    if(spritedata == mp_const_none) {
        self->spritedata = nullptr;
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(spritedata, &bufinfo, MP_BUFFER_RW);

        int required_size = get_required_buffer_size((PicoGraphicsPenType)self->graphics->pen_type, 128, 128);

        if(bufinfo.len != (size_t)(required_size)) {
            mp_raise_ValueError("Spritesheet the wrong size!");
        }

        self->spritedata = bufinfo.buf;
    }
    return mp_const_none;
}

mp_obj_t ModPicoGraphics_load_spritesheet(mp_obj_t self_in, mp_obj_t filename) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);
    mp_obj_t args[2] = {
        filename,
        MP_OBJ_NEW_QSTR(MP_QSTR_r),
    };

    // Stat the file to get its size
    // example tuple response: (32768, 0, 0, 0, 0, 0, 5153, 1654709815, 1654709815, 1654709815)
    mp_obj_t stat = mp_vfs_stat(filename);
    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(stat, mp_obj_tuple_t);
    size_t filesize = mp_obj_get_int(tuple->items[6]);

    mp_buffer_info_t bufinfo;
    bufinfo.buf = (void *)m_new(uint8_t, filesize);
    mp_obj_t file = mp_vfs_open(MP_ARRAY_SIZE(args), &args[0], (mp_map_t *)&mp_const_empty_map);
    int errcode;
    bufinfo.len = mp_stream_rw(file, bufinfo.buf, filesize, &errcode, MP_STREAM_RW_READ | MP_STREAM_RW_ONCE);
    if (errcode != 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to open sprite file!"));
    }

    self->spritedata = bufinfo.buf;

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_sprite(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_sprite_x, ARG_sprite_y, ARG_x, ARG_y, ARG_scale, ARG_transparent };

    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], ModPicoGraphics_obj_t);

    if(self->spritedata == nullptr) return mp_const_false;

    int scale = 1;
    int transparent = 0;

    if(n_args >= 6) scale = mp_obj_get_int(args[ARG_scale]);
    if(n_args >= 7) transparent = mp_obj_get_int(args[ARG_transparent]);

    self->graphics->sprite(
        self->spritedata,
        {mp_obj_get_int(args[ARG_sprite_x]), mp_obj_get_int(args[ARG_sprite_y])},
        {mp_obj_get_int(args[ARG_x]), mp_obj_get_int(args[ARG_y])},
        scale,
        transparent
    );

    return mp_const_true;
}

mp_obj_t ModPicoGraphics_set_font(mp_obj_t self_in, mp_obj_t font) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);
    self->graphics->set_font(mp_obj_to_string_r(font));
    return mp_const_none;
}

mp_int_t ModPicoGraphics_get_framebuffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);
    (void)flags;
    bufinfo->buf = self->graphics->frame_buffer;
    bufinfo->len = get_required_buffer_size((PicoGraphicsPenType)self->graphics->pen_type, self->graphics->bounds.w, self->graphics->bounds.h);
    bufinfo->typecode = 'B';
    return 0;
}

mp_obj_t ModPicoGraphics_set_framebuffer(mp_obj_t self_in, mp_obj_t framebuffer) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);

    if (framebuffer == mp_const_none) {
        m_del(uint8_t, self->buffer, self->graphics->bounds.w * self->graphics->bounds.h);
        self->buffer = nullptr;
        self->graphics->set_framebuffer(nullptr);
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(framebuffer, &bufinfo, MP_BUFFER_RW);

        // This should have no effect if you try to replace the framebuffer with itself
        // but it will free up a buffer that has no other references
        if(self->buffer != nullptr) {
            m_del(uint8_t, self->buffer, self->graphics->bounds.w * self->graphics->bounds.h);
        }

        self->buffer = bufinfo.buf;
        self->graphics->set_framebuffer(self->buffer);
    }
    return mp_const_none;
}

mp_obj_t ModPicoGraphics_get_required_buffer_size(mp_obj_t display_in, mp_obj_t pen_type_in) {
    PicoGraphicsDisplay display = (PicoGraphicsDisplay)mp_obj_get_int(display_in);
    int width = 0;
    int height = 0;
    int rotation = 0;
    int pen_type = mp_obj_get_int(pen_type_in);
    if(!get_display_settings(display, width, height, rotation, pen_type)) mp_raise_ValueError("Unsupported display!");
    size_t required_size = get_required_buffer_size((PicoGraphicsPenType)pen_type, width, height);
    if(required_size == 0) mp_raise_ValueError("Unsupported pen type!");

    return mp_obj_new_int(required_size);
}

mp_obj_t ModPicoGraphics_get_bounds(mp_obj_t self_in) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);
    mp_obj_t tuple[2] = {
        mp_obj_new_int(self->graphics->bounds.w),
        mp_obj_new_int(self->graphics->bounds.h)
    };
    return mp_obj_new_tuple(2, tuple);
}

/*
mp_obj_t ModPicoGraphics_set_scanline_callback(mp_obj_t self_in, mp_obj_t cb_in) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);
    self->scanline_callback = cb_in;
    return mp_const_none;
}
*/

mp_obj_t ModPicoGraphics_update(mp_obj_t self_in) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);
/*
    if(self->scanline_callback != mp_const_none) {
        self->graphics->scanline_interrupt = [self](int y){
            mp_obj_t args[] = {
                mp_obj_new_int(y)
            };
            mp_call_function_n_kw(self->scanline_callback, MP_ARRAY_SIZE(args), 0, args);
        };
    } else {
        self->graphics->scanline_interrupt = nullptr;
    }
*/

    while(self->display->is_busy()) {
    #ifdef MICROPY_EVENT_POLL_HOOK
    MICROPY_EVENT_POLL_HOOK
    #endif
    }

    self->display->update(self->graphics);

    while(self->display->is_busy()) {
    #ifdef MICROPY_EVENT_POLL_HOOK
    MICROPY_EVENT_POLL_HOOK
    #endif
    }

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_set_backlight(mp_obj_t self_in, mp_obj_t brightness) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);

    float b = mp_obj_get_float(brightness);

    if(b < 0 || b > 1.0f) mp_raise_ValueError("brightness out of range. Expected 0.0 to 1.0");

    self->display->set_backlight((uint8_t)(b * 255.0f));

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_module_RGB332_to_RGB(mp_obj_t rgb332) {
    RGB c((RGB332)mp_obj_get_int(rgb332));
    mp_obj_t t[] = {
        mp_obj_new_int(c.r),
        mp_obj_new_int(c.g),
        mp_obj_new_int(c.b),
    };
    return mp_obj_new_tuple(3, t);
}

mp_obj_t ModPicoGraphics_module_RGB565_to_RGB(mp_obj_t rgb565) {
    RGB c((RGB565)mp_obj_get_int(rgb565));
    mp_obj_t t[] = {
        mp_obj_new_int(c.r),
        mp_obj_new_int(c.g),
        mp_obj_new_int(c.b),
    };
    return mp_obj_new_tuple(3, t);
}

mp_obj_t ModPicoGraphics_module_RGB_to_RGB332(mp_obj_t r, mp_obj_t g, mp_obj_t b) {
    return mp_obj_new_int(RGB(
        mp_obj_get_int(r),
        mp_obj_get_int(g),
        mp_obj_get_int(b)
    ).to_rgb332());
}

mp_obj_t ModPicoGraphics_module_RGB_to_RGB565(mp_obj_t r, mp_obj_t g, mp_obj_t b) {
    return mp_obj_new_int(RGB(
        mp_obj_get_int(r),
        mp_obj_get_int(g),
        mp_obj_get_int(b)
    ).to_rgb565());
}

mp_obj_t ModPicoGraphics_set_pen(mp_obj_t self_in, mp_obj_t pen) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);

    self->graphics->set_pen(mp_obj_get_int(pen));

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_reset_pen(mp_obj_t self_in, mp_obj_t pen) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);

    self->graphics->reset_pen(mp_obj_get_int(pen));

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_update_pen(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_i, ARG_r, ARG_g, ARG_b };

    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], ModPicoGraphics_obj_t);

    self->graphics->update_pen(
        mp_obj_get_int(args[ARG_i]) & 0xff,
        mp_obj_get_int(args[ARG_r]) & 0xff,
        mp_obj_get_int(args[ARG_g]) & 0xff,
        mp_obj_get_int(args[ARG_b]) & 0xff
    );

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_create_pen(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_r, ARG_g, ARG_b };

    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], ModPicoGraphics_obj_t);

    int result = self->graphics->create_pen(
        mp_obj_get_int(args[ARG_r]) & 0xff,
        mp_obj_get_int(args[ARG_g]) & 0xff,
        mp_obj_get_int(args[ARG_b]) & 0xff
    );

    if (result == -1) mp_raise_ValueError("create_pen failed. No matching colour or space in palette!");

    return mp_obj_new_int(result);
}

mp_obj_t ModPicoGraphics_set_palette(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    size_t num_tuples = n_args - 1;
    const mp_obj_t *tuples = pos_args + 1;

    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], ModPicoGraphics_obj_t);

    // Check if there is only one argument, which might be a list
    if(n_args == 2) {
        if(mp_obj_is_type(pos_args[1], &mp_type_list)) {
            mp_obj_list_t *points = MP_OBJ_TO_PTR2(pos_args[1], mp_obj_list_t);

            if(points->len <= 0) mp_raise_ValueError("set_palette(): cannot provide an empty list");

            num_tuples = points->len;
            tuples = points->items;
        }
        else {
            mp_raise_TypeError("set_palette(): can't convert object to list");
        }
    }

    for(size_t i = 0; i < num_tuples; i++) {
        mp_obj_t obj = tuples[i];
        if(!mp_obj_is_type(obj, &mp_type_tuple)) mp_raise_ValueError("set_palette(): can't convert object to tuple");

        mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(obj, mp_obj_tuple_t);

        if(tuple->len != 3) mp_raise_ValueError("set_palette(): tuple must contain R, G, B values");

        self->graphics->update_pen(
            i,
            mp_obj_get_int(tuple->items[0]),
            mp_obj_get_int(tuple->items[1]),
            mp_obj_get_int(tuple->items[2])
        );
    }

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_set_clip(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_w, ARG_h };

    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], ModPicoGraphics_obj_t);

    self->graphics->set_clip({
        mp_obj_get_int(args[ARG_x]),
        mp_obj_get_int(args[ARG_y]),
        mp_obj_get_int(args[ARG_w]),
        mp_obj_get_int(args[ARG_h])
    });

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_remove_clip(mp_obj_t self_in) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);

    self->graphics->remove_clip();

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_clear(mp_obj_t self_in) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);

    self->graphics->clear();

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_pixel(mp_obj_t self_in, mp_obj_t x, mp_obj_t y) {
    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(self_in, ModPicoGraphics_obj_t);

    self->graphics->pixel({
        mp_obj_get_int(x),
        mp_obj_get_int(y)
    });

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_pixel_span(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_l };

    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], ModPicoGraphics_obj_t);

    self->graphics->pixel_span({
        mp_obj_get_int(args[ARG_x]),
        mp_obj_get_int(args[ARG_y])
    },  mp_obj_get_int(args[ARG_l]));

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_rectangle(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_w, ARG_h };

    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], ModPicoGraphics_obj_t);

    self->graphics->rectangle({
        mp_obj_get_int(args[ARG_x]),
        mp_obj_get_int(args[ARG_y]),
        mp_obj_get_int(args[ARG_w]),
        mp_obj_get_int(args[ARG_h])
    });

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_circle(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_r };

    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], ModPicoGraphics_obj_t);

    self->graphics->circle({
        mp_obj_get_int(args[ARG_x]),
        mp_obj_get_int(args[ARG_y])
    },  mp_obj_get_int(args[ARG_r]));

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_character(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_char, ARG_x, ARG_y, ARG_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_char, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_scale, MP_ARG_INT, {.u_int = 2} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, ModPicoGraphics_obj_t);

    int c = mp_obj_get_int(args[ARG_char].u_obj);
    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int scale = args[ARG_scale].u_int;

    self->graphics->character((char)c, Point(x, y), scale);

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_text, ARG_x, ARG_y, ARG_wrap, ARG_scale, ARG_angle, ARG_spacing };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_text, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_wordwrap, MP_ARG_INT, {.u_int = __INT32_MAX__} },  // Sort-of a fudge, but wide-enough to avoid wrapping on any display size
        { MP_QSTR_scale, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_angle, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_spacing, MP_ARG_INT, {.u_int = 1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, ModPicoGraphics_obj_t);

    mp_obj_t text_obj = args[ARG_text].u_obj;

    if(!mp_obj_is_str_or_bytes(text_obj)) mp_raise_TypeError("text: string required");

    GET_STR_DATA_LEN(text_obj, str, str_len);

    std::string t((const char*)str);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int wrap = args[ARG_wrap].u_int;
    float scale = args[ARG_scale].u_obj == mp_const_none ? 2.0f : mp_obj_get_float(args[ARG_scale].u_obj);
    int angle = args[ARG_angle].u_int;
    int letter_spacing = args[ARG_spacing].u_int;

    self->graphics->text(t, Point(x, y), wrap, scale, angle, letter_spacing);

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_measure_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_text, ARG_scale, ARG_spacing };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_text, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_scale, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_spacing, MP_ARG_INT, {.u_int = 1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, ModPicoGraphics_obj_t);

    mp_obj_t text_obj = args[ARG_text].u_obj;

    if(!mp_obj_is_str_or_bytes(text_obj)) mp_raise_TypeError("text: string required");

    GET_STR_DATA_LEN(text_obj, str, str_len);

    std::string t((const char*)str);

    float scale = args[ARG_scale].u_obj == mp_const_none ? 2.0f : mp_obj_get_float(args[ARG_scale].u_obj);
    int letter_spacing = args[ARG_spacing].u_int;

    int width = self->graphics->measure_text(t, scale, letter_spacing);

    return mp_obj_new_int(width);
}

mp_obj_t ModPicoGraphics_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    size_t num_tuples = n_args - 1;
    const mp_obj_t *tuples = pos_args + 1;

    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], ModPicoGraphics_obj_t);

    // Check if there is only one argument, which might be a list
    if(n_args == 2) {
        if(mp_obj_is_type(pos_args[1], &mp_type_list)) {
            mp_obj_list_t *points = MP_OBJ_TO_PTR2(pos_args[1], mp_obj_list_t);

            if(points->len <= 0) mp_raise_ValueError("poly(): cannot provide an empty list");

            num_tuples = points->len;
            tuples = points->items;
        }
        else {
            mp_raise_TypeError("poly(): can't convert object to list");
        }
    }

    if(num_tuples > 0) {
        std::vector<Point> points;
        for(size_t i = 0; i < num_tuples; i++) {
            mp_obj_t obj = tuples[i];
            if(!mp_obj_is_type(obj, &mp_type_tuple)) mp_raise_ValueError("poly(): can't convert object to tuple");

            mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(obj, mp_obj_tuple_t);

            if(tuple->len != 2) mp_raise_ValueError("poly(): tuple must only contain two numbers");

            points.push_back({
                mp_obj_get_int(tuple->items[0]),
                mp_obj_get_int(tuple->items[1])
            });
        }
        self->graphics->polygon(points);
    }

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_triangle(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_x1, ARG_y1, ARG_x2, ARG_y2, ARG_x3, ARG_y3 };

    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], ModPicoGraphics_obj_t);

    self->graphics->triangle(
        {mp_obj_get_int(args[ARG_x1]),
         mp_obj_get_int(args[ARG_y1])},
        {mp_obj_get_int(args[ARG_x2]),
         mp_obj_get_int(args[ARG_y2])},
        {mp_obj_get_int(args[ARG_x3]),
         mp_obj_get_int(args[ARG_y3])}
    );

    return mp_const_none;
}

mp_obj_t ModPicoGraphics_line(size_t n_args, const mp_obj_t *args) {
    enum { ARG_self, ARG_x1, ARG_y1, ARG_x2, ARG_y2 };

    ModPicoGraphics_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self], ModPicoGraphics_obj_t);

    self->graphics->line(
        {mp_obj_get_int(args[ARG_x1]),
         mp_obj_get_int(args[ARG_y1])},
        {mp_obj_get_int(args[ARG_x2]),
         mp_obj_get_int(args[ARG_y2])}
    );

    return mp_const_none;
}
}
