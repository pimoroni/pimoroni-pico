#include "micropython/modules/util.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/pico_vector/pico_vector.hpp"

using namespace pimoroni;

extern "C" {
#include "picovector.h"
#include "micropython/modules/picographics/picographics.h"
#include "py/stream.h"
#include "py/reader.h"
#include "extmod/vfs.h"
#include <stdarg.h>

typedef struct _ModPicoGraphics_obj_t {
    mp_obj_base_t base;
    PicoGraphics *graphics;
    DisplayDriver *display;
    void *buffer;
} ModPicoGraphics_obj_t;

typedef struct _VECTOR_obj_t {
    mp_obj_base_t base;
    void *mem;
    PicoVector *vector;
} _VECTOR_obj_t;

typedef struct _TRANSFORM_obj_t {
    mp_obj_base_t base;
    pp_mat3_t transform;
} _TRANSFORM_obj_t;

typedef struct _POLY_obj_t {
    mp_obj_base_t base;
    pp_poly_t *poly;
} _POLY_obj_t;

void __printf_debug_flush() {
    for(auto i = 0u; i < 10; i++) {
        sleep_ms(1);
        mp_event_handle_nowait();
    }
}

#define mp_picovector_get_point_type mp_obj_get_float
#define mp_picovector_set_point_type mp_obj_new_float

int mp_vprintf(const mp_print_t *print, const char *fmt, va_list args);

void af_debug(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = mp_vprintf(&mp_plat_print, fmt, ap);
    va_end(ap);
    __printf_debug_flush();
    (void)ret;
}

void *af_malloc(size_t size) {
    //mp_printf(&mp_plat_print, "af_malloc %lu\n", size);
    //__printf_debug_flush();
    //void *addr = m_tracked_calloc(sizeof(uint8_t), size);
    void *addr = m_malloc(size);
    //mp_printf(&mp_plat_print, "addr %lu\n", addr);
    //__printf_debug_flush();
    return addr;
}

void *af_realloc(void *p, size_t size) {
    return m_realloc(p, size);
}

void af_free(void *p) {
    //mp_printf(&mp_plat_print, "af_free\n");
    //__printf_debug_flush();
    //m_tracked_free(p);
    m_free(p);
}

void* fileio_open(const char *filename) {
    mp_obj_t fn = mp_obj_new_str(filename, (mp_uint_t)strlen(filename));

    //mp_printf(&mp_plat_print, "Opening file %s\n", filename);
    //__printf_debug_flush();

    mp_obj_t args[2] = {
        fn,
        MP_OBJ_NEW_QSTR(MP_QSTR_r),
    };

    // Stat the file to get its size
    // example tuple response: (32768, 0, 0, 0, 0, 0, 5153, 1654709815, 1654709815, 1654709815)
    //mp_obj_t stat = mp_vfs_stat(fn);
    //mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(stat, mp_obj_tuple_t);
    //int filesize = mp_obj_get_int(tuple->items[6]);
    //mp_printf(&mp_plat_print, "Size %lu\n", filesize);

    mp_obj_t fhandle = mp_vfs_open(MP_ARRAY_SIZE(args), &args[0], (mp_map_t *)&mp_const_empty_map);

    return (void*)fhandle;
}

void fileio_close(void* fhandle) {
    mp_stream_close((mp_obj_t)fhandle);
}

size_t fileio_read(void* fhandle, void *buf, size_t len) {
    //mp_printf(&mp_plat_print, "Reading %lu bytes\n", len);
    //__printf_debug_flush();
    int error;
    return mp_stream_read_exactly((mp_obj_t)fhandle, buf, len, &error);
}

int fileio_getc(void* fhandle) {
    unsigned char buf;
    //mp_printf(&mp_plat_print, "Reading char\n");
    //__printf_debug_flush();
    fileio_read(fhandle, (void *)&buf, 1);
    return (int)buf;
}

size_t fileio_tell(void* fhandle) {
    struct mp_stream_seek_t seek_s;
    seek_s.offset = 0;
    seek_s.whence = SEEK_CUR;

    const mp_stream_p_t *stream_p = mp_get_stream((mp_obj_t)fhandle);

    int error;
    mp_uint_t res = stream_p->ioctl((mp_obj_t)fhandle, MP_STREAM_SEEK, (mp_uint_t)(uintptr_t)&seek_s, &error);
    if (res == MP_STREAM_ERROR) {
        mp_raise_OSError(error);
    }

    return seek_s.offset;
}

// Re-implementation of stream.c/STATIC mp_obj_t stream_seek(size_t n_args, const mp_obj_t *args)
size_t fileio_seek(void* fhandle, size_t pos) {
    struct mp_stream_seek_t seek_s;
    seek_s.offset = pos;
    seek_s.whence = SEEK_SET;

    const mp_stream_p_t *stream_p = mp_get_stream((mp_obj_t)fhandle);

    int error;
    mp_uint_t res = stream_p->ioctl((mp_obj_t)fhandle, MP_STREAM_SEEK, (mp_uint_t)(uintptr_t)&seek_s, &error);
    if (res == MP_STREAM_ERROR) {
        mp_raise_OSError(error);
    }

    return seek_s.offset;
}

/*
static const std::string_view mp_obj_to_string_r(const mp_obj_t &obj) {
    if(mp_obj_is_str_or_bytes(obj)) {
        GET_STR_DATA_LEN(obj, str, str_len);
        return std::string_view((const char*)str, str_len);
    }
    mp_raise_TypeError("can't convert object to str implicitly");
}
*/

/* POLYGON */

mp_obj_t POLYGON_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _POLY_obj_t *self = mp_obj_malloc_with_finaliser(_POLY_obj_t, &POLYGON_type);
    self->poly = pp_poly_new();
    return self;
}

mp_obj_t POLYGON__del__(mp_obj_t self_in) {
    _POLY_obj_t *self = MP_OBJ_TO_PTR2(self_in, _POLY_obj_t);
    pp_poly_free(self->poly);
    return mp_const_none;
}

mp_obj_t POLYGON_path(size_t n_args, const mp_obj_t *all_args) {
    _POLY_obj_t *self = MP_OBJ_TO_PTR2(all_args[0], _POLY_obj_t);

    size_t num_points = n_args - 1;
    const mp_obj_t *points = all_args + 1;

    pp_path_t *path = pp_poly_add_path(self->poly);

    if(num_points < 3) mp_raise_ValueError("Polygon: At least 3 points required.");

    for(auto i = 0u; i < num_points; i++) {
        mp_obj_t c_obj = points[i];

        if(!mp_obj_is_exact_type(c_obj, &mp_type_tuple)) mp_raise_ValueError("Not a tuple");

        mp_obj_tuple_t *t_point = MP_OBJ_TO_PTR2(c_obj, mp_obj_tuple_t);

        if(t_point->len != 2) mp_raise_ValueError("Tuple must have X, Y");

        pp_path_add_point(path, {
            (picovector_point_type)mp_picovector_get_point_type(t_point->items[0]),
            (picovector_point_type)mp_picovector_get_point_type(t_point->items[1]),
        });
    }

    return mp_const_none;
}

mp_obj_t POLYGON_rectangle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_w, ARG_h, ARG_corners, ARG_stroke };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_w, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_h, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_corners, MP_ARG_OBJ, { .u_obj = mp_const_none }},
        { MP_QSTR_stroke, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _POLY_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _POLY_obj_t);

    picovector_point_type x = mp_picovector_get_point_type(args[ARG_x].u_obj);
    picovector_point_type y = mp_picovector_get_point_type(args[ARG_y].u_obj);
    picovector_point_type w = mp_picovector_get_point_type(args[ARG_w].u_obj);
    picovector_point_type h = mp_picovector_get_point_type(args[ARG_h].u_obj);
    picovector_point_type s = args[ARG_stroke].u_obj == mp_const_none ? 0 : mp_picovector_get_point_type(args[ARG_stroke].u_obj);

    picovector_point_type r1 = 0;
    picovector_point_type r2 = 0;
    picovector_point_type r3 = 0;
    picovector_point_type r4 = 0;

    if(mp_obj_is_exact_type(args[ARG_corners].u_obj, &mp_type_tuple)){
        mp_obj_tuple_t *t_corners = MP_OBJ_TO_PTR2(args[ARG_corners].u_obj, mp_obj_tuple_t);

        if(t_corners->len != 4) mp_raise_ValueError("Corners must have r1, r2, r3, r4");

        r1 = mp_picovector_get_point_type(t_corners->items[0]);
        r2 = mp_picovector_get_point_type(t_corners->items[1]);
        r3 = mp_picovector_get_point_type(t_corners->items[2]);
        r4 = mp_picovector_get_point_type(t_corners->items[3]);
    }

    pp_poly_merge(self->poly, ppp_rect({
        x, y, w, h,
        s,
        r1, r2, r3, r4
    }));

    return self;
}

mp_obj_t POLYGON_regular(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_sides, ARG_radius, ARG_stroke };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_radius, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sides, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_stroke, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _POLY_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _POLY_obj_t);


    picovector_point_type x = mp_picovector_get_point_type(args[ARG_x].u_obj);
    picovector_point_type y = mp_picovector_get_point_type(args[ARG_y].u_obj);
    picovector_point_type r = mp_picovector_get_point_type(args[ARG_radius].u_obj);
    int e = args[ARG_sides].u_int;
    picovector_point_type s = args[ARG_stroke].u_obj == mp_const_none ? 0 : mp_picovector_get_point_type(args[ARG_stroke].u_obj);

    pp_poly_merge(self->poly, ppp_regular({
        x, y,
        r,
        e,
        s
    }));

    return self;
}

mp_obj_t POLYGON_circle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_radius, ARG_stroke };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_radius, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_stroke, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _POLY_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _POLY_obj_t);

    picovector_point_type x = mp_picovector_get_point_type(args[ARG_x].u_obj);
    picovector_point_type y = mp_picovector_get_point_type(args[ARG_y].u_obj);
    picovector_point_type r = mp_picovector_get_point_type(args[ARG_radius].u_obj);
    picovector_point_type s = args[ARG_stroke].u_obj == mp_const_none ? 0 : mp_picovector_get_point_type(args[ARG_stroke].u_obj);

    pp_poly_merge(self->poly, ppp_circle({
        x, y,
        r,
        s
    }));

    return self;
}

mp_obj_t POLYGON_arc(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_x, ARG_y, ARG_radius, ARG_from, ARG_to, ARG_stroke };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_radius, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_from, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_to, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_stroke, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _POLY_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _POLY_obj_t);

    picovector_point_type x = mp_picovector_get_point_type(args[ARG_x].u_obj);
    picovector_point_type y = mp_picovector_get_point_type(args[ARG_y].u_obj);
    picovector_point_type r = mp_picovector_get_point_type(args[ARG_radius].u_obj);
    picovector_point_type f = mp_picovector_get_point_type(args[ARG_from].u_obj);
    picovector_point_type t = mp_picovector_get_point_type(args[ARG_to].u_obj);
    picovector_point_type s = args[ARG_stroke].u_obj == mp_const_none ? 0 : mp_picovector_get_point_type(args[ARG_stroke].u_obj);

    pp_poly_merge(self->poly, ppp_arc({
        x, y,
        r,
        s,
        f,
        t
    }));

    return self;
}

// Utility functions

mp_obj_t POLYGON_centroid(mp_obj_t self_in) {
    _POLY_obj_t *self = MP_OBJ_TO_PTR2(self_in, _POLY_obj_t);

    PP_COORD_TYPE sum_x = (PP_COORD_TYPE)0;
    PP_COORD_TYPE sum_y = (PP_COORD_TYPE)0;

    // TODO: Maybe include in pretty-poly?
    // Might need to handle multiple paths
    pp_path_t *path = self->poly->paths;

    for(auto i = 0; i < path->count; i++) {
        sum_x += path->points[i].x;
        sum_y += path->points[i].y;
    }

    sum_x /= (float)path->count;
    sum_y /= (float)path->count;

    mp_obj_t tuple[2];
    tuple[0] = mp_picovector_set_point_type((int)(sum_x));
    tuple[1] = mp_picovector_set_point_type((int)(sum_y));

    return mp_obj_new_tuple(2, tuple);
}

mp_obj_t POLYGON_bounds(mp_obj_t self_in) {
    _POLY_obj_t *self = MP_OBJ_TO_PTR2(self_in, _POLY_obj_t);

    pp_rect_t bounds = pp_poly_bounds(self->poly);

    mp_obj_t tuple[4];
    tuple[0] = mp_picovector_set_point_type((int)(bounds.x));
    tuple[1] = mp_picovector_set_point_type((int)(bounds.y));
    tuple[2] = mp_picovector_set_point_type((int)(bounds.w));
    tuple[3] = mp_picovector_set_point_type((int)(bounds.h));

    return mp_obj_new_tuple(4, tuple);
}

void _pp_path_transform(pp_path_t *path, pp_mat3_t *transform) {
    for (int i = 0; i < path->count; i++) {
        path->points[i] = pp_point_transform(&path->points[i], transform);
    }
}

void _pp_poly_transform(pp_poly_t *poly, pp_mat3_t *transform) {
    pp_path_t *path = poly->paths;

    while(path) {
        _pp_path_transform(path, transform);
        path = path->next;
    }
}

mp_obj_t POLYGON_transform(mp_obj_t self_in, mp_obj_t transform_in) {
    _POLY_obj_t *self = MP_OBJ_TO_PTR2(self_in, _POLY_obj_t);

    if (!MP_OBJ_IS_TYPE(transform_in, &TRANSFORM_type)) mp_raise_ValueError("Transform required");
    _TRANSFORM_obj_t *transform = (_TRANSFORM_obj_t *)MP_OBJ_TO_PTR(transform_in);

    _pp_poly_transform(self->poly, &transform->transform);

    return mp_const_none;
}

void POLYGON_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    _POLY_obj_t *self = MP_OBJ_TO_PTR2(self_in, _POLY_obj_t);
    (void)self;

    // TODO: Make print better
    mp_print_str(print, "Polygon();");
}

typedef struct _mp_obj_polygon_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_t polygon;
    pp_path_t *cur;
} mp_obj_polygon_it_t;

static mp_obj_t POLYGON_it_iternext(mp_obj_t self_in) {
    mp_obj_polygon_it_t *self = MP_OBJ_TO_PTR2(self_in, mp_obj_polygon_it_t);
    //_POLY_obj_t *poly = MP_OBJ_TO_PTR2(self->polygon, _POLY_obj_t);

    //mp_printf(&mp_plat_print, "points: %d, current: %d\n", polygon->contour.count, self->cur);

    if(!self->cur) return MP_OBJ_STOP_ITERATION;

    mp_obj_t tuple[self->cur->count];
    for (auto i = 0; i < self->cur->count; i++) {
        mp_obj_t t_point[2] = {
            mp_picovector_set_point_type((int)(self->cur->points[i].x)),
            mp_picovector_set_point_type((int)(self->cur->points[i].y))
        };
        tuple[i] = mp_obj_new_tuple(2, t_point);
    }

    self->cur = self->cur->next;
    return mp_obj_new_tuple(self->cur->count, tuple);
}

mp_obj_t POLYGON_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf) {
    mp_obj_polygon_it_t *o = (mp_obj_polygon_it_t *)iter_buf;
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = POLYGON_it_iternext;
    o->polygon = o_in;
    o->cur = MP_OBJ_TO_PTR2(o_in, _POLY_obj_t)->poly->paths;
    return MP_OBJ_FROM_PTR(o);
}

/* TRANSFORM */

mp_obj_t TRANSFORM_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _TRANSFORM_obj_t *self = m_new_obj(_TRANSFORM_obj_t);
    self->base.type = &TRANSFORM_type;

    self->transform = pp_mat3_identity();

    return self;
}

mp_obj_t TRANSFORM_rotate(mp_obj_t self_in, mp_obj_t angle_in, mp_obj_t origin_in) {
    _TRANSFORM_obj_t *transform = MP_OBJ_TO_PTR2(self_in, _TRANSFORM_obj_t);

    float angle = mp_obj_get_float(angle_in);

    if(mp_obj_is_exact_type(origin_in, &mp_type_tuple)) {
        mp_obj_tuple_t *t_origin = MP_OBJ_TO_PTR2(origin_in, mp_obj_tuple_t);

        if(t_origin->len != 2) mp_raise_ValueError("Origin Tuple must have X, Y");
    
        picovector_point_type x = mp_picovector_get_point_type(t_origin->items[0]);
        picovector_point_type y = mp_picovector_get_point_type(t_origin->items[1]);

        pp_mat3_translate(&transform->transform, x, y);
        pp_mat3_rotate(&transform->transform, angle);
        pp_mat3_translate(&transform->transform, -x, -y);
    } else {
        pp_mat3_rotate(&transform->transform, angle);
    }

    return mp_const_none;
}

mp_obj_t TRANSFORM_translate(mp_obj_t self_in, mp_obj_t x_in, mp_obj_t y_in) {
    _TRANSFORM_obj_t *transform = MP_OBJ_TO_PTR2(self_in, _TRANSFORM_obj_t);

    picovector_point_type o_x = mp_picovector_get_point_type(x_in);
    picovector_point_type o_y = mp_picovector_get_point_type(y_in);

    pp_mat3_translate(&transform->transform, o_x, o_y);

    return mp_const_none;
}

mp_obj_t TRANSFORM_scale(mp_obj_t self_in, mp_obj_t x_in, mp_obj_t y_in) {
    _TRANSFORM_obj_t *transform = MP_OBJ_TO_PTR2(self_in, _TRANSFORM_obj_t);

    picovector_point_type o_x = mp_picovector_get_point_type(x_in);
    picovector_point_type o_y = mp_picovector_get_point_type(y_in);

    pp_mat3_scale(&transform->transform, o_x, o_y);

    return mp_const_none;
}

mp_obj_t TRANSFORM_reset(mp_obj_t self_in) {
    _TRANSFORM_obj_t *transform = MP_OBJ_TO_PTR2(self_in, _TRANSFORM_obj_t);
    transform->transform = pp_mat3_identity();
    return mp_const_none;
}

/* VECTOR */

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
    ModPicoGraphics_obj_t *graphics = (ModPicoGraphics_obj_t *)MP_OBJ_TO_PTR(args[ARG_picographics].u_obj);

    // The PicoVector class calls `pretty_poly::init()` with the memory region
    // it does not store a pointer to this, so we need to store one ourselves
    // TODO: C Pretty Poly does not support runtime memory allocation
    //self->mem = m_new(uint8_t, PicoVector::pretty_poly_buffer_size());

    self->vector = m_new_class(PicoVector, graphics->graphics, self->mem);

    return self;
}

mp_obj_t VECTOR_set_transform(mp_obj_t self_in, mp_obj_t transform_in) {
    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VECTOR_obj_t);
    (void)self;

    if(transform_in == mp_const_none) {
        pp_mat3_t* old = pp_transform(NULL);
        (void)old; // TODO: Return old transform?
    } else if MP_OBJ_IS_TYPE(transform_in, &TRANSFORM_type) {
        _TRANSFORM_obj_t *transform = (_TRANSFORM_obj_t *)MP_OBJ_TO_PTR(transform_in);
        pp_mat3_t* old = pp_transform(&transform->transform);
        (void)old;
    } else {
        // TODO: ValueError?
    }

    return mp_const_none;
}

mp_obj_t VECTOR_set_font(mp_obj_t self_in, mp_obj_t font, mp_obj_t size) {
    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VECTOR_obj_t);

    int font_size = mp_obj_get_int(size);
    (void)font_size;
    bool result = false;

    if (mp_obj_is_str(font)) {
        // TODO: Implement when Alright Fonts rewrite is ready
        GET_STR_DATA_LEN(font, str, str_len);
        result = self->vector->set_font((const char*)str, font_size);
    }
    else {
        
    }
    return result ? mp_const_true : mp_const_false;
}

mp_obj_t VECTOR_set_font_size(mp_obj_t self_in, mp_obj_t size) {
    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VECTOR_obj_t);

    int font_size = mp_obj_get_int(size);
    self->vector->set_font_size(font_size);
    return mp_const_none;
}

mp_obj_t VECTOR_set_font_word_spacing(mp_obj_t self_in, mp_obj_t spacing) {
    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VECTOR_obj_t);

    self->vector->set_font_word_spacing(mp_obj_get_int(spacing));
    return mp_const_none;
}

mp_obj_t VECTOR_set_font_letter_spacing(mp_obj_t self_in, mp_obj_t spacing) {
    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VECTOR_obj_t);

    self->vector->set_font_letter_spacing(mp_obj_get_int(spacing));
    return mp_const_none;
}

mp_obj_t VECTOR_set_font_line_height(mp_obj_t self_in, mp_obj_t spacing) {
    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VECTOR_obj_t);

    self->vector->set_font_line_height(mp_obj_get_int(spacing));
    return mp_const_none;
}

mp_obj_t VECTOR_set_font_align(mp_obj_t self_in, mp_obj_t align) {
    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VECTOR_obj_t);

    self->vector->set_font_align(mp_obj_get_int(align));
    return mp_const_none;
}

mp_obj_t VECTOR_measure_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_text, ARG_x, ARG_y, ARG_angle };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_text, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_angle, MP_ARG_OBJ, {.u_obj = mp_const_none} }
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _VECTOR_obj_t);

    mp_obj_t text_obj = args[ARG_text].u_obj;

    if(!mp_obj_is_str_or_bytes(text_obj)) mp_raise_TypeError("text: string required");

    GET_STR_DATA_LEN(text_obj, str, str_len);

    const std::string_view t((const char *)str, str_len);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;

    pp_mat3_t tt = pp_mat3_identity();

    if(args[ARG_angle].u_obj != mp_const_none) {
        pp_mat3_rotate(&tt, mp_obj_get_float(args[ARG_angle].u_obj));
    }

    pp_mat3_translate(&tt, (float)x, (float)y);

    pp_rect_t bounds = self->vector->measure_text(t, &tt);

    // TODO: Should probably add the transformations available to text here?
    mp_obj_t tuple[4];
    tuple[0] = mp_picovector_set_point_type((int)(bounds.x));
    tuple[1] = mp_picovector_set_point_type((int)(bounds.y));
    tuple[2] = mp_picovector_set_point_type((int)(bounds.w));
    tuple[3] = mp_picovector_set_point_type((int)(bounds.h));

    return mp_obj_new_tuple(4, tuple);
}

mp_obj_t VECTOR_set_clip(mp_obj_t self_in, mp_obj_t clip_in) {
    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VECTOR_obj_t);

    picovector_point_type x = self->vector->graphics->bounds.x;
    picovector_point_type y = self->vector->graphics->bounds.y;
    picovector_point_type w = self->vector->graphics->bounds.w;
    picovector_point_type h = self->vector->graphics->bounds.h;

    if(mp_obj_is_exact_type(clip_in, &mp_type_tuple)){
        mp_obj_tuple_t *t_clip = MP_OBJ_TO_PTR2(clip_in, mp_obj_tuple_t);

        if(t_clip->len != 4) mp_raise_ValueError("Clip must have x, y, w, h");

        x = mp_picovector_get_point_type(t_clip->items[0]);
        y = mp_picovector_get_point_type(t_clip->items[1]);
        w = mp_picovector_get_point_type(t_clip->items[2]);
        h = mp_picovector_get_point_type(t_clip->items[3]);
    }

    pp_clip(x, y, w, h);

    return mp_const_none;
}

mp_obj_t VECTOR_set_antialiasing(mp_obj_t self_in, mp_obj_t aa) {
    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VECTOR_obj_t);

    self->vector->set_antialiasing((pp_antialias_t)mp_obj_get_int(aa));
    return mp_const_none;
}

mp_obj_t VECTOR_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_text, ARG_x, ARG_y, ARG_angle };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_text, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_angle, MP_ARG_OBJ, {.u_obj = mp_const_none} }
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _VECTOR_obj_t);

    mp_obj_t text_obj = args[ARG_text].u_obj;

    if(!mp_obj_is_str_or_bytes(text_obj)) mp_raise_TypeError("text: string required");

    GET_STR_DATA_LEN(text_obj, str, str_len);

    const std::string_view t((const char *)str, str_len);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;

    pp_mat3_t tt = pp_mat3_identity();

    if(args[ARG_angle].u_obj != mp_const_none) {
        pp_mat3_rotate(&tt, mp_obj_get_float(args[ARG_angle].u_obj));
    }

    pp_mat3_translate(&tt, (float)x, (float)y);

    self->vector->text(t, &tt);

    return mp_const_none;
}

mp_obj_t VECTOR_draw(mp_obj_t self_in, mp_obj_t poly_in) {
    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VECTOR_obj_t);
    (void)self;

    if(!MP_OBJ_IS_TYPE(poly_in, &POLYGON_type)) mp_raise_TypeError("draw: Polygon required.");

    _POLY_obj_t *poly = MP_OBJ_TO_PTR2(poly_in, _POLY_obj_t);

    pp_render(poly->poly);

    return mp_const_none;
}

}
