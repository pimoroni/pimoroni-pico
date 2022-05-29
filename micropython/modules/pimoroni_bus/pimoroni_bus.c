#include "pimoroni_bus.h"

MP_DEFINE_CONST_FUN_OBJ_1(SPISlot_obj, SPISlot);

const mp_obj_type_t SPIPins_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPIBus,
    .print = PimoroniBus_print,
    .make_new = SPIPins_make_new
};

const mp_obj_type_t ParallelPins_type = {
    { &mp_type_type },
    .name = MP_QSTR_ParallelBus,
    .print = PimoroniBus_print,
    .make_new = ParallelPins_make_new
};

STATIC const mp_map_elem_t pimoroni_bus_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_pimoroni_bus) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SPIBus), (mp_obj_t)&SPIPins_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ParallelBus), (mp_obj_t)&ParallelPins_type },
    { MP_ROM_QSTR(MP_QSTR_SPISlot), MP_ROM_PTR(&SPISlot_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_pimoroni_bus_globals, pimoroni_bus_globals_table);

const mp_obj_module_t pimoroni_bus_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_pimoroni_bus_globals,
};

MP_REGISTER_MODULE(MP_QSTR_pimoroni_bus, pimoroni_bus_user_cmodule, MODULE_PIMORONI_BUS_ENABLED);