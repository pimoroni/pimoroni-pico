#include "py/runtime.h"
#include "py/objstr.h"

extern mp_obj_t profiler_get_probes();
extern mp_obj_t profiler_get_probe(mp_obj_t name);
extern mp_obj_t profiler_reset();