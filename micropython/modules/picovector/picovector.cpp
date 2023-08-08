#include "micropython/modules/util.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

extern "C" {
#include "picovector.h"
#include "micropython/modules/picographics/picographics.h"
#include "py/stream.h"
#include "py/reader.h"
#include "extmod/vfs.h"

typedef struct _ModPicoGraphics_obj_t {
    mp_obj_base_t base;
    PicoGraphics *graphics;
    DisplayDriver *display;
    void *buffer;
} ModPicoGraphics_obj_t;

typedef struct _VECTOR_obj_t {
    mp_obj_base_t base;
    ModPicoGraphics_obj_t *graphics;
} _VECTOR_obj_t;

mp_obj_t VECTOR_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum {
        ARG_picographics
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_picographics, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(!MP_OBJ_IS_TYPE(args[ARG_picographics].u_obj, &ModPicoGraphics_type)) mp_raise_ValueError(MP_ERROR_TEXT("PicoGraphics Object Required"));

    _VECTOR_obj_t *self = m_new_obj(_VECTOR_obj_t);
    self->base.type = &VECTOR_type;
    self->graphics = (ModPicoGraphics_obj_t *)MP_OBJ_TO_PTR(args[ARG_picographics].u_obj);

    return self;
}

mp_obj_t VECTOR_regular_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_sides, ARG_radius, ARG_rotation };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_sides, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_radius, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_rotation, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _VECTOR_obj_t);

    Point origin(args[ARG_x].u_int, args[ARG_y].u_int);
    unsigned int sides = args[ARG_sides].u_int;
    float radius =  mp_obj_get_float(args[ARG_radius].u_obj);
    float rotation = mp_obj_get_float(args[ARG_rotation].u_obj);
    int o_x = args[ARG_x].u_int;
    int o_y = args[ARG_y].u_int;

    float angle = (360.0f / sides) * (M_PI / 180.0f);

    pretty_poly::point_t<int> *points = new pretty_poly::point_t<int>[sides];

    for(auto s = 0u; s < sides; s++) {
        float current_angle = angle * s + rotation;
        points[s] = {
            int(cos(current_angle) * radius + o_x),
            int(sin(current_angle) * radius + o_y)
        };
    }

    std::vector<pretty_poly::contour_t<int>> contours;
    contours.push_back({points, sides});
    self->graphics->graphics->polygon(contours);

    delete points;

    return mp_const_none;
}

mp_obj_t VECTOR_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    size_t num_tuples = n_args - 1;
    const mp_obj_t *lists = pos_args + 1;

    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], _VECTOR_obj_t);

    std::vector<pretty_poly::contour_t<int>> contours;

    for(auto i = 0u; i < num_tuples; i++) {
        mp_obj_t c_obj = lists[i];

        if(!mp_obj_is_exact_type(c_obj, &mp_type_list)) mp_raise_ValueError("Not a list");

        mp_obj_list_t *t_contour = MP_OBJ_TO_PTR2(c_obj, mp_obj_list_t);

        pretty_poly::point_t<int> *points = new pretty_poly::point_t<int>[t_contour->len];

        for(auto p = 0u; p < t_contour->len; p++) {
            mp_obj_t p_obj = t_contour->items[p];

            if(!mp_obj_is_exact_type(p_obj, &mp_type_tuple)) mp_raise_ValueError("Not a tuple");

            mp_obj_tuple_t *t_point = MP_OBJ_TO_PTR2(p_obj, mp_obj_tuple_t);
            points[p] = {
                mp_obj_get_int(t_point->items[0]),
                mp_obj_get_int(t_point->items[1]),
            };
        }

        contours.push_back({points, t_contour->len});
    }

    self->graphics->graphics->polygon(contours);

    for(auto contour : contours) {
        delete contour.points;
    }

    return mp_const_none;
}

}
