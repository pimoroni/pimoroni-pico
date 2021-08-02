#include "plasma_2040.h"


/***** Methods *****/
MP_DEFINE_CONST_FUN_OBJ_1(PlasmaAPA102___del___obj, PlasmaAPA102___del__);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaAPA102_set_rgb_obj, 5, PlasmaAPA102_set_rgb);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaAPA102_set_hsv_obj, 3, PlasmaAPA102_set_hsv);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaAPA102_set_brightness_obj, 2, PlasmaAPA102_set_brightness);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaAPA102_start_obj, 1, PlasmaAPA102_start);
MP_DEFINE_CONST_FUN_OBJ_1(PlasmaAPA102_clear_obj, PlasmaAPA102_clear);

MP_DEFINE_CONST_FUN_OBJ_1(PlasmaWS2812___del___obj, PlasmaWS2812___del__);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaWS2812_set_rgb_obj, 5, PlasmaWS2812_set_rgb);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaWS2812_set_hsv_obj, 3, PlasmaWS2812_set_hsv);
MP_DEFINE_CONST_FUN_OBJ_KW(PlasmaWS2812_start_obj, 1, PlasmaWS2812_start);
MP_DEFINE_CONST_FUN_OBJ_1(PlasmaWS2812_clear_obj, PlasmaWS2812_clear);

/***** Binding of Methods *****/
STATIC const mp_rom_map_elem_t PlasmaAPA102_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&PlasmaAPA102___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_set_rgb), MP_ROM_PTR(&PlasmaAPA102_set_rgb_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_hsv), MP_ROM_PTR(&PlasmaAPA102_set_hsv_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_brightness), MP_ROM_PTR(&PlasmaAPA102_set_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&PlasmaAPA102_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&PlasmaAPA102_clear_obj) },
};
STATIC const mp_rom_map_elem_t PlasmaWS2812_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&PlasmaWS2812___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_set_rgb), MP_ROM_PTR(&PlasmaWS2812_set_rgb_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_hsv), MP_ROM_PTR(&PlasmaWS2812_set_hsv_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&PlasmaWS2812_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&PlasmaWS2812_clear_obj) },
};

STATIC MP_DEFINE_CONST_DICT(PlasmaAPA102_locals_dict, PlasmaAPA102_locals_dict_table);
STATIC MP_DEFINE_CONST_DICT(PlasmaWS2812_locals_dict, PlasmaWS2812_locals_dict_table);

/***** Class Definition *****/
const mp_obj_type_t PlasmaAPA102_type = {
    { &mp_type_type },
    .name = MP_QSTR_plasma_apa102,
    .print = PlasmaAPA102_print,
    .make_new = PlasmaAPA102_make_new,
    .locals_dict = (mp_obj_dict_t*)&PlasmaAPA102_locals_dict,
};

const mp_obj_type_t PlasmaWS2812_type = {
    { &mp_type_type },
    .name = MP_QSTR_plasma_ws2812,
    .print = PlasmaWS2812_print,
    .make_new = PlasmaWS2812_make_new,
    .locals_dict = (mp_obj_dict_t*)&PlasmaWS2812_locals_dict,
};

/***** Globals Table *****/
STATIC const mp_map_elem_t plasma_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_plasma) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_APA102), (mp_obj_t)&PlasmaAPA102_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WS2812), (mp_obj_t)&PlasmaWS2812_type },
    { MP_ROM_QSTR(MP_QSTR_PIN_LED_R), MP_ROM_INT(16) },
    { MP_ROM_QSTR(MP_QSTR_PIN_LED_G), MP_ROM_INT(17) },
    { MP_ROM_QSTR(MP_QSTR_PIN_LED_B), MP_ROM_INT(18) },
    { MP_ROM_QSTR(MP_QSTR_PIN_BUTTON_A), MP_ROM_INT(12) },
    { MP_ROM_QSTR(MP_QSTR_PIN_BUTTON_B), MP_ROM_INT(13) },

    { MP_ROM_QSTR(MP_QSTR_COLOR_ORDER_RGB), MP_ROM_INT(0x00) },
    { MP_ROM_QSTR(MP_QSTR_COLOR_ORDER_RBG), MP_ROM_INT(0x01) },
    { MP_ROM_QSTR(MP_QSTR_COLOR_ORDER_GRB), MP_ROM_INT(0x02) },
    { MP_ROM_QSTR(MP_QSTR_COLOR_ORDER_GBR), MP_ROM_INT(0x03) },
    { MP_ROM_QSTR(MP_QSTR_COLOR_ORDER_BRG), MP_ROM_INT(0x04) },
    { MP_ROM_QSTR(MP_QSTR_COLOR_ORDER_BGR), MP_ROM_INT(0x05) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_plasma_globals, plasma_globals_table);

/***** Module Definition *****/
const mp_obj_module_t plasma_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_plasma_globals,
};
MP_REGISTER_MODULE(MP_QSTR_plasma, plasma_user_cmodule, MODULE_PLASMA_ENABLED);