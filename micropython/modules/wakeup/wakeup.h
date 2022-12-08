#include "py/runtime.h"
#include "py/objstr.h"

extern mp_obj_t Wakeup_get_gpio_state();
#if WAKEUP_HAS_SHIFT_REGISTER==1
extern mp_obj_t Wakeup_get_shift_state();
#endif