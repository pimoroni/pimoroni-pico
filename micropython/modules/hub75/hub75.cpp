#include <cstdio>
#include "hub75.hpp"
#include "pico/multicore.h"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))


extern "C" {
#include "hub75.h"
#include "py/builtin.h"
#include "py/mpthread.h"

typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;

const mp_obj_float_t const_float_1 = {{&mp_type_float}, 1.0f};

/********** WS2812 **********/

/***** Variables Struct *****/
typedef struct _Hub75_obj_t {
    mp_obj_base_t base;
    Hub75* hub75;
    void *buf;
} _Hub75_obj_t;

_Hub75_obj_t *hub75_obj;


void __isr dma_complete() {
    if(hub75_obj) hub75_obj->hub75->dma_complete();
}

/***** Print *****/
void Hub75_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; // Unused input parameter
    _Hub75_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Hub75_obj_t);
    mp_print_str(print, "Hub75( ");

    mp_print_str(print, "dimensions = ");
    mp_obj_print_helper(print, mp_obj_new_int(self->hub75->width), PRINT_REPR);
    mp_print_str(print, " x ");
    mp_obj_print_helper(print, mp_obj_new_int(self->hub75->height), PRINT_REPR);

    mp_print_str(print, ", addr = front: ");
    mp_obj_print_helper(print, mp_obj_new_int((uint32_t)&self->hub75->front_buffer[0]), PRINT_REPR);
    mp_print_str(print, " back: ");
    mp_obj_print_helper(print, mp_obj_new_int((uint32_t)&self->hub75->back_buffer[0]), PRINT_REPR);

    switch(self->hub75->panel_type) {
        case PANEL_GENERIC:
            mp_print_str(print, ", panel: generic ");
            break;
        case PANEL_FM6126A:
            mp_print_str(print, ", panel: generic ");
            break;
    }

    mp_print_str(print, " )");
}

/***** Destructor ******/
mp_obj_t Hub75___del__(mp_obj_t self_in) {
    _Hub75_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Hub75_obj_t);
    self->hub75->stop(dma_complete);
    delete self->hub75;
    return mp_const_none;
}

/***** Constructor *****/
mp_obj_t Hub75_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { 
        ARG_width,
        ARG_height,
        ARG_buffer,
        ARG_panel_type,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_buffer, MP_ARG_OBJ, {.u_obj = nullptr} },
        { MP_QSTR_panel_type, MP_ARG_INT, {.u_int = 0} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int width = args[ARG_width].u_int;
    int height = args[ARG_height].u_int;
    PanelType paneltype = (PanelType)args[ARG_panel_type].u_int;

    Pixel *buffer = nullptr;

    if (args[ARG_buffer].u_obj) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_RW);
        buffer = (Pixel *)bufinfo.buf;
        if(bufinfo.len < (size_t)(width * height * 2 * sizeof(Pixel))) {
            mp_raise_ValueError("Supplied buffer is too small!");
        }
    } else {
        buffer = m_new(Pixel, width * height * 2);
    }

    hub75_obj = m_new_obj_with_finaliser(_Hub75_obj_t);
    hub75_obj->base.type = &Hub75_type;
    hub75_obj->buf = buffer;
    hub75_obj->hub75 = new Hub75(width, height, buffer, paneltype);

    return MP_OBJ_FROM_PTR(hub75_obj);
}

mp_obj_t Hub75_clear(mp_obj_t self_in) {
    _Hub75_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Hub75_obj_t);
    self->hub75->clear();
    return mp_const_none;
}

mp_obj_t Hub75_flip(mp_obj_t self_in) {
    _Hub75_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Hub75_obj_t);
    self->hub75->flip();
    return mp_const_none;
}

void Hub75_display_update() {
    if(hub75_obj) {
        hub75_obj->hub75->start(nullptr);
    }
}

mp_obj_t Hub75_start(mp_obj_t self_in) {
    _Hub75_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Hub75_obj_t);
    //size_t stack_size = 0;
    //mp_thread_create(&Hub75_display_update, nullptr, &stack_size);
    //multicore_reset_core1();
    //multicore_launch_core1(Hub75_display_update);
    self->hub75->start(dma_complete);
    return mp_const_none;
}

mp_obj_t Hub75_stop(mp_obj_t self_in) {
    _Hub75_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Hub75_obj_t);
    self->hub75->stop(dma_complete);
    return mp_const_none;
}

mp_obj_t Hub75_set_rgb(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_r, ARG_g, ARG_b };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_r, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_g, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int r = args[ARG_r].u_int;
    int g = args[ARG_g].u_int;
    int b = args[ARG_b].u_int;

    _Hub75_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Hub75_obj_t);
    self->hub75->set_rgb(x, y, r, g, b);

    return mp_const_none;
}

mp_obj_t Hub75_set_hsv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_h, ARG_s, ARG_v };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_h, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_s, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_v, MP_ARG_REQUIRED | MP_ARG_OBJ }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    float h = mp_obj_get_float(args[ARG_h].u_obj);
    float s = mp_obj_get_float(args[ARG_s].u_obj);
    float v = mp_obj_get_float(args[ARG_v].u_obj);

    _Hub75_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Hub75_obj_t);
    self->hub75->set_hsv(x, y, h, s, v);

    return mp_const_none;
}

mp_obj_t Hub75_set_all_hsv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_h, ARG_s, ARG_v };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_h, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_s, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_v, MP_ARG_REQUIRED | MP_ARG_OBJ }
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    float h = mp_obj_get_float(args[ARG_h].u_obj);
    float s = mp_obj_get_float(args[ARG_s].u_obj);
    float v = mp_obj_get_float(args[ARG_v].u_obj);

    _Hub75_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Hub75_obj_t);

    for (auto x = 0u; x < self->hub75->width; x++) {
        for (auto y = 0u; y < self->hub75->height; y++) {
            self->hub75->set_hsv(x, y, h, s, v);
        }
    }

    return mp_const_none;
}

}