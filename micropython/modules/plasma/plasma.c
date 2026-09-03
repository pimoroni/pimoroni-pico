#include "plasma.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(PlasmaAPA102___del___obj, PlasmaAPA102___del__);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaAPA102_set_rgb_obj, 5, PlasmaAPA102_set_rgb);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaAPA102_set_hsv_obj, 3, PlasmaAPA102_set_hsv);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaAPA102_set_brightness_obj, 2, PlasmaAPA102_set_brightness);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaAPA102_start_obj, 1, PlasmaAPA102_start);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaAPA102_get_obj, 2, PlasmaAPA102_get);
MP_DEFINE_CONST_FUN_OBJ_1(PlasmaAPA102_clear_obj, PlasmaAPA102_clear);
MP_DEFINE_CONST_FUN_OBJ_1(PlasmaAPA102_update_obj, PlasmaAPA102_update);
MP_DEFINE_CONST_FUN_OBJ_2(PlasmaAPA102_set_blocking_obj, PlasmaAPA102_set_blocking);
MP_DEFINE_CONST_FUN_OBJ_1(PlasmaAPA102_is_busy_obj, PlasmaAPA102_is_busy);
MP_DEFINE_CONST_FUN_OBJ_1(PlasmaAPA102_num_leds_obj, PlasmaAPA102_num_leds);

MP_DEFINE_CONST_FUN_OBJ_1(PlasmaWS2812___del___obj, PlasmaWS2812___del__);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaWS2812_set_rgb_obj, 5, PlasmaWS2812_set_rgb);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaWS2812_set_hsv_obj, 3, PlasmaWS2812_set_hsv);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaWS2812_start_obj, 1, PlasmaWS2812_start);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaWS2812_get_obj, 2, PlasmaWS2812_get);
MP_DEFINE_CONST_FUN_OBJ_1(PlasmaWS2812_clear_obj, PlasmaWS2812_clear);
MP_DEFINE_CONST_FUN_OBJ_1(PlasmaWS2812_update_obj, PlasmaWS2812_update);
MP_DEFINE_CONST_FUN_OBJ_2(PlasmaWS2812_set_blocking_obj, PlasmaWS2812_set_blocking);
MP_DEFINE_CONST_FUN_OBJ_1(PlasmaWS2812_is_busy_obj, PlasmaWS2812_is_busy);
MP_DEFINE_CONST_FUN_OBJ_1(PlasmaWS2812_num_leds_obj, PlasmaWS2812_num_leds);

/***** Binding of Methods *****/
static const mp_rom_map_elem_t PlasmaAPA102_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&PlasmaAPA102___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_set_rgb), MP_ROM_PTR(&PlasmaAPA102_set_rgb_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_hsv), MP_ROM_PTR(&PlasmaAPA102_set_hsv_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_brightness), MP_ROM_PTR(&PlasmaAPA102_set_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&PlasmaAPA102_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_get), MP_ROM_PTR(&PlasmaAPA102_get_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&PlasmaAPA102_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&PlasmaAPA102_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_blocking), MP_ROM_PTR(&PlasmaAPA102_set_blocking_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_busy), MP_ROM_PTR(&PlasmaAPA102_is_busy_obj) },
    { MP_ROM_QSTR(MP_QSTR_num_leds), MP_ROM_PTR(&PlasmaAPA102_num_leds_obj) },
};
static const mp_rom_map_elem_t PlasmaWS2812_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&PlasmaWS2812___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_set_rgb), MP_ROM_PTR(&PlasmaWS2812_set_rgb_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_hsv), MP_ROM_PTR(&PlasmaWS2812_set_hsv_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&PlasmaWS2812_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_get), MP_ROM_PTR(&PlasmaWS2812_get_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&PlasmaWS2812_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&PlasmaWS2812_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_blocking), MP_ROM_PTR(&PlasmaWS2812_set_blocking_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_busy), MP_ROM_PTR(&PlasmaWS2812_is_busy_obj) },
    { MP_ROM_QSTR(MP_QSTR_num_leds), MP_ROM_PTR(&PlasmaWS2812_num_leds_obj) },
};

static MP_DEFINE_CONST_DICT(PlasmaAPA102_locals_dict, PlasmaAPA102_locals_dict_table);
static MP_DEFINE_CONST_DICT(PlasmaWS2812_locals_dict, PlasmaWS2812_locals_dict_table);

/***** Class Definition *****/
#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    PlasmaAPA102_type,
    MP_QSTR_plasma_apa102,
    MP_TYPE_FLAG_NONE,
    make_new, PlasmaAPA102_make_new,
    print, PlasmaAPA102_print,
    locals_dict, (mp_obj_dict_t*)&PlasmaAPA102_locals_dict
);
#else
const mp_obj_type_t PlasmaAPA102_type = {
    { &mp_type_type },
    .name = MP_QSTR_plasma_apa102,
    .print = PlasmaAPA102_print,
    .make_new = PlasmaAPA102_make_new,
    .locals_dict = (mp_obj_dict_t*)&PlasmaAPA102_locals_dict,
};
#endif

#ifdef MP_DEFINE_CONST_OBJ_TYPE
MP_DEFINE_CONST_OBJ_TYPE(
    PlasmaWS2812_type,
    MP_QSTR_plasma_ws2812,
    MP_TYPE_FLAG_NONE,
    make_new, PlasmaWS2812_make_new,
    print, PlasmaWS2812_print,
    locals_dict, (mp_obj_dict_t*)&PlasmaWS2812_locals_dict
);
#else
const mp_obj_type_t PlasmaWS2812_type = {
    { &mp_type_type },
    .name = MP_QSTR_plasma_ws2812,
    .print = PlasmaWS2812_print,
    .make_new = PlasmaWS2812_make_new,
    .locals_dict = (mp_obj_dict_t*)&PlasmaWS2812_locals_dict,
};
#endif

typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;
mp_obj_float_t shunt_resistor = {{&mp_type_float}, 0.015f};

/***** Globals Table *****/
static const mp_map_elem_t plasma_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_plasma) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_APA102), (mp_obj_t)&PlasmaAPA102_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WS2812), (mp_obj_t)&PlasmaWS2812_type },

    { MP_ROM_QSTR(MP_QSTR_COLOR_ORDER_RGB), MP_ROM_INT(0x00) },
    { MP_ROM_QSTR(MP_QSTR_COLOR_ORDER_RBG), MP_ROM_INT(0x01) },
    { MP_ROM_QSTR(MP_QSTR_COLOR_ORDER_GRB), MP_ROM_INT(0x02) },
    { MP_ROM_QSTR(MP_QSTR_COLOR_ORDER_GBR), MP_ROM_INT(0x03) },
    { MP_ROM_QSTR(MP_QSTR_COLOR_ORDER_BRG), MP_ROM_INT(0x04) },
    { MP_ROM_QSTR(MP_QSTR_COLOR_ORDER_BGR), MP_ROM_INT(0x05) },
};
static MP_DEFINE_CONST_DICT(mp_module_plasma_globals, plasma_globals_table);

/***** Module Definition *****/
const mp_obj_module_t plasma_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_plasma_globals,
};
#if MICROPY_VERSION <= 70144
MP_REGISTER_MODULE(MP_QSTR_plasma, plasma_user_cmodule, MODULE_PLASMA_ENABLED);
#else
MP_REGISTER_MODULE(MP_QSTR_plasma, plasma_user_cmodule);
#endif
