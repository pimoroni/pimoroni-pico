#include "picovector.h"

/* Polygon */

static MP_DEFINE_CONST_FUN_OBJ_1(POLYGON__del__obj, POLYGON__del__);

// Transformations
//static MP_DEFINE_CONST_FUN_OBJ_KW(POLYGON_rotate_obj, 3, POLYGON_rotate);
//static MP_DEFINE_CONST_FUN_OBJ_KW(POLYGON_translate_obj, 4, POLYGON_translate);

// Utility functions
static MP_DEFINE_CONST_FUN_OBJ_1(POLYGON_centroid_obj, POLYGON_centroid);
static MP_DEFINE_CONST_FUN_OBJ_1(POLYGON_bounds_obj, POLYGON_bounds);
static MP_DEFINE_CONST_FUN_OBJ_2(POLYGON_transform_obj, POLYGON_transform);

// Primitives
static MP_DEFINE_CONST_FUN_OBJ_KW(POLYGON_rectangle_obj, 5, POLYGON_rectangle);
static MP_DEFINE_CONST_FUN_OBJ_VAR(POLYGON_path_obj, 4, POLYGON_path);
static MP_DEFINE_CONST_FUN_OBJ_KW(POLYGON_regular_obj, 5, POLYGON_regular);
static MP_DEFINE_CONST_FUN_OBJ_KW(POLYGON_circle_obj, 4, POLYGON_circle);
static MP_DEFINE_CONST_FUN_OBJ_KW(POLYGON_arc_obj, 6, POLYGON_arc);


static const mp_rom_map_elem_t POLYGON_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&POLYGON__del__obj) },

    // Transformations
    //{ MP_ROM_QSTR(MP_QSTR_rotate), MP_ROM_PTR(&POLYGON_rotate_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_translate), MP_ROM_PTR(&POLYGON_translate_obj) },

    // Utility functions
    { MP_ROM_QSTR(MP_QSTR_centroid), MP_ROM_PTR(&POLYGON_centroid_obj) },
    { MP_ROM_QSTR(MP_QSTR_bounds), MP_ROM_PTR(&POLYGON_bounds_obj) },
    { MP_ROM_QSTR(MP_QSTR_transform), MP_ROM_PTR(&POLYGON_transform_obj) },

    // Primitives
    { MP_ROM_QSTR(MP_QSTR_rectangle), MP_ROM_PTR(&POLYGON_rectangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_regular), MP_ROM_PTR(&POLYGON_regular_obj) },
    { MP_ROM_QSTR(MP_QSTR_path), MP_ROM_PTR(&POLYGON_path_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&POLYGON_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_arc), MP_ROM_PTR(&POLYGON_arc_obj) },
};

static MP_DEFINE_CONST_DICT(POLYGON_locals_dict, POLYGON_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    POLYGON_type,
    MP_QSTR_polygon,
    MP_TYPE_FLAG_NONE,
    make_new, POLYGON_make_new,
    print, POLYGON_print,
    iter, POLYGON_getiter,
    locals_dict, (mp_obj_dict_t*)&POLYGON_locals_dict
);

/* Transform */

static MP_DEFINE_CONST_FUN_OBJ_3(TRANSFORM_rotate_obj, TRANSFORM_rotate);
static MP_DEFINE_CONST_FUN_OBJ_3(TRANSFORM_translate_obj, TRANSFORM_translate);
static MP_DEFINE_CONST_FUN_OBJ_1(TRANSFORM_reset_obj, TRANSFORM_reset);

static const mp_rom_map_elem_t TRANSFORM_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_rotate), MP_ROM_PTR(&TRANSFORM_rotate_obj) },
    { MP_ROM_QSTR(MP_QSTR_translate), MP_ROM_PTR(&TRANSFORM_translate_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&TRANSFORM_reset_obj) },
};

static MP_DEFINE_CONST_DICT(TRANSFORM_locals_dict, TRANSFORM_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    TRANSFORM_type,
    MP_QSTR_Transform,
    MP_TYPE_FLAG_NONE,
    make_new, TRANSFORM_make_new,
    locals_dict, (mp_obj_dict_t*)&TRANSFORM_locals_dict
);

/* PicoVector */

static MP_DEFINE_CONST_FUN_OBJ_KW(VECTOR_text_obj, 4, VECTOR_text);
static MP_DEFINE_CONST_FUN_OBJ_3(VECTOR_set_font_obj, VECTOR_set_font);
static MP_DEFINE_CONST_FUN_OBJ_2(VECTOR_set_font_size_obj, VECTOR_set_font_size);
static MP_DEFINE_CONST_FUN_OBJ_2(VECTOR_set_antialiasing_obj, VECTOR_set_antialiasing);
static MP_DEFINE_CONST_FUN_OBJ_2(VECTOR_set_transform_obj, VECTOR_set_transform);
static MP_DEFINE_CONST_FUN_OBJ_2(VECTOR_set_clip_obj, VECTOR_set_clip);

static MP_DEFINE_CONST_FUN_OBJ_2(VECTOR_draw_obj, VECTOR_draw);

static const mp_rom_map_elem_t VECTOR_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_font), MP_ROM_PTR(&VECTOR_set_font_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_font_size), MP_ROM_PTR(&VECTOR_set_font_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_antialiasing), MP_ROM_PTR(&VECTOR_set_antialiasing_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_transform), MP_ROM_PTR(&VECTOR_set_transform_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_clip), MP_ROM_PTR(&VECTOR_set_clip_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&VECTOR_text_obj) },

    { MP_ROM_QSTR(MP_QSTR_draw), MP_ROM_PTR(&VECTOR_draw_obj) },
};

static MP_DEFINE_CONST_DICT(VECTOR_locals_dict, VECTOR_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    VECTOR_type,
    MP_QSTR_picovector,
    MP_TYPE_FLAG_NONE,
    make_new, VECTOR_make_new,
    locals_dict, (mp_obj_dict_t*)&VECTOR_locals_dict
);

/* Module */

static const mp_map_elem_t VECTOR_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_picovector) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PicoVector), (mp_obj_t)&VECTOR_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Polygon), (mp_obj_t)&POLYGON_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Transform), (mp_obj_t)&TRANSFORM_type },
    { MP_ROM_QSTR(MP_QSTR_ANTIALIAS_NONE), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_ANTIALIAS_X4), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_ANTIALIAS_X16), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_ANTIALIAS_FAST), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_ANTIALIAS_BEST), MP_ROM_INT(2) },
};

static MP_DEFINE_CONST_DICT(mp_module_VECTOR_globals, VECTOR_globals_table);

const mp_obj_module_t VECTOR_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_VECTOR_globals,
};

MP_REGISTER_MODULE(MP_QSTR_picovector, VECTOR_user_cmodule);