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
    PicoVector *vector;
} _VECTOR_obj_t;


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
    
    void *mem = m_tracked_calloc(PicoVector::pretty_poly_buffer_size(), sizeof(uint8_t));

    self->vector = m_new_class(PicoVector, graphics->graphics, mem);

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
    enum { ARG_self, ARG_text, ARG_x, ARG_y };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_text, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT }
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

    self->vector->text(t, Point(x, y));

    return mp_const_none;
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
            int(cos(current_angle) * radius),
            int(sin(current_angle) * radius)
        };
    }

    std::vector<pretty_poly::contour_t<int>> contours;
    contours.push_back({points, sides});
    self->vector->polygon(contours, Point(o_x, o_y));

    delete points;

    return mp_const_none;
}

mp_obj_t VECTOR_polygon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    size_t num_tuples = n_args - 1;
    const mp_obj_t *lists = pos_args + 1;

    _VECTOR_obj_t *self = MP_OBJ_TO_PTR2(pos_args[0], _VECTOR_obj_t);

    int offset_x = 0;
    int offset_y = 0;
    float angle = 0.0f;

    // TODO: We should probably convert this to a full-fat argument parser
    // with optional kwargs for translation and rotation
    // this is very, very hacky
    if(mp_obj_is_int(pos_args[1])) {
        offset_x = mp_obj_get_int(pos_args[1]);
        lists++;
        num_tuples--;
    }

    if (mp_obj_is_int(pos_args[2])) {
        offset_y = mp_obj_get_int(pos_args[2]);
        lists++;
        num_tuples--;
    }

    if (mp_obj_is_float(pos_args[3])) {
        angle = mp_obj_get_float(pos_args[3]);
        lists++;
        num_tuples--;
    }

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

    // TODO: This is pretty awful
    // Translating contours could be another operation
    // But it's costly to convert to/from a list of lists of tuples
    // Perhaps polygons should be a purely internal C++ concept
    // And we could have make_polygon(list(tuple, tuple)) ?
    if(angle != 0.0f) {
        self->vector->rotate(contours, Point(offset_x, offset_y), angle);
        self->vector->polygon(contours, Point(0, 0));
    } else {
        self->vector->polygon(contours, Point(offset_x, offset_y));
    }

    for(auto contour : contours) {
        delete contour.points;
    }

    return mp_const_none;
}

}
