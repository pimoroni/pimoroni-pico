#include "profiler.h"

STATIC MP_DEFINE_CONST_FUN_OBJ_0(profiler_get_probes_obj, profiler_get_probes);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(profiler_reset_obj, profiler_reset);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(profiler_get_probe_obj, profiler_get_probe);

STATIC const mp_map_elem_t profiler_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_profiler) },
    { MP_ROM_QSTR(MP_QSTR_get_probes), MP_ROM_PTR(&profiler_get_probes_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_probe), MP_ROM_PTR(&profiler_get_probe_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&profiler_reset_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_profiler_globals, profiler_globals_table);

const mp_obj_module_t profiler_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_profiler_globals,
};

#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_profiler, profiler_user_cmodule, MODULE_PROFILER_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_profiler, profiler_user_cmodule);
#endif