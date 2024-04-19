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

typedef struct _PATH_obj_t {
    mp_obj_base_t base;
    pp_path_t path;
} _PATH_obj_t;

void __printf_debug_flush() {
    for(auto i = 0u; i < 10; i++) {
        sleep_ms(1);
        mp_event_handle_nowait();
    }
}

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
    void *addr = m_tracked_calloc(sizeof(uint8_t), size);
    //mp_printf(&mp_plat_print, "addr %lu\n", addr);
    //__printf_debug_flush();
    return addr;
}

void *af_realloc(void *p, size_t size) {
    return NULL;
}

void af_free(void *p) {
    //mp_printf(&mp_plat_print, "af_free\n");
    //__printf_debug_flush();
    m_tracked_free(p);
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

mp_obj_t RECTANGLE_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_x, ARG_y, ARG_w, ARG_h };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_w, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_h, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _PATH_obj_t *self = mp_obj_malloc_with_finaliser(_PATH_obj_t, &POLYGON_type);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int w = args[ARG_w].u_int;
    int h = args[ARG_h].u_int;

    self->path.points = m_new(pp_point_t, 4);
    self->path.count = 4;

    self->path.points[0] = {picovector_point_type(x), picovector_point_type(y)};
    self->path.points[1] = {picovector_point_type(x + w), picovector_point_type(y)};
    self->path.points[2] = {picovector_point_type(x + w), picovector_point_type(y + h)};
    self->path.points[3] = {picovector_point_type(x), picovector_point_type(y + h)};

    return self;
}

mp_obj_t REGULAR_POLYGON_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_x, ARG_y, ARG_sides, ARG_radius, ARG_rotation };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_sides, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_radius, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_rotation, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _PATH_obj_t *self = mp_obj_malloc_with_finaliser(_PATH_obj_t, &POLYGON_type);

    Point origin(args[ARG_x].u_int, args[ARG_y].u_int);
    unsigned int sides = args[ARG_sides].u_int;
    float radius =  mp_obj_get_float(args[ARG_radius].u_obj);
    float rotation = 0.0f;
    if (args[ARG_rotation].u_obj != mp_const_none) {
        rotation = mp_obj_get_float(args[ARG_rotation].u_obj);
        rotation *= (M_PI / 180.0f);
    }
    int o_x = args[ARG_x].u_int;
    int o_y = args[ARG_y].u_int;

    float angle = (360.0f / sides) * (M_PI / 180.0f);

    self->path.points = m_new(pp_point_t, sides);
    self->path.count = sides;

    for(auto s = 0u; s < sides; s++) {
        float current_angle = angle * s + rotation;
        self->path.points[s] = {
            (picovector_point_type)(cos(current_angle) * radius) + o_x,
            (picovector_point_type)(sin(current_angle) * radius) + o_y
        };
    }

    return self;
}

mp_obj_t POLYGON_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _PATH_obj_t *self = mp_obj_malloc_with_finaliser(_PATH_obj_t, &POLYGON_type);

    size_t num_points = n_args;
    const mp_obj_t *points = all_args;

    if(num_points < 3) mp_raise_ValueError("Polygon: At least 3 points required.");

    self->path.points = m_new(pp_point_t, num_points);
    self->path.count = num_points;

    for(auto i = 0u; i < num_points; i++) {
        mp_obj_t c_obj = points[i];

        if(!mp_obj_is_exact_type(c_obj, &mp_type_tuple)) mp_raise_ValueError("Not a tuple");

        mp_obj_tuple_t *t_point = MP_OBJ_TO_PTR2(c_obj, mp_obj_tuple_t);

        if(t_point->len != 2) mp_raise_ValueError("Tuple must have X, Y");

        self->path.points[i] = {
            (picovector_point_type)mp_obj_get_int(t_point->items[0]),
            (picovector_point_type)mp_obj_get_int(t_point->items[1]),
        };
    }

    return self;
}

mp_obj_t POLYGON_centroid(mp_obj_t self_in) {
    _PATH_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PATH_obj_t);

    PP_COORD_TYPE sum_x = (PP_COORD_TYPE)0;
    PP_COORD_TYPE sum_y = (PP_COORD_TYPE)0;

    for(auto i = 0u; i < self->path.count; i++) {
        sum_x += self->path.points[i].x;
        sum_y += self->path.points[i].y;
    }

    sum_x /= (float)self->path.count;
    sum_y /= (float)self->path.count;

    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_int((int)(sum_x));
    tuple[1] = mp_obj_new_int((int)(sum_y));

    return mp_obj_new_tuple(2, tuple);
}

mp_obj_t POLYGON_bounds(mp_obj_t self_in) {
    _PATH_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PATH_obj_t);

    pp_rect_t bounds = pp_contour_bounds(&self->path);

    mp_obj_t tuple[4];
    tuple[0] = mp_obj_new_int((int)(bounds.x));
    tuple[1] = mp_obj_new_int((int)(bounds.y));
    tuple[2] = mp_obj_new_int((int)(bounds.w));
    tuple[3] = mp_obj_new_int((int)(bounds.h));

    return mp_obj_new_tuple(4, tuple);
}

void POLYGON_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    _PATH_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PATH_obj_t);

    pp_rect_t bounds = pp_contour_bounds(&self->path);

    mp_print_str(print, "Polygon(points = ");
    mp_obj_print_helper(print, mp_obj_new_int(self->path.count), PRINT_REPR);
    mp_print_str(print, ", bounds = ");
    mp_obj_print_helper(print, mp_obj_new_int(bounds.x), PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, mp_obj_new_int(bounds.y), PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, mp_obj_new_int(bounds.w), PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, mp_obj_new_int(bounds.h), PRINT_REPR);
    mp_print_str(print, ")");
}

mp_obj_t POLYGON__del__(mp_obj_t self_in) {
    _PATH_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PATH_obj_t);
    (void)self;
    // TODO: Do we actually need to free anything here, if it's on GC heap it should get collected
    return mp_const_none;
}

typedef struct _mp_obj_polygon_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_t polygon;
    size_t cur;
} mp_obj_polygon_it_t;

static mp_obj_t py_path_it_iternext(mp_obj_t self_in) {
    mp_obj_polygon_it_t *self = MP_OBJ_TO_PTR2(self_in, mp_obj_polygon_it_t);
    _PATH_obj_t *path = MP_OBJ_TO_PTR2(self->polygon, _PATH_obj_t);

    //mp_printf(&mp_plat_print, "points: %d, current: %d\n", polygon->contour.count, self->cur);

    if(self->cur >= path->path.count) return MP_OBJ_STOP_ITERATION;

    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_int((int)(path->path.points[self->cur].x));
    tuple[1] = mp_obj_new_int((int)(path->path.points[self->cur].y));

    self->cur++;
    return mp_obj_new_tuple(2, tuple);
}

mp_obj_t PATH_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf) {
    mp_obj_polygon_it_t *o = (mp_obj_polygon_it_t *)iter_buf;
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = py_path_it_iternext;
    o->polygon = o_in;
    o->cur = 0;
    return MP_OBJ_FROM_PTR(o);
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

mp_obj_t VECTOR_set_font(mp_obj_t self_in, mp_obj_t font, mp_obj_t size) {
    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VECTOR_obj_t);
    (void)self;

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
    (void)self;

    int font_size = mp_obj_get_int(size);
    (void)font_size;
    // TODO: Implement when Alright Fonts rewrite is ready
    self->vector->set_font_size(font_size);
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
    (void)self;

    mp_obj_t text_obj = args[ARG_text].u_obj;

    if(!mp_obj_is_str_or_bytes(text_obj)) mp_raise_TypeError("text: string required");

    GET_STR_DATA_LEN(text_obj, str, str_len);

    const std::string_view t((const char *)str, str_len);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    (void)x;
    (void)y;

    pp_mat3_t tt = pp_mat3_identity();

    if(args[ARG_angle].u_obj != mp_const_none) {
        pp_mat3_rotate(&tt, mp_obj_get_float(args[ARG_angle].u_obj));
    }

    pp_mat3_translate(&tt, (float)x, (float)y);

    //mp_printf(&mp_plat_print, "self->vector->text()\n");
    //__printf_debug_flush();

    self->vector->text(t, &tt);

    return mp_const_none;
}

mp_obj_t VECTOR_rotate(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_polygon, ARG_angle, ARG_origin_x, ARG_origin_y };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_polygon, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_angle, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_origin_x, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_origin_y, MP_ARG_INT, {.u_int = 0} }
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _VECTOR_obj_t);

    if(!MP_OBJ_IS_TYPE(args[ARG_polygon].u_obj, &POLYGON_type)) mp_raise_TypeError("rotate: polygon required");

    _PATH_obj_t *poly = MP_OBJ_TO_PTR2(args[ARG_polygon].u_obj, _PATH_obj_t);

    pp_point_t origin = {(PP_COORD_TYPE)args[ARG_origin_x].u_int, (PP_COORD_TYPE)args[ARG_origin_y].u_int};

    float angle = mp_obj_get_float(args[ARG_angle].u_obj);

    self->vector->rotate(&poly->path, origin, angle);

    return mp_const_none;
}

mp_obj_t VECTOR_translate(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_polygon, ARG_x, ARG_y };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_polygon, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_INT, {.u_int = 0} }
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _VECTOR_obj_t);

    if(!MP_OBJ_IS_TYPE(args[ARG_polygon].u_obj, &POLYGON_type)) mp_raise_TypeError("rotate: polygon required");

    _PATH_obj_t *poly = MP_OBJ_TO_PTR2(args[ARG_polygon].u_obj, _PATH_obj_t);

    pp_point_t translate = {(PP_COORD_TYPE)args[ARG_x].u_int, (PP_COORD_TYPE)args[ARG_y].u_int};

    self->vector->translate(&poly->path, translate);

    return mp_const_none;
}

mp_obj_t VECTOR_draw(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    size_t num_polygons = n_args - 1;
    const mp_obj_t *polygons = pos_args + 1;

    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], _VECTOR_obj_t);

    pp_poly_t group;
    group.count = num_polygons;
    group.paths = (pp_path_t *)malloc(sizeof(pp_path_t) * num_polygons);

    for(auto i = 0u; i < num_polygons; i++) {
        mp_obj_t poly_obj = polygons[i];

        if(!MP_OBJ_IS_TYPE(poly_obj, &POLYGON_type)) mp_raise_TypeError("draw: Polygon required.");

        _PATH_obj_t *poly = MP_OBJ_TO_PTR2(poly_obj, _PATH_obj_t);
        group.paths[i].points = poly->path.points;
        group.paths[i].count = poly->path.count;
    }

    self->vector->draw(&group);

    free(group.paths);

    return mp_const_none;
}

}
