#include "picovector.h"

/* Polygon */

STATIC MP_DEFINE_CONST_FUN_OBJ_1(POLYGON__del__obj, POLYGON__del__);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(POLYGON_centroid_obj, POLYGON_centroid);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(POLYGON_bounds_obj, POLYGON_bounds);


STATIC const mp_rom_map_elem_t POLYGON_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&POLYGON__del__obj) },
    { MP_ROM_QSTR(MP_QSTR_centroid), MP_ROM_PTR(&POLYGON_centroid_obj) },
    { MP_ROM_QSTR(MP_QSTR_bounds), MP_ROM_PTR(&POLYGON_bounds_obj) },
};

STATIC MP_DEFINE_CONST_DICT(POLYGON_locals_dict, POLYGON_locals_dict_table);

#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    POLYGON_type,
    MP_QSTR_polygon,
    MP_TYPE_FLAG_NONE,
    make_new, POLYGON_make_new,
    print, POLYGON_print,
    iter, POLYGON_getiter,
    locals_dict, (mp_obj_dict_t*)&POLYGON_locals_dict
);
MP_DEFINE_CONST_OBJ_TYPE(
    REGULAR_POLYGON_type,
    MP_QSTR_regular_polygon,
    MP_TYPE_FLAG_NONE,
    make_new, REGULAR_POLYGON_make_new,
    locals_dict, (mp_obj_dict_t*)&POLYGON_locals_dict
);
MP_DEFINE_CONST_OBJ_TYPE(
    RECTANGLE_type,
    MP_QSTR_rectangle,
    MP_TYPE_FLAG_NONE,
    make_new, RECTANGLE_make_new,
    locals_dict, (mp_obj_dict_t*)&POLYGON_locals_dict
);
#else
const mp_obj_type_t POLYGON_type = {
    { &mp_type_type },
    .name = MP_QSTR_polygon,
    .make_new = POLYGON_make_new,
    .print = POLYGON_print,
    .iter = POLYGON_getiter,
    .locals_dict = (mp_obj_dict_t*)&POLYGON_locals_dict,
};
const mp_obj_type_t REGULAR_POLYGON_type = {
    { &mp_type_type },
    .name = MP_QSTR_regular_polygon,
    .make_new = REGULAR_POLYGON_make_new,
    .locals_dict = (mp_obj_dict_t*)&POLYGON_locals_dict,
};
const mp_obj_type_t RECTANGLE_type = {
    { &mp_type_type },
    .name = MP_QSTR_rectangle,
    .make_new = RECTANGLE_make_new,
    .locals_dict = (mp_obj_dict_t*)&POLYGON_locals_dict,
};
#endif

/* PicoVector */

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(VECTOR_text_obj, 4, VECTOR_text);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(VECTOR_set_font_obj, VECTOR_set_font);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(VECTOR_set_font_size_obj, VECTOR_set_font_size);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(VECTOR_set_antialiasing_obj, VECTOR_set_antialiasing);

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(VECTOR_draw_obj, 2, VECTOR_draw);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(VECTOR_rotate_obj, 3, VECTOR_rotate);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(VECTOR_translate_obj, 4, VECTOR_translate);

STATIC const mp_rom_map_elem_t VECTOR_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_font), MP_ROM_PTR(&VECTOR_set_font_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_font_size), MP_ROM_PTR(&VECTOR_set_font_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_antialiasing), MP_ROM_PTR(&VECTOR_set_antialiasing_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&VECTOR_text_obj) },

    { MP_ROM_QSTR(MP_QSTR_draw), MP_ROM_PTR(&VECTOR_draw_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotate), MP_ROM_PTR(&VECTOR_rotate_obj) },
    { MP_ROM_QSTR(MP_QSTR_translate), MP_ROM_PTR(&VECTOR_translate_obj) },
};

STATIC MP_DEFINE_CONST_DICT(VECTOR_locals_dict, VECTOR_locals_dict_table);

#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    VECTOR_type,
    MP_QSTR_picovector,
    MP_TYPE_FLAG_NONE,
    make_new, VECTOR_make_new,
    locals_dict, (mp_obj_dict_t*)&VECTOR_locals_dict
);
#else
const mp_obj_type_t VECTOR_type = {
    { &mp_type_type },
    .name = MP_QSTR_picovector,
    .make_new = VECTOR_make_new,
    .locals_dict = (mp_obj_dict_t*)&VECTOR_locals_dict,
};
#endif

/* Module */

STATIC const mp_map_elem_t VECTOR_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_picovector) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PicoVector), (mp_obj_t)&VECTOR_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Polygon), (mp_obj_t)&POLYGON_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RegularPolygon), (mp_obj_t)&REGULAR_POLYGON_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Rectangle), (mp_obj_t)&RECTANGLE_type },
    { MP_ROM_QSTR(MP_QSTR_ANTIALIAS_NONE), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_ANTIALIAS_X4), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_ANTIALIAS_X16), MP_ROM_INT(2) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_VECTOR_globals, VECTOR_globals_table);

const mp_obj_module_t VECTOR_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_VECTOR_globals,
};

#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_picovector, VECTOR_user_cmodule, MODULE_PICOVECTOR_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_picovector, VECTOR_user_cmodule);
#endif