#include "py/runtime.h"
#include "py/objstr.h"

extern mp_obj_t Wakeup_get_gpio_state();
extern mp_obj_t Wakeup_get_wake_reason();
extern mp_obj_t Wakeup_set_wake_reason(mp_obj_t value);