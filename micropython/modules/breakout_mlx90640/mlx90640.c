#include "mlx90640.h"


MP_DEFINE_CONST_FUN_OBJ_2(MLX90640_setup_obj, MLX90640_setup);
MP_DEFINE_CONST_FUN_OBJ_1(MLX90640_get_frame_obj, MLX90640_get_frame);

STATIC const mp_rom_map_elem_t MLX90640_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_setup), MP_ROM_PTR(&MLX90640_setup_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_frame), MP_ROM_PTR(&MLX90640_get_frame_obj) },
};
STATIC MP_DEFINE_CONST_DICT(MLX90640_locals_dict, MLX90640_locals_dict_table);

#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    MLX90640_MLX90640_type,
    MP_QSTR_MLX90640,
    MP_TYPE_FLAG_NONE,
    make_new, MLX90640_make_new,
    locals_dict, (mp_obj_dict_t*)&MLX90640_locals_dict
);
#else
const mp_obj_type_t MLX90640_MLX90640_type = {
    { &mp_type_type },
    .name = MP_QSTR_MLX90640,
    .make_new = MLX90640_make_new,
    .locals_dict = (mp_obj_dict_t*)&MLX90640_locals_dict,
};
#endif

STATIC const mp_map_elem_t MLX90640_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_msa301) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MLX90640), (mp_obj_t)&MLX90640_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_MLX90640_globals, MLX90640_globals_table);

const mp_obj_module_t MLX90640_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_MLX90640_globals,
};

#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_MLX90640, MLX90640_user_cmodule, MODULE_MLX90640_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_MLX90640, MLX90640_user_cmodule);
#endif