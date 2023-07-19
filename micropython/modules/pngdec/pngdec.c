#include "pngdec.h"

STATIC MP_DEFINE_CONST_FUN_OBJ_1(PNG_del_obj, _PNG_del);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(PNG_openRAM_obj, _PNG_openRAM);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(PNG_openFILE_obj, _PNG_openFILE);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(PNG_decode_obj, 1, _PNG_decode);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(PNG_getWidth_obj, _PNG_getWidth);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(PNG_getHeight_obj, _PNG_getHeight);

// class
STATIC const mp_rom_map_elem_t PNG_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&PNG_del_obj) },
    { MP_ROM_QSTR(MP_QSTR_open_RAM), MP_ROM_PTR(&PNG_openRAM_obj) },
    { MP_ROM_QSTR(MP_QSTR_open_file), MP_ROM_PTR(&PNG_openFILE_obj) },
    { MP_ROM_QSTR(MP_QSTR_decode), MP_ROM_PTR(&PNG_decode_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_width), MP_ROM_PTR(&PNG_getWidth_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&PNG_getHeight_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&PNG_getHeight_obj) },
};

STATIC MP_DEFINE_CONST_DICT(PNG_locals_dict, PNG_locals_dict_table);

#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    PNG_type,
    MP_QSTR_pngdec,
    MP_TYPE_FLAG_NONE,
    make_new, _PNG_make_new,
    //print, _PNG_print,
    locals_dict, (mp_obj_dict_t*)&PNG_locals_dict
);
#else
const mp_obj_type_t PNG_type = {
    { &mp_type_type },
    .name = MP_QSTR_pngdec,
    //.print = _PNG_print,
    .make_new = _PNG_make_new,
    .locals_dict = (mp_obj_dict_t*)&PNG_locals_dict,
};
#endif

// module
STATIC const mp_map_elem_t PNG_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_pngdec) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PNG), (mp_obj_t)&PNG_type },

    { MP_ROM_QSTR(MP_QSTR_PNG_SCALE_FULL), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_PNG_SCALE_HALF), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_PNG_SCALE_QUARTER), MP_ROM_INT(4) },
    { MP_ROM_QSTR(MP_QSTR_PNG_SCALE_EIGHTH), MP_ROM_INT(8) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_PNG_globals, PNG_globals_table);

const mp_obj_module_t PNG_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_PNG_globals,
};

#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_pngdec, PNG_user_cmodule, MODULE_PNGDEC_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_pngdec, PNG_user_cmodule);
#endif