// Include MicroPython API.
#include "py/runtime.h"
#include "extmod/modmachine.h"
#include "hardware/i2c.h"

/***** Extern of Class Definition *****/
extern const mp_obj_type_t PimoroniI2C_type;

typedef struct _PimoroniI2C_obj_t {
    mp_obj_base_t base;
    void *i2c;
} _PimoroniI2C_obj_t;

/***** Extern of Class Methods *****/
extern void PimoroniI2C_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t PimoroniI2C_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t PimoroniI2C___del__(mp_obj_t self_in);

extern bool Pimoroni_mp_obj_to_i2c(mp_obj_t in, void *out);

extern const mp_obj_type_t machine_i2c_type;

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    i2c_inst_t *const i2c_inst;
    uint8_t i2c_id;
    uint8_t scl;
    uint8_t sda;
    uint32_t freq;
} machine_i2c_obj_t;

extern int machine_i2c_transfer_single(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags);

_PimoroniI2C_obj_t* PimoroniI2C_from_machine_i2c_or_native(mp_obj_t i2c_obj);