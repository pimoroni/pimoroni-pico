#include "libraries/pngdec/PNGdec.h"

#include "micropython/modules/util.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

extern "C" {
#include "pngdec.h"
#include "micropython/modules/picographics/picographics.h"
#include "py/stream.h"
#include "py/reader.h"
#include "extmod/vfs.h"

typedef struct _ModPicoGraphics_obj_t {
    mp_obj_base_t base;
    PicoGraphics *graphics;
    void *display;
} ModPicoGraphics_obj_t;

typedef struct _PNG_decode_target {
    void *target;
    uint8_t mode = 0;
    Point position = {0, 0};
    Rect source = {0, 0, 0, 0};
    Point scale = {1, 1};
    int rotation = 0;
} _PNG_decode_target;

typedef struct _PNG_obj_t {
    mp_obj_base_t base;
    PNG *png;
    void *dither_buffer;
    mp_obj_t file;
    mp_buffer_info_t buf;
    PNG_DRAW_CALLBACK *decode_callback;
    _PNG_decode_target *decode_target;
    bool decode_into_buffer;
    int width;
    int height;
} _PNG_obj_t;

enum DECODE_MODE : uint8_t {
    MODE_POSTERIZE = 0u,
    MODE_DITHER = 1u,
    MODE_COPY = 2u,
};

void *pngdec_open_callback(const char *filename, int32_t *size) {
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

void pngdec_close_callback(void *handle) {
    mp_stream_close((mp_obj_t)handle);
}

int32_t pngdec_read_callback(PNGFILE *png, uint8_t *p, int32_t c) {
    mp_obj_t fhandle = png->fHandle;
    int error;
    return mp_stream_read_exactly(fhandle, p, c, &error);
}

// Re-implementation of stream.c/STATIC mp_obj_t stream_seek(size_t n_args, const mp_obj_t *args)
int32_t pngdec_seek_callback(PNGFILE *png, int32_t p) {
    mp_obj_t fhandle = png->fHandle;
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

void pngdec_open_helper(_PNG_obj_t *self) {
    int result = -1;

    if(mp_obj_is_str(self->file)){
        GET_STR_DATA_LEN(self->file, str, str_len);

        result = self->png->open(
            (const char*)str,
            pngdec_open_callback,
            pngdec_close_callback,
            pngdec_read_callback,
            pngdec_seek_callback,
            self->decode_callback);

    // Source is a buffer
    } else {
        mp_get_buffer_raise(self->file, &self->buf, MP_BUFFER_READ);

        result = self->png->openRAM((uint8_t *)self->buf.buf, self->buf.len, self->decode_callback);
    }

    if(result != 0) mp_raise_msg(&mp_type_RuntimeError, "PNG: could not read file/buffer.");
}

void PNGDraw(PNGDRAW *pDraw) {
#ifdef mp_event_handle_nowait
mp_event_handle_nowait();
#endif
    _PNG_decode_target *target = (_PNG_decode_target*)pDraw->pUser;
    PicoGraphics *current_graphics = (PicoGraphics *)target->target;
    Point current_position = target->position;
    uint8_t current_mode = target->mode;
    Point scale = target->scale;
    int rotation = target->rotation;
    Point step = {0, 0};

    // "pixel" is slow and clipped,
    // guaranteeing we wont draw png data out of the framebuffer..
    // Can we clip beforehand and make this faster?

    if(pDraw->y < target->source.y || pDraw->y >= target->source.y + target->source.h) return;

    switch (rotation) {
        case 0:
            current_position.y += (pDraw->y - target->source.y) * scale.y;
            step = {scale.x, 0};
            break;
        case 90:
            current_position.y += target->source.w * scale.y;
            current_position.x += target->source.h * scale.x;
            current_position.x += (pDraw->y - target->source.y) * -scale.x;
            step = {0, -scale.y};
            break;
        case 180:
            current_position.x += target->source.w * scale.x;
            current_position.y += target->source.h * scale.y;
            current_position.y += (pDraw->y - target->source.y) * -scale.y;
            step = {-scale.x, 0};
            break;
        case 270:
            current_position.x += (pDraw->y - target->source.y) * scale.x;
            step = {0, scale.y};
            break;
    }

    //mp_printf(&mp_plat_print, "Drawing scanline at %d, %dbpp, type: %d, width: %d pitch: %d alpha: %d\n", y, pDraw->iBpp, pDraw->iPixelType, pDraw->iWidth, pDraw->iPitch, pDraw->iHasAlpha);
    uint8_t *pixel = (uint8_t *)pDraw->pPixels;
    if(pDraw->iPixelType == PNG_PIXEL_TRUECOLOR ) {
        for(int x = 0; x < pDraw->iWidth; x++) {
            uint8_t r = *pixel++;
            uint8_t g = *pixel++;
            uint8_t b = *pixel++;
            if(x < target->source.x || x >= target->source.x + target->source.w) continue;
            current_graphics->set_pen(r, g, b);
            current_graphics->rectangle({current_position.x, current_position.y, scale.x, scale.y});
            current_position += step;
        }
    } else if (pDraw->iPixelType == PNG_PIXEL_TRUECOLOR_ALPHA) {
        for(int x = 0; x < pDraw->iWidth; x++) {
            uint8_t r = *pixel++;
            uint8_t g = *pixel++;
            uint8_t b = *pixel++;
            uint8_t a = *pixel++;
            if(x < target->source.x || x >= target->source.x + target->source.w) continue;
            if (a) {
                current_graphics->set_pen(r, g, b);
                current_graphics->rectangle({current_position.x, current_position.y, scale.x, scale.y});
            }
            current_position += step;
        }
    } else if (pDraw->iPixelType == PNG_PIXEL_INDEXED) {
        for(int x = 0; x < pDraw->iWidth; x++) {
            uint8_t i = 0;
            if(pDraw->iBpp == 8) {
                i = *pixel++;
            } else if (pDraw->iBpp == 4) {
                i = *pixel;
                i >>= (x & 0b1) ? 0 : 4;
                i &= 0xf;
                if (x & 1) pixel++;
            } else {
                i = *pixel;
                i >>= 6 - ((x & 0b11) << 1);
                i &= 0x3;
                if ((x & 0b11) == 0b11) pixel++;
            }
            if(x < target->source.x || x >= target->source.x + target->source.w) continue;
            // grab the colour from the palette
            uint8_t r = pDraw->pPalette[(i * 3) + 0];
            uint8_t g = pDraw->pPalette[(i * 3) + 1];
            uint8_t b = pDraw->pPalette[(i * 3) + 2];
            uint8_t a = pDraw->iHasAlpha ? pDraw->pPalette[768 + i] : 1;
            if (a) {
                if (current_graphics->pen_type == PicoGraphics::PEN_RGB332) {
                    if (current_mode == MODE_POSTERIZE || current_mode == MODE_COPY) {
                        // Posterized output to RGB332
                        current_graphics->set_pen(RGB(r, g, b).to_rgb332());
                        current_graphics->rectangle({current_position.x, current_position.y, scale.x, scale.y});
                    } else {
                        // Dithered output to RGB332
                        for(auto px = 0; px < scale.x; px++) {
                            for(auto py = 0; py < scale.y; py++) {
                                current_graphics->set_pixel_dither(current_position + Point{px, py}, {r, g, b});
                            }
                        }
                    }
                } else if(current_graphics->pen_type == PicoGraphics::PEN_P8
                    || current_graphics->pen_type == PicoGraphics::PEN_P4
                    || current_graphics->pen_type == PicoGraphics::PEN_3BIT
                    || current_graphics->pen_type == PicoGraphics::PEN_INKY7) {

                        // Copy raw palette indexes over
                        if(current_mode == MODE_COPY) {
                            current_graphics->set_pen(i);
                            current_graphics->rectangle({current_position.x, current_position.y, scale.x, scale.y});
                        // Posterized output to the available palete
                        } else if(current_mode == MODE_POSTERIZE) {
                            int closest = RGB(r, g, b).closest(current_graphics->get_palette(), current_graphics->get_palette_size());
                            if (closest == -1) {
                                closest = 0;
                            }
                            current_graphics->set_pen(closest);
                            current_graphics->rectangle({current_position.x, current_position.y, scale.x, scale.y});
                        } else {
                            for(auto px = 0; px < scale.x; px++) {
                                for(auto py = 0; py < scale.y; py++) {
                                    current_graphics->set_pixel_dither(current_position + Point{px, py}, {r, g, b});
                                }
                            }
                        }

                } else {
                    current_graphics->set_pen(r, g, b);
                    current_graphics->rectangle({current_position.x, current_position.y, scale.x, scale.y});
                }
            }
            current_position += step;
        }
    }
}

mp_obj_t _PNG_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum {
        ARG_picographics
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_picographics, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!MP_OBJ_IS_TYPE(args[ARG_picographics].u_obj, &ModPicoGraphics_type)) mp_raise_ValueError(MP_ERROR_TEXT("PicoGraphics Object Required"));

    _PNG_obj_t *self = m_new_obj_with_finaliser(_PNG_obj_t);
    self->base.type = &PNG_type;
    self->png = m_new_class(PNG);

    //mp_printf(&mp_plat_print, "PNG RAM %fK\n", sizeof(PNG) / 1024.0f);

    ModPicoGraphics_obj_t *graphics = (ModPicoGraphics_obj_t *)MP_OBJ_TO_PTR(args[ARG_picographics].u_obj);

    self->decode_callback = PNGDraw;
    self->decode_target = m_new(_PNG_decode_target, 1);
    self->decode_target->target = (void *)graphics->graphics;
    self->decode_into_buffer = false;

    return self;
}

mp_obj_t _PNG_del(mp_obj_t self_in) {
    _PNG_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PNG_obj_t);
    self->png->close();
    return mp_const_none;
}

// open_FILE
mp_obj_t _PNG_openFILE(mp_obj_t self_in, mp_obj_t filename) {
    _PNG_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PNG_obj_t);

    // TODO Check for valid filename, and maybe that file exists?

    self->file = filename;

    pngdec_open_helper(self);
    self->width = self->png->getWidth();
    self->height = self->png->getHeight();
    self->png->close();

    return mp_const_true;
}

// open_RAM
mp_obj_t _PNG_openRAM(mp_obj_t self_in, mp_obj_t buffer) {
    _PNG_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PNG_obj_t);

    // TODO Check for valid buffer

    self->file = buffer;

    pngdec_open_helper(self);
    self->width = self->png->getWidth();
    self->height = self->png->getHeight();
    self->png->close();

    return mp_const_true;
}

// decode
mp_obj_t _PNG_decode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_scale, ARG_mode, ARG_source, ARG_rotate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_INT, {.u_int = 0}  },
        { MP_QSTR_y, MP_ARG_INT, {.u_int = 0}  },
        { MP_QSTR_scale, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_mode, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_source, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_rotate, MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _PNG_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _PNG_obj_t);

    if(mp_obj_is_type(args[ARG_source].u_obj, &mp_type_tuple)){
        mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(args[ARG_source].u_obj, mp_obj_tuple_t);

        if(tuple->len != 4) mp_raise_ValueError("decode(): source tuple must contain (x, y, w, h)");

        self->decode_target->source = {
            mp_obj_get_int(tuple->items[0]),
            mp_obj_get_int(tuple->items[1]),
            mp_obj_get_int(tuple->items[2]),
            mp_obj_get_int(tuple->items[3])
        };
    } else {
        self->decode_target->source = {0, 0, self->width, self->height};
    }

    self->decode_target->rotation = args[ARG_rotate].u_int;
    switch(self->decode_target->rotation) {
        case 0:
        case 90:
        case 180:
        case 270:
            break;
        default:
            mp_raise_ValueError("decode(): rotation must be one of 0, 90, 180 or 270");
            break;
    }

    // Scale is a single int, corresponds to both width/height
    if (mp_obj_is_int(args[ARG_scale].u_obj)) {
        self->decode_target->scale = {
            mp_obj_get_int(args[ARG_scale].u_obj),
            mp_obj_get_int(args[ARG_scale].u_obj)
        };
    // Scale is a tuple, separate scales for width/height
    } else if(mp_obj_is_type(args[ARG_scale].u_obj, &mp_type_tuple)){
        mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(args[ARG_scale].u_obj, mp_obj_tuple_t);

        if(tuple->len != 2) mp_raise_ValueError("decode(): scale tuple must contain (scale_x, scale_y)");

        self->decode_target->scale = {
            mp_obj_get_int(tuple->items[0]),
            mp_obj_get_int(tuple->items[1])
        };
    // Something else, just roll with the default
    } else {
        self->decode_target->scale = {1, 1};
    }

    self->decode_target->mode = args[ARG_mode].u_int;

    self->decode_target->position = {args[ARG_x].u_int, args[ARG_y].u_int};

    // Just-in-time open of the filename/buffer we stored in self->file via open_RAM or open_file

    // Source is a filename
    int result = -1;

    pngdec_open_helper(self);

    result = self->png->decode(self->decode_target, 0);

    // Close the file since we've opened it on-demand
    self->png->close();

    return result == 1 ? mp_const_true : mp_const_false;
}

// get_width
mp_obj_t _PNG_getWidth(mp_obj_t self_in) {
    _PNG_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PNG_obj_t);
    return mp_obj_new_int(self->width);
}

// get_height
mp_obj_t _PNG_getHeight(mp_obj_t self_in) {
    _PNG_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PNG_obj_t);
    return mp_obj_new_int(self->height);
}

// get_height
mp_obj_t _PNG_getPalette(mp_obj_t self_in) {
    _PNG_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PNG_obj_t);
    pngdec_open_helper(self);

    self->png->decode(nullptr, 0);

    uint8_t *palette = self->png->getPalette();

    mp_obj_t palette_out[256];

    for(auto i = 0u; i < 256; i++) {
        mp_obj_t entry[3] = {
            mp_obj_new_int(*palette++),
            mp_obj_new_int(*palette++),
            mp_obj_new_int(*palette++)
        };
        palette_out[i] = mp_obj_new_tuple(3, entry);
    }

    self->png->close();

    return mp_obj_new_list(256, palette_out);
}

}
