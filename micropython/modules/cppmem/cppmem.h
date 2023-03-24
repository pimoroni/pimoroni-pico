#include "py/runtime.h"
#include "py/objstr.h"

extern mp_obj_t cpp_mem_get_alloc_bytes();
extern mp_obj_t cpp_mem_get_alloc_count();
extern mp_obj_t cpp_mem_get_free_count();
extern mp_obj_t cpp_mem_set_allocator_mode(mp_obj_t mode);
extern mp_obj_t cpp_mem_get_allocator_mode();