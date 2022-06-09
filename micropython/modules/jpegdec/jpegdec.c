#include "jpegdec.h"

STATIC MP_DEFINE_CONST_FUN_OBJ_1(JPEG_del_obj, _JPEG_del);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(JPEG_openRAM_obj, _JPEG_openRAM);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(JPEG_decode_obj, 1, _JPEG_decode);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(JPEG_getWidth_obj, _JPEG_getWidth);
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