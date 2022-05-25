#include "py/runtime.h"
#include "py/objstr.h"
#include "JPEGDEC.h"

const mp_obj_type_t JPEG_type;

typedef struct _JPEG_obj_t {
    mp_obj_base_t base;
    JPEGIMAGE *jpeg;
    mp_obj_t callback;
} _JPEG_obj_t;

mp_obj_t current_callback = mp_const_none;

int JPEGDraw(JPEGDRAW *pDraw) {
    for(int y = 0; y < pDraw->iHeight; y++) {
        for(int x = 0; x < pDraw->iWidth; x++) {
            int i = y * pDraw->iWidth + x;
            mp_obj_t args[] = {
                mp_obj_new_int(pDraw->x + x),
                mp_obj_new_int(pDraw->y + y),
                mp_obj_new_int(pDraw->pPixels[i])
            };
            mp_call_function_n_kw(current_callback, MP_ARRAY_SIZE(args), 0, args);
        }
    }
    return 1;
}

STATIC mp_obj_t _JPEG_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum {
        ARG_callback
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_callback, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _JPEG_obj_t *self = m_new_obj_with_finaliser(_JPEG_obj_t);
    self->base.type = &JPEG_type;
    self->jpeg = m_new(JPEGIMAGE, 1);
    self->callback = args[ARG_callback].u_obj;
    return self;
}

STATIC mp_obj_t _JPEG_del(mp_obj_t self_in) {
    _JPEG_obj_t *self = MP_OBJ_TO_PTR(self_in);
    m_del(JPEGIMAGE, self->jpeg, 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(JPEG_del_obj, _JPEG_del);

// open_RAM
STATIC mp_obj_t _JPEG_openRAM(mp_obj_t self_in, mp_obj_t buffer) {
    _JPEG_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer, &bufinfo, MP_BUFFER_READ);
    int result = JPEG_openRAM(self->jpeg, bufinfo.buf, bufinfo.len, JPEGDraw);
    JPEG_setPixelType(self->jpeg, RGB565_BIG_ENDIAN);
    return result == 1 ? mp_const_true : mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(JPEG_openRAM_obj, _JPEG_openRAM);

// decode
STATIC mp_obj_t _JPEG_decode(mp_obj_t self_in, mp_obj_t flags) {
    _JPEG_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int x = 0;
    int y = 0;
    int f = mp_obj_get_int(flags);
    current_callback = self->callback;
    return JPEG_decode(self->jpeg, x, y, f) == 1 ? mp_const_true : mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(JPEG_decode_obj, _JPEG_decode);

// get_width
STATIC mp_obj_t _JPEG_getWidth(mp_obj_t self_in) {
    _JPEG_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(JPEG_getWidth(self->jpeg));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(JPEG_getWidth_obj, _JPEG_getWidth);

// get_height
STATIC mp_obj_t _JPEG_getHeight(mp_obj_t self_in) {
    _JPEG_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(JPEG_getHeight(self->jpeg));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(JPEG_getHeight_obj, _JPEG_getHeight);

// class
STATIC const mp_rom_map_elem_t JPEG_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&JPEG_del_obj) },
    { MP_ROM_QSTR(MP_QSTR_open_RAM), MP_ROM_PTR(&JPEG_openRAM_obj) },
    { MP_ROM_QSTR(MP_QSTR_decode), MP_ROM_PTR(&JPEG_decode_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_width), MP_ROM_PTR(&JPEG_getWidth_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&JPEG_getHeight_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&JPEG_getHeight_obj) },
};

STATIC MP_DEFINE_CONST_DICT(JPEG_locals_dict, JPEG_locals_dict_table);

const mp_obj_type_t JPEG_type = {
    { &mp_type_type },
    .name = MP_QSTR_jpegdec,
    //.print = _JPEG_print,
    .make_new = _JPEG_make_new,
    .locals_dict = (mp_obj_dict_t*)&JPEG_locals_dict,
};

// module
STATIC const mp_map_elem_t JPEG_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_jpegdec) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_JPEG), (mp_obj_t)&JPEG_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_JPEG_globals, JPEG_globals_table);

const mp_obj_module_t JPEG_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_JPEG_globals,
};
MP_REGISTER_MODULE(MP_QSTR_jpegdec, JPEG_user_cmodule, MODULE_JPEGDEC_ENABLED);