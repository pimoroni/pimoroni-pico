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

typedef struct _POLYGON_obj_t {
    mp_obj_base_t base;
    pretty_poly::contour_t<picovector_point_type> contour;
} _POLYGON_obj_t;

pretty_poly::file_io::file_io(std::string_view filename) {
    mp_obj_t fn = mp_obj_new_str(filename.data(), (mp_uint_t)filename.size());

    //mp_printf(&mp_plat_print, "Opening file %s\n", filename.data());

    mp_obj_t args[2] = {
        fn,
        MP_OBJ_NEW_QSTR(MP_QSTR_r),
    };

    // Stat the file to get its size
    // example tuple response: (32768, 0, 0, 0, 0, 0, 5153, 1654709815, 1654709815, 1654709815)
    mp_obj_t stat = mp_vfs_stat(fn);
    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(stat, mp_obj_tuple_t);
    filesize = mp_obj_get_int(tuple->items[6]);

    mp_obj_t fhandle = mp_vfs_open(MP_ARRAY_SIZE(args), &args[0], (mp_map_t *)&mp_const_empty_map);

    this->state = (void *)fhandle;
}

pretty_poly::file_io::~file_io() {
    mp_stream_close((mp_obj_t)this->state);
}

size_t pretty_poly::file_io::read(void *buf, size_t len) {
    //mp_printf(&mp_plat_print, "Reading %lu bytes\n", len);
    mp_obj_t fhandle = this->state;
    int error;
    return mp_stream_read_exactly(fhandle, buf, len, &error);
}

size_t pretty_poly::file_io::tell() {
    mp_obj_t fhandle = this->state;
    struct mp_stream_seek_t seek_s;
    seek_s.offset = 0;
    seek_s.whence = SEEK_CUR;

    const mp_stream_p_t *stream_p = mp_get_stream(fhandle);

    int error;
    mp_uint_t res = stream_p->ioctl(fhandle, MP_STREAM_SEEK, (mp_uint_t)(uintptr_t)&seek_s, &error);
    if (res == MP_STREAM_ERROR) {
        mp_raise_OSError(error);
    }

    return seek_s.offset;
}

bool pretty_poly::file_io::fail() {
    return false;
}

// Re-implementation of stream.c/STATIC mp_obj_t stream_seek(size_t n_args, const mp_obj_t *args)
size_t pretty_poly::file_io::seek(size_t pos) {
    mp_obj_t fhandle = this->state;
    struct mp_stream_seek_t seek_s;
    seek_s.offset = pos;
    seek_s.whence = SEEK_SET;

    const mp_stream_p_t *stream_p = mp_get_stream(fhandle);

    int error;
    mp_uint_t res = stream_p->ioctl(fhandle, MP_STREAM_SEEK, (mp_uint_t)(uintptr_t)&seek_s, &error);
    if (res == MP_STREAM_ERROR) {
        mp_raise_OSError(error);
    }

    return seek_s.offset;
}

static const std::string_view mp_obj_to_string_r(const mp_obj_t &obj) {
    if(mp_obj_is_str_or_bytes(obj)) {
        GET_STR_DATA_LEN(obj, str, str_len);
        return std::string_view((const char*)str, str_len);
    }
    mp_raise_TypeError("can't convert object to str implicitly");
}

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

    _POLYGON_obj_t *self = m_new_obj_with_finaliser(_POLYGON_obj_t);
    self->base.type = &POLYGON_type;

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;
    int w = args[ARG_w].u_int;
    int h = args[ARG_h].u_int;

    self->contour.points = m_new(pretty_poly::point_t<picovector_point_type>, 4);
    self->contour.count = 4;

    self->contour.points[0] = {picovector_point_type(x), picovector_point_type(y)};
    self->contour.points[1] = {picovector_point_type(x + w), picovector_point_type(y)};
    self->contour.points[2] = {picovector_point_type(x + w), picovector_point_type(y + h)};
    self->contour.points[3] = {picovector_point_type(x), picovector_point_type(y + h)};

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

    _POLYGON_obj_t *self = m_new_obj_with_finaliser(_POLYGON_obj_t);
    self->base.type = &POLYGON_type;

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

    self->contour.points = m_new(pretty_poly::point_t<picovector_point_type>, sides);
    self->contour.count = sides;

    for(auto s = 0u; s < sides; s++) {
        float current_angle = angle * s + rotation;
        self->contour.points[s] = {
            (picovector_point_type)(cos(current_angle) * radius) + o_x,
            (picovector_point_type)(sin(current_angle) * radius) + o_y
        };
    }

    return self;
}

mp_obj_t POLYGON_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _POLYGON_obj_t *self = m_new_obj_with_finaliser(_POLYGON_obj_t);
    self->base.type = &POLYGON_type;

    size_t num_points = n_args;
    const mp_obj_t *points = all_args;

    if(num_points < 3) mp_raise_ValueError("Polygon: At least 3 points required.");

    self->contour.points = m_new(pretty_poly::point_t<picovector_point_type>, num_points);
    self->contour.count = num_points;

    for(auto i = 0u; i < num_points; i++) {
        mp_obj_t c_obj = points[i];

        if(!mp_obj_is_exact_type(c_obj, &mp_type_tuple)) mp_raise_ValueError("Not a tuple");

        mp_obj_tuple_t *t_point = MP_OBJ_TO_PTR2(c_obj, mp_obj_tuple_t);

        if(t_point->len != 2) mp_raise_ValueError("Tuple must have X, Y");

        self->contour.points[i] = {
            (picovector_point_type)mp_obj_get_int(t_point->items[0]),
            (picovector_point_type)mp_obj_get_int(t_point->items[1]),
        };
    }

    return self;
}

mp_obj_t POLYGON_centroid(mp_obj_t self_in) {
    _POLYGON_obj_t *self = MP_OBJ_TO_PTR2(self_in, _POLYGON_obj_t);

    pretty_poly::point_t<picovector_point_type> sum(0, 0);

    for(auto i = 0u; i < self->contour.count; i++) {
        sum += self->contour.points[i];
    }

    sum /= (float)self->contour.count;

    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_int((int)(sum.x));
    tuple[1] = mp_obj_new_int((int)(sum.y));

    return mp_obj_new_tuple(2, tuple);
}

mp_obj_t POLYGON_bounds(mp_obj_t self_in) {
    _POLYGON_obj_t *self = MP_OBJ_TO_PTR2(self_in, _POLYGON_obj_t);

    mp_obj_t tuple[4];
    tuple[0] = mp_obj_new_int((int)(self->contour.bounds().x));
    tuple[1] = mp_obj_new_int((int)(self->contour.bounds().y));
    tuple[2] = mp_obj_new_int((int)(self->contour.bounds().w));
    tuple[3] = mp_obj_new_int((int)(self->contour.bounds().h));

    return mp_obj_new_tuple(4, tuple);
}

void POLYGON_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    _POLYGON_obj_t *self = MP_OBJ_TO_PTR2(self_in, _POLYGON_obj_t);

    mp_print_str(print, "Polygon(points = ");
    mp_obj_print_helper(print, mp_obj_new_int(self->contour.count), PRINT_REPR);
    mp_print_str(print, ", bounds = ");
    mp_obj_print_helper(print, mp_obj_new_int(self->contour.bounds().x), PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, mp_obj_new_int(self->contour.bounds().y), PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, mp_obj_new_int(self->contour.bounds().w), PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, mp_obj_new_int(self->contour.bounds().h), PRINT_REPR);
    mp_print_str(print, ")");
}

mp_obj_t POLYGON__del__(mp_obj_t self_in) {
    _POLYGON_obj_t *self = MP_OBJ_TO_PTR2(self_in, _POLYGON_obj_t);
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

STATIC mp_obj_t py_image_it_iternext(mp_obj_t self_in) {
    mp_obj_polygon_it_t *self = MP_OBJ_TO_PTR2(self_in, mp_obj_polygon_it_t);
    _POLYGON_obj_t *polygon = MP_OBJ_TO_PTR2(self->polygon, _POLYGON_obj_t);

    //mp_printf(&mp_plat_print, "points: %d, current: %d\n", polygon->contour.count, self->cur);

    if(self->cur >= polygon->contour.count) return MP_OBJ_STOP_ITERATION;

    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_int((int)(polygon->contour.points[self->cur].x));
    tuple[1] = mp_obj_new_int((int)(polygon->contour.points[self->cur].y));

    self->cur++;
    return mp_obj_new_tuple(2, tuple);
}

mp_obj_t POLYGON_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf) {
    mp_obj_polygon_it_t *o = (mp_obj_polygon_it_t *)iter_buf;
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = py_image_it_iternext;
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
    self->mem = m_new(uint8_t, PicoVector::pretty_poly_buffer_size());

    self->vector = m_new_class(PicoVector, graphics->graphics, self->mem);

    return self;
}

mp_obj_t VECTOR_set_font(mp_obj_t self_in, mp_obj_t font, mp_obj_t size) {
    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VECTOR_obj_t);

    int font_size = mp_obj_get_int(size);
    bool result = false;

    if (mp_obj_is_str(font)) {
        result = self->vector->set_font(mp_obj_to_string_r(font), font_size);
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

mp_obj_t VECTOR_set_antialiasing(mp_obj_t self_in, mp_obj_t aa) {
    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(self_in, _VECTOR_obj_t);

    self->vector->set_antialiasing((pretty_poly::antialias_t)mp_obj_get_int(aa));
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

    const std::string_view t((const char*)str, str_len);

    int x = args[ARG_x].u_int;
    int y = args[ARG_y].u_int;

    if(args[ARG_angle].u_obj == mp_const_none) {
        self->vector->text(t, Point(x, y));
    } else {
        self->vector->text(t, Point(x, y), mp_obj_get_float(args[ARG_angle].u_obj));
    }

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

    _POLYGON_obj_t *poly = MP_OBJ_TO_PTR2(args[ARG_polygon].u_obj, _POLYGON_obj_t);

    Point origin = Point(args[ARG_origin_x].u_int, args[ARG_origin_y].u_int);

    float angle = mp_obj_get_float(args[ARG_angle].u_obj);

    self->vector->rotate(poly->contour, origin, angle);

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

    _POLYGON_obj_t *poly = MP_OBJ_TO_PTR2(args[ARG_polygon].u_obj, _POLYGON_obj_t);

    Point translate = Point(args[ARG_x].u_int, args[ARG_y].u_int);

    self->vector->translate(poly->contour, translate);

    return mp_const_none;
}

mp_obj_t VECTOR_draw(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    size_t num_polygons = n_args - 1;
    const mp_obj_t *polygons = pos_args + 1;

    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], _VECTOR_obj_t);

    std::vector<pretty_poly::contour_t<picovector_point_type>> contours;

    for(auto i = 0u; i < num_polygons; i++) {
        mp_obj_t poly_obj = polygons[i];

        if(!MP_OBJ_IS_TYPE(poly_obj, &POLYGON_type)) mp_raise_TypeError("draw: Polygon required.");

        _POLYGON_obj_t *poly = MP_OBJ_TO_PTR2(poly_obj, _POLYGON_obj_t);
        contours.emplace_back(poly->contour.points, poly->contour.count);
    }

    self->vector->polygon(contours);

    return mp_const_none;
}

}
