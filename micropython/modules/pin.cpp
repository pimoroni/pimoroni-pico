#include "util.hpp"

extern "C" {
#include "py/runtime.h"
#include "machine_pin.h"
#include "pin.hpp"

extern int pimoroni_gpio_from_obj(const mp_obj_t &object) {
    int gpio;
    if (mp_obj_is_type(object, &machine_pin_type)) {
        machine_pin_obj_t *pin = MP_OBJ_TO_PTR2(object, machine_pin_obj_t);
        #if MICROPY_HW_PIN_EXT_COUNT
        if(pin->is_ext) {
            mp_raise_TypeError(MP_ERROR_TEXT("pin cannot be an external pin"));
        }
        #endif
        gpio = pin->id;
    }
    else if (mp_obj_is_int(object)) {
        gpio = mp_obj_get_int(object);
    }
    else {
        mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("pin must be of type %q or int, not %q"), machine_pin_type.name, mp_obj_get_type(object)->name);
    }
    return gpio;
}
}