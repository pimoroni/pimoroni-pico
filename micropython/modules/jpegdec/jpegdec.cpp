#include "libraries/jpegdec/JPEGDEC.h"

#include "micropython/modules/util.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

extern "C" {
#include "jpegdec.h"
#include "micropython/modules/picographics/picographics.h"

typedef struct _ModPicoGraphics_obj_t {
    mp_obj_base_t base;
    PicoGraphics *graphics;
    DisplayDriver *display;
    void *buffer;
} ModPicoGraphics_obj_t;

typedef struct _JPEG_obj_t {
    mp_obj_base_t base;
    JPEGDEC *jpeg;
    mp_obj_t callback;
    ModPicoGraphics_obj_t *graphics;
} _JPEG_obj_t;

PicoGraphics *current_graphics = nullptr;

int JPEGDraw(JPEGDRAW *pDraw) {
    // "pixel" is slow and clipped,
    // guaranteeing we wont draw jpeg data out of the framebuffer..
    // Can we clip beforehand and make this faster?
    if(pDraw->iBpp == 4) { // TODO 4-bit pixel unpacking isn't working. What's up?
        uint8_t *pixels = (uint8_t *)pDraw->pPixels;
        for(int y = 0; y < pDraw->iHeight; y++) {
            for(int x = 0; x < pDraw->iWidth; x++) {
                int i = y * pDraw->iWidth + x;
                uint8_t p = pixels[i / 2];
                p >>= (i & 0b1) * 4;
                p &= 0xf;
                current_graphics->set_pen(p);
                current_graphics->pixel({pDraw->x + x, pDraw->y + y});
            }
        }
    } else if(pDraw->iBpp == 8) {
        uint8_t *pixels = (uint8_t *)pDraw->pPixels;
        for(int y = 0; y < pDraw->iHeight; y++) {
            for(int x = 0; x < pDraw->iWidth; x++) {
                int i = y * pDraw->iWidth + x;
                current_graphics->set_pen(pixels[i] >> (current_graphics->pen_type == PicoGraphics::PEN_P4 ? 4 : 0));
                current_graphics->pixel({pDraw->x + x, pDraw->y + y});
            }
        }
    } else {
        for(int y = 0; y < pDraw->iHeight; y++) {
            for(int x = 0; x < pDraw->iWidth; x++) {
                int i = y * pDraw->iWidth + x;
                if (current_graphics->pen_type == PicoGraphics::PEN_RGB332) {
                    current_graphics->set_pen(PicoGraphics::rgb565_to_rgb332(pDraw->pPixels[i]));
                } else {
                    current_graphics->set_pen(pDraw->pPixels[i]);
                } 
                current_graphics->pixel({pDraw->x + x, pDraw->y + y});
            }
        }
    }
    return 1;
}

mp_obj_t _JPEG_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum {
        ARG_picographics
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_picographics, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!MP_OBJ_IS_TYPE(args[ARG_picographics].u_obj, &ModPicoGraphics_type)) mp_raise_ValueError(MP_ERROR_TEXT("PicoGraphics Object Required"));

    _JPEG_obj_t *self = m_new_obj_with_finaliser(_JPEG_obj_t);
    self->base.type = &JPEG_type;
    self->jpeg = m_new_class(JPEGDEC);
    self->graphics = (ModPicoGraphics_obj_t *)MP_OBJ_TO_PTR(args[ARG_picographics].u_obj);
    return self;
}

mp_obj_t _JPEG_del(mp_obj_t self_in) {
    _JPEG_obj_t *self = MP_OBJ_TO_PTR2(self_in, _JPEG_obj_t);
    (void)self;
    return mp_const_none;
}

// open_RAM
mp_obj_t _JPEG_openRAM(mp_obj_t self_in, mp_obj_t buffer) {
    _JPEG_obj_t *self = MP_OBJ_TO_PTR2(self_in, _JPEG_obj_t);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer, &bufinfo, MP_BUFFER_READ);
    int result = self->jpeg->openRAM((uint8_t *)bufinfo.buf, bufinfo.len, JPEGDraw);
    if (result == 1) {
        switch(self->graphics->graphics->pen_type) {
            case PicoGraphics::PEN_RGB332:
            case PicoGraphics::PEN_RGB565:
                self->jpeg->setPixelType(RGB565_BIG_ENDIAN);
                break;
            case PicoGraphics::PEN_P8:
                self->jpeg->setPixelType(EIGHT_BIT_GRAYSCALE);
                break;
            // TODO currently uses EIGHT_BIT_GREYSCALE and shifts down should this use a 4-bit dither?
            case PicoGraphics::PEN_P4:
                self->jpeg->setPixelType(EIGHT_BIT_GRAYSCALE);
                break;
            // TODO 2-bit is currently unsupported
            case PicoGraphics::PEN_P2:
                self->jpeg->setPixelType(TWO_BIT_DITHERED);
                break;
        }
    }
    return result == 1 ? mp_const_true : mp_const_false;
}

// decode
mp_obj_t _JPEG_decode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_scale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_INT, {.u_int = 0}  },
        { MP_QSTR_y, MP_ARG_INT, {.u_int = 0}  },
        { MP_QSTR_scale, MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _JPEG_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _JPEG_obj_t);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int f = args[ARG_scale].u_int;

    // We need to store a pointer to the PicoGraphics surface
    // since the JPEGDRAW struct has no userdata
    current_graphics = self->graphics->graphics;
    int result = self->jpeg->decode(x, y, f);
    current_graphics = nullptr;
    return result == 1 ? mp_const_true : mp_const_false;
}

// get_width
mp_obj_t _JPEG_getWidth(mp_obj_t self_in) {
    _JPEG_obj_t *self = MP_OBJ_TO_PTR2(self_in, _JPEG_obj_t);
    return mp_obj_new_int(self->jpeg->getWidth());
}

// get_height
mp_obj_t _JPEG_getHeight(mp_obj_t self_in) {
    _JPEG_obj_t *self = MP_OBJ_TO_PTR2(self_in, _JPEG_obj_t);
    return mp_obj_new_int(self->jpeg->getHeight());
}

}