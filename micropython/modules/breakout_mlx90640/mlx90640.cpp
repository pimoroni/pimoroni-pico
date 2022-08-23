#include "drivers/mlx90640/mlx90640.hpp"

#include "micropython/modules/util.hpp"

using namespace pimoroni;

extern "C" {
#include "mlx90640.h"
#include "pimoroni_i2c.h"

typedef struct _ModMLX90640_obj_t {
    mp_obj_base_t base;
    MLX90640 *breakout;
    _PimoroniI2C_obj_t *i2c;
    int address;
} ModMLX90640_obj_t;

mp_obj_t MLX90640_setup(mp_obj_t self_in, mp_obj_t fps) {
    _ModMLX90640_obj_t *self = MP_OBJ_TO_PTR2(self_in, _ModMLX90640_obj_t);

    self->breakout->setup(mp_obj_get_int(fps));

    return mp_const_none;
}

mp_obj_t MLX90640_get_frame(mp_obj_t self_in) {
    _ModMLX90640_obj_t *self = MP_OBJ_TO_PTR2(self_in, _ModMLX90640_obj_t);

    self->breakout->get_frame();

    mp_obj_list_t *list = MP_OBJ_TO_PTR2(mp_obj_new_list(MLX90640::WIDTH * MLX90640::HEIGHT, NULL), mp_obj_list_t);

    for(auto y = 0u; y < MLX90640::HEIGHT; y++) {
        for(auto x = 0u; x < MLX90640::WIDTH; x++) {
            int offset = y * MLX90640::WIDTH + x;
            float v = self->breakout->mlx90640To[offset];
            list->items[offset] = mp_obj_new_float(v);
        }
    }

    return list;
}

mp_obj_t MLX90640_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_i2c, ARG_address };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_address, MP_ARG_INT, {.u_int = MLX90640_DEFAULT_I2C_ADDRESS}}
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _ModMLX90640_obj_t *self = m_new_obj_with_finaliser(_ModMLX90640_obj_t);
    self->base.type = &MLX90640_type;

    self->i2c = PimoroniI2C_from_machine_i2c_or_native(args[ARG_i2c].u_obj);
    self->address = args[ARG_address].u_int;

    self->breakout = m_new_class(MLX90640, (pimoroni::I2C *)(self->i2c->i2c), self->address);

    return self;
}

}