#include "cppmem.h"

STATIC MP_DEFINE_CONST_FUN_OBJ_0(cpp_mem_get_alloc_bytes_obj, cpp_mem_get_alloc_bytes);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(cpp_mem_get_alloc_count_obj, cpp_mem_get_alloc_count);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(cpp_mem_get_free_count_obj, cpp_mem_get_free_count);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cpp_mem_set_allocator_mode_obj, cpp_mem_set_allocator_mode);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(cpp_mem_get_allocator_mode_obj, cpp_mem_get_allocator_mode);

STATIC const mp_map_elem_t cppmem_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_cppmem) },
    { MP_ROM_QSTR(MP_QSTR_alloc_bytes), MP_ROM_PTR(&cpp_mem_get_alloc_bytes_obj) },
    { MP_ROM_QSTR(MP_QSTR_alloc_count), MP_ROM_PTR(&cpp_mem_get_alloc_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_free_count), MP_ROM_PTR(&cpp_mem_get_free_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_mode), MP_ROM_PTR(&cpp_mem_set_allocator_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_mode), MP_ROM_PTR(&cpp_mem_get_allocator_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_FIXED_HEAP), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_MICROPYTHON), MP_ROM_INT(1) }
};
STATIC MP_DEFINE_CONST_DICT(mp_module_cppmem_globals, cppmem_globals_table);

const mp_obj_module_t cppmem_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_cppmem_globals,
};

#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_cppmem, cppmem_user_cmodule, MODULE_CPPMEM_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_cppmem, cppmem_user_cmodule);
#endif