#include "duo75.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(Duo75___del___obj, Duo75___del__);
MP_DEFINE_CONST_FUN_OBJ_KW(Duo75_set_pixel_obj, 5, Duo75_set_pixel);
MP_DEFINE_CONST_FUN_OBJ_1(Duo75_clear_obj, Duo75_clear);
MP_DEFINE_CONST_FUN_OBJ_1(Duo75_start_obj, Duo75_start);
MP_DEFINE_CONST_FUN_OBJ_1(Duo75_stop_obj, Duo75_stop);
MP_DEFINE_CONST_FUN_OBJ_2(Duo75_update_obj, Duo75_update);


/***** Binding of Methods *****/
static const mp_rom_map_elem_t Duo75_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&Duo75___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pixel), MP_ROM_PTR(&Duo75_set_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&Duo75_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&Duo75_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&Duo75_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&Duo75_update_obj) },
};

static MP_DEFINE_CONST_DICT(Duo75_locals_dict, Duo75_locals_dict_table);

/***** Class Definition *****/
MP_DEFINE_CONST_OBJ_TYPE(
    Duo75_type,
    MP_QSTR_duo75,
    MP_TYPE_FLAG_NONE,
    make_new, Duo75_make_new,
    print, Duo75_print,
    locals_dict, (mp_obj_dict_t*)&Duo75_locals_dict
);

/***** Globals Table *****/

static const mp_map_elem_t duo75_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_duo75) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Duo75), (mp_obj_t)&Duo75_type },
};
static MP_DEFINE_CONST_DICT(mp_module_duo75_globals, duo75_globals_table);

/***** Module Definition *****/
const mp_obj_module_t duo75_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_duo75_globals,
};
MP_REGISTER_MODULE(MP_QSTR_duo75, duo75_user_cmodule);