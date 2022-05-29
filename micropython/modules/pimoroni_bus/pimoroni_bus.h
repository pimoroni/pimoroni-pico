#include "py/runtime.h"
#include "hardware/spi.h"

extern const mp_obj_type_t SPIPins_type;
extern const mp_obj_type_t ParallelPins_type;

typedef struct _PimoroniBus_obj_t  {
    mp_obj_base_t base;
    void *pins;
} _PimoroniBus_obj_t;

extern void PimoroniBus_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t SPIPins_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t ParallelPins_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

extern mp_obj_t SPISlot(mp_obj_t self_in);