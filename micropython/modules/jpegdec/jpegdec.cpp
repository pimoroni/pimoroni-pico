#include "libraries/jpegdec/JPEGDEC.h"

#include "micropython/modules/util.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

extern "C" {
#include "jpegdec.h"
#include "micropython/modules/picographics/picographics.h"
#include "py/stream.h"
#include "py/reader.h"
#include "extmod/vfs.h"

typedef struct _ModPicoGraphics_obj_t {
    mp_obj_base_t base;
    PicoGraphics *graphics;
    DisplayDriver *display;
    void *buffer;
} ModPicoGraphics_obj_t;

typedef struct _JPEG_obj_t {
    mp_obj_base_t base;
    JPEGDEC *jpeg;
    void *dither_buffer;
    mp_obj_t file;
    mp_buffer_info_t buf;
    ModPicoGraphics_obj_t *graphics;
} _JPEG_obj_t;


uint8_t current_flags = 0;

enum FLAGS : uint8_t {
    FLAG_NO_DITHER = 1u
};


void *jpegdec_open_callback(const char *filename, int32_t *size) {
    mp_obj_t fn = mp_obj_new_str(filename, (mp_uint_t)strlen(filename));

    mp_obj_t args[2] = {
        fn,
        MP_OBJ_NEW_QSTR(MP_QSTR_r),
    };

    // Stat the file to get its size
    // example tuple response: (32768, 0, 0, 0, 0, 0, 5153, 1654709815, 1654709815, 1654709815)
    mp_obj_t stat = mp_vfs_stat(fn);
    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(stat, mp_obj_tuple_t);
    *size = mp_obj_get_int(tuple->items[6]);

    mp_obj_t fhandle = mp_vfs_open(MP_ARRAY_SIZE(args), &args[0], (mp_map_t *)&mp_const_empty_map);

    return (void *)fhandle;
}

void jpegdec_close_callback(void *handle) {
    mp_stream_close((mp_obj_t)handle);
}

int32_t jpegdec_read_callback(JPEGFILE *jpeg, uint8_t *p, int32_t c) {
    mp_obj_t fhandle = jpeg->fHandle;
    int error;
    return mp_stream_read_exactly(fhandle, p, c, &error);
}

// Re-implementation of stream.c/STATIC mp_obj_t stream_seek(size_t n_args, const mp_obj_t *args)
int32_t jpegdec_seek_callback(JPEGFILE *jpeg, int32_t p) {
    mp_obj_t fhandle = jpeg->fHandle;
    struct mp_stream_seek_t seek_s;
    seek_s.offset = p;
    seek_s.whence = SEEK_SET;

    const mp_stream_p_t *stream_p = mp_get_stream(fhandle);

    int error;
    mp_uint_t res = stream_p->ioctl(fhandle, MP_STREAM_SEEK, (mp_uint_t)(uintptr_t)&seek_s, &error);
    if (res == MP_STREAM_ERROR) {
        mp_raise_OSError(error);
    }

    return seek_s.offset;
}

int JPEGDraw(JPEGDRAW *pDraw) {
#ifdef mp_event_handle_nowait
mp_event_handle_nowait();
#endif
    PicoGraphics *current_graphics = (PicoGraphics *)pDraw->pUser;
    // "pixel" is slow and clipped,
    // guaranteeing we wont draw jpeg data out of the framebuffer..
    // Can we clip beforehand and make this faster?
    if(pDraw->iBpp == 8) {
        uint8_t *pixel = (uint8_t *)pDraw->pPixels;
        for(int y = 0; y < pDraw->iHeight; y++) {
            for(int x = 0; x < pDraw->iWidth; x++) {
                if(x < pDraw->iWidthUsed) { // Clip to the used width
                    current_graphics->set_pen((uint8_t)(*pixel >> 4));
                    current_graphics->pixel({pDraw->x + x, pDraw->y + y});
                }
                pixel++;
            }
        }
    } else if(pDraw->iBpp == 4) {
        uint8_t *pixels = (uint8_t *)pDraw->pPixels;
        for(int y = 0; y < pDraw->iHeight; y++) {
            for(int x = 0; x < pDraw->iWidth; x++) {
                if(x >= pDraw->iWidthUsed) break; // Clip to the used width
                int i = y * pDraw->iWidth + x;
                uint8_t p = pixels[i / 2];
                p >>= (i & 0b1) ? 0 : 4;
                p &= 0xf;
                current_graphics->set_pen(p);
                current_graphics->pixel({pDraw->x + x, pDraw->y + y});
            }
        }
    } else if(pDraw->iBpp == 1) {
        uint8_t *pixels = (uint8_t *)pDraw->pPixels;
        for(int y = 0; y < pDraw->iHeight; y++) {
            for(int x = 0; x < pDraw->iWidth; x++) {
                if(x >= pDraw->iWidthUsed) break; // Clip to the used width
                int i = y * pDraw->iWidth + x;
                uint8_t p = pixels[i / 8];
                p >>= 7 - (i & 0b111);
                p &= 0x1;
                current_graphics->set_pen(p);
                current_graphics->pixel({pDraw->x + x, pDraw->y + y});
            }
        }
    } else {
        for(int y = 0; y < pDraw->iHeight; y++) {
            for(int x = 0; x < pDraw->iWidth; x++) {
                if(x >= pDraw->iWidthUsed) break; // Clip to the used width
                int i = y * pDraw->iWidth + x;
                if (current_graphics->pen_type == PicoGraphics::PEN_RGB332) {
                    if (current_flags & FLAG_NO_DITHER) {
                        // Posterized output to RGB332
                        current_graphics->set_pen(RGB((RGB565)pDraw->pPixels[i]).to_rgb332());
                        current_graphics->pixel({pDraw->x + x, pDraw->y + y});
                    } else {
                        // Dithered output to RGB332
                        current_graphics->set_pixel_dither({pDraw->x + x, pDraw->y + y}, (RGB565)(pDraw->pPixels[i]));
                    }
                } else if (current_graphics->pen_type == PicoGraphics::PEN_RGB888
                || current_graphics->pen_type == PicoGraphics::PEN_DV_RGB888) {
                    current_graphics->set_pen(RGB((RGB565)pDraw->pPixels[i]).to_rgb888());
                    current_graphics->pixel({pDraw->x + x, pDraw->y + y});
                } else if (current_graphics->pen_type == PicoGraphics::PEN_P8 
                || current_graphics->pen_type == PicoGraphics::PEN_P4
                || current_graphics->pen_type == PicoGraphics::PEN_DV_P5
                || current_graphics->pen_type == PicoGraphics::PEN_3BIT
                || current_graphics->pen_type == PicoGraphics::PEN_INKY7) {
                    if (current_flags & FLAG_NO_DITHER) {
                        int closest = RGB((RGB565)pDraw->pPixels[i]).closest(current_graphics->get_palette(), current_graphics->get_palette_size());
                        if (closest == -1) {
                            closest = 0;
                        }
                        current_graphics->set_pen(closest);
                        current_graphics->pixel({pDraw->x + x, pDraw->y + y});
                    } else {
                        current_graphics->set_pixel_dither({pDraw->x + x, pDraw->y + y}, RGB((RGB565)pDraw->pPixels[i]));
                    }
                } else if (current_graphics->pen_type == PicoGraphics::PEN_DV_RGB555) {
                    current_graphics->set_pen(RGB((RGB565)pDraw->pPixels[i]).to_rgb555());
                    current_graphics->pixel({pDraw->x + x, pDraw->y + y});
                } else {
                    current_graphics->set_pen(pDraw->pPixels[i]);
                    current_graphics->pixel({pDraw->x + x, pDraw->y + y});
                } 
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
    self->jpeg->close();
    return mp_const_none;
}

// open_FILE
mp_obj_t _JPEG_openFILE(mp_obj_t self_in, mp_obj_t filename) {
    _JPEG_obj_t *self = MP_OBJ_TO_PTR2(self_in, _JPEG_obj_t);

    // TODO Check for valid filename, and maybe that file exists?

    self->file = filename;

    return mp_const_true;
}

// open_RAM
mp_obj_t _JPEG_openRAM(mp_obj_t self_in, mp_obj_t buffer) {
    _JPEG_obj_t *self = MP_OBJ_TO_PTR2(self_in, _JPEG_obj_t);

    // TODO Check for valid buffer

    self->file = buffer;

    return mp_const_true;
}

// decode
mp_obj_t _JPEG_decode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_scale, ARG_dither };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_INT, {.u_int = 0}  },
        { MP_QSTR_y, MP_ARG_INT, {.u_int = 0}  },
        { MP_QSTR_scale, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_dither, MP_ARG_OBJ, {.u_obj = mp_const_true} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _JPEG_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _JPEG_obj_t);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int f = args[ARG_scale].u_int;

    current_flags = args[ARG_dither].u_obj == mp_const_false ? FLAG_NO_DITHER : 0;

    // Just-in-time open of the filename/buffer we stored in self->file via open_RAM or open_file

    // Source is a filename
    int result = -1;

    if(mp_obj_is_str(self->file)){
        GET_STR_DATA_LEN(self->file, str, str_len);

        result = self->jpeg->open(
            (const char*)str,
            jpegdec_open_callback,
            jpegdec_close_callback,
            jpegdec_read_callback,
            jpegdec_seek_callback,
            JPEGDraw);

    // Source is a buffer
    } else {
        mp_get_buffer_raise(self->file, &self->buf, MP_BUFFER_READ);

        result = self->jpeg->openRAM((uint8_t *)self->buf.buf, self->buf.len, JPEGDraw);
    }
    
    if(result != 1) mp_raise_msg(&mp_type_RuntimeError, "JPEG: could not read file/buffer.");

    // Force a specific data output type to best match our PicoGraphics buffer
    switch(self->graphics->graphics->pen_type) {
        case PicoGraphics::PEN_RGB332:
        case PicoGraphics::PEN_RGB565:
        case PicoGraphics::PEN_RGB888:
        case PicoGraphics::PEN_P8:
        case PicoGraphics::PEN_P4:
        case PicoGraphics::PEN_3BIT:
        case PicoGraphics::PEN_INKY7:
        case PicoGraphics::PEN_DV_RGB555:
        case PicoGraphics::PEN_DV_RGB888:
        case PicoGraphics::PEN_DV_P5:
            self->jpeg->setPixelType(RGB565_BIG_ENDIAN);
            break;
        // TODO 2-bit is currently unsupported
        case PicoGraphics::PEN_P2:
        case PicoGraphics::PEN_1BIT:
            self->jpeg->setPixelType(EIGHT_BIT_GRAYSCALE);
            break;
    }

    // We need to store a pointer to the PicoGraphics surface
    self->jpeg->setUserPointer((void *)self->graphics->graphics);

    result = self->jpeg->decode(x, y, f);

    current_flags = 0;

    // Close the file since we've opened it on-demand
    self->jpeg->close();

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
