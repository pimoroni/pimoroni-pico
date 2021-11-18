#include "hub75.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(Hub75___del___obj, Hub75___del__);
MP_DEFINE_CONST_FUN_OBJ_KW(Hub75_set_rgb_obj, 5, Hub75_set_rgb);
MP_DEFINE_CONST_FUN_OBJ_1(Hub75_clear_obj, Hub75_clear);
MP_DEFINE_CONST_FUN_OBJ_1(Hub75_start_obj, Hub75_start);
MP_DEFINE_CONST_FUN_OBJ_1(Hub75_stop_obj, Hub75_stop);
MP_DEFINE_CONST_FUN_OBJ_1(Hub75_flip_obj, Hub75_flip);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t Hub75_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&Hub75___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_set_rgb), MP_ROM_PTR(&Hub75_set_rgb_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&Hub75_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&Hub75_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&Hub75_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_flip), MP_ROM_PTR(&Hub75_flip_obj) },
};

STATIC MP_DEFINE_CONST_DICT(Hub75_locals_dict, Hub75_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t Hub75_type = {
    { &mp_type_type },
    .name = MP_QSTR_hub75,
    .print = Hub75_print,
    .make_new = Hub75_make_new,
    .locals_dict = (mp_obj_dict_t*)&Hub75_locals_dict,
};

/***** Globals Table *****/

STATIC const mp_map_elem_t hub75_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_hub75) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Hub75), (mp_obj_t)&Hub75_type },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_hub75_globals, hub75_globals_table);

/***** Module Definition *****/
const mp_obj_module_t hub75_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_hub75_globals,
};
MP_REGISTER_MODULE(MP_QSTR_hub75, hub75_user_cmodule, MODULE_HUB75_ENABLED);