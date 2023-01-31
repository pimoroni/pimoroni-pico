#include "adcfft.h"

// Class
MP_DEFINE_CONST_FUN_OBJ_1(adcfft_update_obj, adcfft_update);
MP_DEFINE_CONST_FUN_OBJ_3(adcfft_get_scaled_obj, adcfft_get_scaled);
MP_DEFINE_CONST_FUN_OBJ_1(adcfft__del__obj, adcfft__del__);

STATIC const mp_rom_map_elem_t adcfft_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&adcfft__del__obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&adcfft_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_scaled), MP_ROM_PTR(&adcfft_get_scaled_obj) },
};

STATIC MP_DEFINE_CONST_DICT(adcfft_locals_dict, adcfft_locals_dict_table);

#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    adcfft_type,
    MP_QSTR_ADCFFT,
    MP_TYPE_FLAG_NONE,
    make_new, adcfft_make_new,
    locals_dict, (mp_obj_dict_t*)&adcfft_locals_dict
);
#else
const mp_obj_type_t adcfft_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADCFFT,
    .make_new = adcfft_make_new,
    .locals_dict = (mp_obj_dict_t*)&adcfft_locals_dict,
};
#endif

// Module
STATIC const mp_map_elem_t adcfft_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_adcfft) },  // Module name
    { MP_OBJ_NEW_QSTR(MP_QSTR_ADCFFT), (mp_obj_t)&adcfft_type },             // Class name & type
};
STATIC MP_DEFINE_CONST_DICT(mp_module_adcfft_globals, adcfft_globals_table);


const mp_obj_module_t adcfft_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_adcfft_globals,
};

#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_adcfft, adcfft_user_cmodule, MODULE_ADCFFT_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_adcfft, adcfft_user_cmodule);
#endif