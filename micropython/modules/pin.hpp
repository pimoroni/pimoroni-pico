extern "C" {
#include "py/obj.h"

extern int pimoroni_gpio_from_obj(const mp_obj_t &object);
extern int pimoroni_obj_is_int_or_gpio(const mp_obj_t &object);
}