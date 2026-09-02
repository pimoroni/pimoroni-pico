#include "drivers/brushless/brushless.hpp"
#include "drivers/brushless/brushless_cluster.hpp"
#include "common/pimoroni_common.hpp"
#include "micropython/modules/util.hpp"
#include "micropython/modules/pin.hpp"
#include <cstdio>


using namespace pimoroni;
using namespace brushless;

extern "C" {
#include "brushless.h"
#include "py/builtin.h"
#include "float.h"

static void brushless_tuple_or_list(const mp_obj_t &object, mp_obj_t **items, size_t *length) {
    if(mp_obj_is_type(object, &mp_type_list)) {
        mp_obj_list_t *list = MP_OBJ_TO_PTR2(object, mp_obj_list_t);
        *length = list->len;
        *items = list->items;
    }
    else if(mp_obj_is_type(object, &mp_type_tuple)) {
        mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(object, mp_obj_tuple_t);
        *length = tuple->len;
        *items = tuple->items;
    }
    if(*items == nullptr) {
        mp_raise_TypeError(MP_ERROR_TEXT("cannot convert object to a list or tuple of integers, or a single integer"));
    } else if(*length == 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("list or tuple must contain at least one integer"));
    }
}

static uint8_t* brushless_motors_from_items(mp_obj_t *items, size_t length, int motor_count) {
    uint8_t *motors = m_new(uint8_t, length);
    for(size_t i = 0; i < length; i++) {
        int motor = mp_obj_get_int(items[i]);
        if(motor < 0 || motor >= motor_count) {
            m_free(motors);
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("a motor in the list or tuple is out of range. Expected 0 to %d"), motor_count - 1);
        }
        else {
            motors[i] = (uint8_t)motor;
        }
    }
    return motors;
}

// Fetches one pin from a trio or bridge element, range checked
static uint8_t brushless_pin_from_obj(const mp_obj_t &object) {
    int pin = pimoroni_gpio_from_obj(object);
    if(pin < 0 || pin >= (int)NUM_BANK0_GPIOS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("a pin is out of range. Expected 0 to %d"), NUM_BANK0_GPIOS - 1);
    }
    return (uint8_t)pin;
}


/********** Brushless **********/

/***** Variables Struct *****/
typedef struct _Brushless_obj_t {
    mp_obj_base_t base;
    Brushless* brushless;
} _Brushless_obj_t;


/***** Print *****/
void Brushless_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _Brushless_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Brushless_obj_t);
    mp_print_str(print, "Brushless(");

    mp_print_str(print, "pins = (");
    pin_trio pins = self->brushless->pins();
    mp_obj_print_helper(print, mp_obj_new_int(pins.u), PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, mp_obj_new_int(pins.v), PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, mp_obj_new_int(pins.w), PRINT_REPR);
    mp_print_str(print, "), enabled = ");
    mp_obj_print_helper(print, self->brushless->is_enabled() ? mp_const_true : mp_const_false, PRINT_REPR);
    mp_print_str(print, ", duties = (");
    mp_obj_print_helper(print, mp_obj_new_float(self->brushless->u_duty()), PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, mp_obj_new_float(self->brushless->v_duty()), PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, mp_obj_new_float(self->brushless->w_duty()), PRINT_REPR);
    mp_print_str(print, "), freq = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->brushless->frequency()), PRINT_REPR);
    if(self->brushless->direction() == NORMAL_DIR)
        mp_print_str(print, ", direction = NORMAL_DIR");
    else
        mp_print_str(print, ", direction = REVERSED_DIR");

    mp_print_str(print, ")");
}


/***** Constructor *****/
mp_obj_t Brushless_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _Brushless_obj_t *self = nullptr;

    enum { ARG_pins, ARG_direction, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pins, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_direction, MP_ARG_INT, {.u_int = NORMAL_DIR} },
        { MP_QSTR_freq, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Determine what pins this motor will use
    const mp_obj_t object = args[ARG_pins].u_obj;
    mp_obj_t *items = nullptr;
    size_t item_count = 0;
    if(mp_obj_is_type(object, &mp_type_list)) {
        mp_obj_list_t *list = MP_OBJ_TO_PTR2(object, mp_obj_list_t);
        item_count = list->len;
        items = list->items;
    }
    else if(mp_obj_is_type(object, &mp_type_tuple)) {
        mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(object, mp_obj_tuple_t);
        item_count = tuple->len;
        items = tuple->items;
    }

    if(items == nullptr)
        mp_raise_TypeError(MP_ERROR_TEXT("cannot convert object to a trio of pins, or of bridge pair tuples"));
    if(item_count != 3)
        mp_raise_ValueError(MP_ERROR_TEXT("trio must contain three items"));

    bool bridge_form = mp_obj_is_type(items[0], &mp_type_tuple);
    pin_trio trio;
    bridge_trio bridges;
    if(bridge_form) {
        pin_pair pairs[3];
        for(uint8_t leg = 0; leg < 3; leg++) {
            if(!mp_obj_is_type(items[leg], &mp_type_tuple)) {
                mp_raise_ValueError(MP_ERROR_TEXT("cannot convert item to a bridge pair tuple"));
            }
            mp_obj_tuple_t *pair = MP_OBJ_TO_PTR2(items[leg], mp_obj_tuple_t);
            if(pair->len != 2) {
                mp_raise_ValueError(MP_ERROR_TEXT("bridge pair tuple must contain two pins"));
            }
            pairs[leg].first = brushless_pin_from_obj(pair->items[0]);
            pairs[leg].second = brushless_pin_from_obj(pair->items[1]);
        }
        bridges = bridge_trio(pairs[0], pairs[1], pairs[2]);
    }
    else {
        trio.u = brushless_pin_from_obj(items[0]);
        trio.v = brushless_pin_from_obj(items[1]);
        trio.w = brushless_pin_from_obj(items[2]);
    }

    int direction = args[ARG_direction].u_int;
    if(direction < 0 || direction > 1) {
        mp_raise_ValueError(MP_ERROR_TEXT("direction out of range. Expected NORMAL_DIR (0) or REVERSED_DIR (1)"));
    }

    float freq = Brushless::DEFAULT_FREQUENCY;
    if(args[ARG_freq].u_obj != mp_const_none) {
        freq = mp_obj_get_float(args[ARG_freq].u_obj);
    }

    Brushless *brushless;
    if(bridge_form)
        brushless = m_new_class(Brushless, bridges, (Direction)direction, freq);
    else
        brushless = m_new_class(Brushless, trio, (Direction)direction, freq);

    if(!brushless->init()) {
        uint pin_a, pin_b;
        bool conflicted = brushless->pin_conflict(pin_a, pin_b);
        m_del_class(Brushless, brushless);
        if(conflicted) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("pins %d and %d share a PWM slice channel, so would emit the same signal"), pin_a, pin_b);
        }
        mp_raise_ValueError(MP_ERROR_TEXT("freq out of range. Expected 10Hz to 400KHz"));
    }

    self = mp_obj_malloc_with_finaliser(_Brushless_obj_t, &Brushless_type);
    self->brushless = brushless;

    return MP_OBJ_FROM_PTR(self);
}


/***** Destructor ******/
mp_obj_t Brushless___del__(mp_obj_t self_in) {
    _Brushless_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Brushless_obj_t);
    m_del_class(Brushless, self->brushless);
    return mp_const_none;
}


/***** Methods *****/
extern mp_obj_t Brushless_pins(mp_obj_t self_in) {
    _Brushless_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Brushless_obj_t);
    pin_trio pins = self->brushless->pins();

    mp_obj_t tuple[3];
    tuple[0] = mp_obj_new_int(pins.u);
    tuple[1] = mp_obj_new_int(pins.v);
    tuple[2] = mp_obj_new_int(pins.w);
    return mp_obj_new_tuple(3, tuple);
}

extern mp_obj_t Brushless_has_inverses(mp_obj_t self_in) {
    _Brushless_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Brushless_obj_t);
    return self->brushless->has_inverses() ? mp_const_true : mp_const_false;
}

extern mp_obj_t Brushless_enable(mp_obj_t self_in) {
    _Brushless_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Brushless_obj_t);
    self->brushless->enable();
    return mp_const_none;
}

extern mp_obj_t Brushless_disable(mp_obj_t self_in) {
    _Brushless_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Brushless_obj_t);
    self->brushless->disable();
    return mp_const_none;
}

extern mp_obj_t Brushless_is_enabled(mp_obj_t self_in) {
    _Brushless_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Brushless_obj_t);
    return self->brushless->is_enabled() ? mp_const_true : mp_const_false;
}

extern mp_obj_t Brushless_u_duty(mp_obj_t self_in) {
    _Brushless_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Brushless_obj_t);
    return mp_obj_new_float(self->brushless->u_duty());
}

extern mp_obj_t Brushless_v_duty(mp_obj_t self_in) {
    _Brushless_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Brushless_obj_t);
    return mp_obj_new_float(self->brushless->v_duty());
}

extern mp_obj_t Brushless_w_duty(mp_obj_t self_in) {
    _Brushless_obj_t *self = MP_OBJ_TO_PTR2(self_in, _Brushless_obj_t);
    return mp_obj_new_float(self->brushless->w_duty());
}

extern mp_obj_t Brushless_duties(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_u, ARG_v, ARG_w };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_u, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_v, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_w, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Brushless_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Brushless_obj_t);

    float u = mp_obj_get_float(args[ARG_u].u_obj);
    float v = mp_obj_get_float(args[ARG_v].u_obj);
    float w = mp_obj_get_float(args[ARG_w].u_obj);
    self->brushless->duties(u, v, w);

    return mp_const_none;
}

extern mp_obj_t Brushless_frequency(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_freq, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Brushless_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Brushless_obj_t);

    if(n_args <= 1) {
        return mp_obj_new_float(self->brushless->frequency());
    }
    else {
        float freq = mp_obj_get_float(args[ARG_freq].u_obj);

        if(!self->brushless->frequency(freq))
            mp_raise_ValueError(MP_ERROR_TEXT("freq out of range. Expected 10Hz to 400KHz"));
        else
            return mp_const_none;
    }
}

extern mp_obj_t Brushless_direction(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_direction };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_direction, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _Brushless_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _Brushless_obj_t);

    if(n_args <= 1) {
        return mp_obj_new_int((int)self->brushless->direction());
    }
    else {
        int direction = mp_obj_get_int(args[ARG_direction].u_obj);
        if(direction < 0 || direction > 1) {
            mp_raise_ValueError(MP_ERROR_TEXT("direction out of range. Expected NORMAL_DIR (0) or REVERSED_DIR (1)"));
        }
        self->brushless->direction((Direction)direction);
        return mp_const_none;
    }
}


/********** BrushlessCluster **********/

/***** Variables Struct *****/
typedef struct _BrushlessCluster_obj_t {
    mp_obj_base_t base;
    BrushlessCluster* cluster;
} _BrushlessCluster_obj_t;


/***** Print *****/
void BrushlessCluster_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; //Unused input parameter
    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _BrushlessCluster_obj_t);
    mp_print_str(print, "BrushlessCluster(");

    mp_print_str(print, "motors = {");
    uint8_t motor_count = self->cluster->count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
        mp_print_str(print, "\n\t");
        mp_obj_print_helper(print, mp_obj_new_int(motor), PRINT_REPR);
        mp_print_str(print, " = {pins = (");
        pin_trio pins = self->cluster->pins(motor);
        mp_obj_print_helper(print, mp_obj_new_int(pins.u), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_int(pins.v), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_int(pins.w), PRINT_REPR);
        mp_print_str(print, "), enabled = ");
        mp_obj_print_helper(print, self->cluster->is_enabled(motor) ? mp_const_true : mp_const_false, PRINT_REPR);
        mp_print_str(print, ", duties = (");
        mp_obj_print_helper(print, mp_obj_new_float(self->cluster->u_duty(motor)), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_float(self->cluster->v_duty(motor)), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_float(self->cluster->w_duty(motor)), PRINT_REPR);
        mp_print_str(print, "), phase = ");
        mp_obj_print_helper(print, mp_obj_new_float(self->cluster->phase(motor)), PRINT_REPR);
        if(self->cluster->direction(motor) == NORMAL_DIR)
            mp_print_str(print, ", direction = NORMAL_DIR}");
        else
            mp_print_str(print, ", direction = REVERSED_DIR}");
        if(motor < motor_count - 1)
            mp_print_str(print, ", ");
    }
    if(motor_count > 0) {
        mp_print_str(print, "\n");
    }
    mp_print_str(print, "}, freq = ");
    mp_obj_print_helper(print, mp_obj_new_float(self->cluster->frequency()), PRINT_REPR);

    mp_print_str(print, ")");
}


/***** Constructor *****/
mp_obj_t BrushlessCluster_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _BrushlessCluster_obj_t *self = nullptr;

    enum { ARG_pio, ARG_sm, ARG_pins, ARG_direction, ARG_freq, ARG_auto_phase };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pio, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_sm, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pins, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_direction, MP_ARG_INT, {.u_int = NORMAL_DIR} },
        { MP_QSTR_freq, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_auto_phase, MP_ARG_BOOL, {.u_bool = false} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int pio_int = args[ARG_pio].u_int;
    if(pio_int < 0 || pio_int > (int)NUM_PIOS) {
        mp_raise_ValueError(MP_ERROR_TEXT("pio out of range. Expected 0 to 1"));
    }
    PIO pio = pio_int == 0 ? pio0 : pio1;

    int sm = args[ARG_sm].u_int;
    if(sm < 0 || sm > (int)NUM_PIO_STATE_MACHINES) {
        mp_raise_ValueError(MP_ERROR_TEXT("sm out of range. Expected 0 to 3"));
    }

    size_t trio_count = 0;
    pin_trio *trios = nullptr;
    bridge_trio *bridges = nullptr;

    // Determine what pins this cluster will use
    const mp_obj_t object = args[ARG_pins].u_obj;
    mp_obj_t *items = nullptr;
    if(mp_obj_is_type(object, &mp_type_list)) {
        mp_obj_list_t *list = MP_OBJ_TO_PTR2(object, mp_obj_list_t);
        trio_count = list->len;
        items = list->items;
    }
    else if(mp_obj_is_type(object, &mp_type_tuple)) {
        mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR2(object, mp_obj_tuple_t);
        trio_count = tuple->len;
        items = tuple->items;
    }

    if(items == nullptr)
        mp_raise_TypeError(MP_ERROR_TEXT("cannot convert object to a list or tuple of pin trios"));
    else if(trio_count == 0)
        mp_raise_TypeError(MP_ERROR_TEXT("list or tuple must contain at least one trio tuple"));
    else if(trio_count == 3 && pimoroni_obj_is_int_or_gpio(items[0]) && pimoroni_obj_is_int_or_gpio(items[1]) && pimoroni_obj_is_int_or_gpio(items[2])) {
        // A single 3 pin list/tuple was provided
        trios = m_new(pin_trio, 1);
        trio_count = 1;
        trios[0].u = brushless_pin_from_obj(items[0]);
        trios[0].v = brushless_pin_from_obj(items[1]);
        trios[0].w = brushless_pin_from_obj(items[2]);
        if(trios[0].u == trios[0].v || trios[0].v == trios[0].w || trios[0].u == trios[0].w) {
            m_free(trios);
            mp_raise_ValueError(MP_ERROR_TEXT("cannot use the same pin for two phases of a motor"));
        }
    }
    else {
        // Each item is a trio tuple: three pins, or three {signal, inverse} pair tuples
        for(size_t i = 0; i < trio_count; i++) {
            if(!mp_obj_is_type(items[i], &mp_type_tuple)) {
                m_free(trios); m_free(bridges);
                mp_raise_ValueError(MP_ERROR_TEXT("cannot convert item to a trio tuple"));
            }
            mp_obj_tuple_t *trio = MP_OBJ_TO_PTR2(items[i], mp_obj_tuple_t);
            if(trio->len != 3) {
                m_free(trios); m_free(bridges);
                mp_raise_ValueError(MP_ERROR_TEXT("trio tuple must contain three items"));
            }

            // The first trio's shape selects the pin form for the whole cluster
            if(trios == nullptr && bridges == nullptr) {
                if(mp_obj_is_type(trio->items[0], &mp_type_tuple)) {
                    bridges = m_new(bridge_trio, trio_count);
                }
                else {
                    trios = m_new(pin_trio, trio_count);
                }
            }

            if(trios != nullptr) {
                trios[i].u = brushless_pin_from_obj(trio->items[0]);
                trios[i].v = brushless_pin_from_obj(trio->items[1]);
                trios[i].w = brushless_pin_from_obj(trio->items[2]);
                if(trios[i].u == trios[i].v || trios[i].v == trios[i].w || trios[i].u == trios[i].w) {
                    m_free(trios);
                    mp_raise_ValueError(MP_ERROR_TEXT("cannot use the same pin for two phases of a motor"));
                }
            }
            else {
                pin_pair pairs[3];
                for(uint8_t leg = 0; leg < 3; leg++) {
                    if(!mp_obj_is_type(trio->items[leg], &mp_type_tuple)) {
                        m_free(bridges);
                        mp_raise_ValueError(MP_ERROR_TEXT("cannot convert item to a bridge pair tuple"));
                    }
                    mp_obj_tuple_t *pair = MP_OBJ_TO_PTR2(trio->items[leg], mp_obj_tuple_t);
                    if(pair->len != 2) {
                        m_free(bridges);
                        mp_raise_ValueError(MP_ERROR_TEXT("bridge pair tuple must contain two pins"));
                    }
                    pairs[leg].first = brushless_pin_from_obj(pair->items[0]);
                    pairs[leg].second = brushless_pin_from_obj(pair->items[1]);
                    if(pairs[leg].first == pairs[leg].second) {
                        m_free(bridges);
                        mp_raise_ValueError(MP_ERROR_TEXT("cannot use the same pin for a bridge signal and inverse"));
                    }
                }
                bridges[i] = bridge_trio(pairs[0], pairs[1], pairs[2]);
            }
        }
    }

    int direction = args[ARG_direction].u_int;
    if(direction < 0 || direction > 1) {
        m_free(trios); m_free(bridges);
        mp_raise_ValueError(MP_ERROR_TEXT("direction out of range. Expected NORMAL_DIR (0) or REVERSED_DIR (1)"));
    }

    float freq = BrushlessState::DEFAULT_FREQUENCY;
    if(args[ARG_freq].u_obj != mp_const_none) {
        freq = mp_obj_get_float(args[ARG_freq].u_obj);
    }

    bool auto_phase = args[ARG_auto_phase].u_bool;

    BrushlessCluster *cluster;
    if(bridges != nullptr) {
        cluster = m_new_class(BrushlessCluster, pio, sm, bridges, trio_count, (Direction)direction, freq, auto_phase);
        m_free(bridges);
    }
    else {
        cluster = m_new_class(BrushlessCluster, pio, sm, trios, trio_count, (Direction)direction, freq, auto_phase);
        m_free(trios);
    }

    if(!cluster->init()) {
        bool reachable = cluster->pins_reachable();
        bool available = cluster->pins_available();
        m_del_class(BrushlessCluster, cluster);
        if(!reachable) {
            mp_raise_ValueError(MP_ERROR_TEXT("the pins do not fit the 32 pin range this PIO can currently reach"));
        }
        if(!available) {
            mp_raise_ValueError(MP_ERROR_TEXT("the pins overlap a range already driven by another cluster on this PIO"));
        }
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("unable to allocate the hardware resources needed to initialise this BrushlessCluster. Try running `import gc` followed by `gc.collect()` before creating it"));
    }

    self = mp_obj_malloc_with_finaliser(_BrushlessCluster_obj_t, &BrushlessCluster_type);
    self->cluster = cluster;

    return MP_OBJ_FROM_PTR(self);
}


/***** Destructor ******/
mp_obj_t BrushlessCluster___del__(mp_obj_t self_in) {
    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _BrushlessCluster_obj_t);
    m_del_class(BrushlessCluster, self->cluster);
    return mp_const_none;
}


/***** Methods *****/
extern mp_obj_t BrushlessCluster_count(mp_obj_t self_in) {
    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _BrushlessCluster_obj_t);
    return mp_obj_new_int(self->cluster->count());
}

extern mp_obj_t BrushlessCluster_pins(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motor };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motor, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _BrushlessCluster_obj_t);

    int motor = args[ARG_motor].u_int;
    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError(MP_ERROR_TEXT("this cluster does not have any motors"));
    else if(motor < 0 || motor >= motor_count)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
    else {
        pin_trio pins = self->cluster->pins((uint)motor);

        mp_obj_t tuple[3];
        tuple[0] = mp_obj_new_int(pins.u);
        tuple[1] = mp_obj_new_int(pins.v);
        tuple[2] = mp_obj_new_int(pins.w);
        return mp_obj_new_tuple(3, tuple);
    }

    return mp_const_none;
}

extern mp_obj_t BrushlessCluster_has_inverses(mp_obj_t self_in) {
    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _BrushlessCluster_obj_t);
    return self->cluster->has_inverses() ? mp_const_true : mp_const_false;
}

extern mp_obj_t BrushlessCluster_enable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motors, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motors, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _BrushlessCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError(MP_ERROR_TEXT("this cluster does not have any motors"));
    else {
        // Determine what motor(s) to enable
        const mp_obj_t object = args[ARG_motors].u_obj;
        if(mp_obj_is_int(object)) {
            int motor = mp_obj_get_int(object);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                self->cluster->enable((uint)motor, args[ARG_load].u_bool);
        }
        else {
            size_t length = 0;
            mp_obj_t *items = nullptr;
            brushless_tuple_or_list(object, &items, &length);

            // Create and populate a local array of motor indices
            uint8_t *motors = brushless_motors_from_items(items, length, motor_count);

            self->cluster->enable(motors, length, args[ARG_load].u_bool);
            m_free(motors);
        }
    }

    return mp_const_none;
}

extern mp_obj_t BrushlessCluster_enable_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _BrushlessCluster_obj_t);
    self->cluster->enable_all(args[ARG_load].u_bool);

    return mp_const_none;
}

extern mp_obj_t BrushlessCluster_disable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motors, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motors, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _BrushlessCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError(MP_ERROR_TEXT("this cluster does not have any motors"));
    else {
        // Determine what motor(s) to disable
        const mp_obj_t object = args[ARG_motors].u_obj;
        if(mp_obj_is_int(object)) {
            int motor = mp_obj_get_int(object);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                self->cluster->disable((uint)motor, args[ARG_load].u_bool);
        }
        else {
            size_t length = 0;
            mp_obj_t *items = nullptr;
            brushless_tuple_or_list(object, &items, &length);

            // Create and populate a local array of motor indices
            uint8_t *motors = brushless_motors_from_items(items, length, motor_count);

            self->cluster->disable(motors, length, args[ARG_load].u_bool);
            m_free(motors);
        }
    }

    return mp_const_none;
}

extern mp_obj_t BrushlessCluster_disable_all(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _BrushlessCluster_obj_t);
    self->cluster->disable_all(args[ARG_load].u_bool);

    return mp_const_none;
}

extern mp_obj_t BrushlessCluster_is_enabled(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motor };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motor, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _BrushlessCluster_obj_t);

    int motor = args[ARG_motor].u_int;
    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError(MP_ERROR_TEXT("this cluster does not have any motors"));
    else if(motor < 0 || motor >= motor_count)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
    else
        return self->cluster->is_enabled((uint)motor) ? mp_const_true : mp_const_false;

    return mp_const_none;
}

// The three per-leg duty getters share this body
static mp_obj_t brushless_leg_duty(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args, uint8_t leg) {
    enum { ARG_self, ARG_motor };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motor, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _BrushlessCluster_obj_t);

    int motor = args[ARG_motor].u_int;
    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError(MP_ERROR_TEXT("this cluster does not have any motors"));
    else if(motor < 0 || motor >= motor_count)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
    else {
        switch(leg) {
            case 0: return mp_obj_new_float(self->cluster->u_duty((uint)motor));
            case 1: return mp_obj_new_float(self->cluster->v_duty((uint)motor));
            default: return mp_obj_new_float(self->cluster->w_duty((uint)motor));
        }
    }

    return mp_const_none;
}

extern mp_obj_t BrushlessCluster_u_duty(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return brushless_leg_duty(n_args, pos_args, kw_args, 0);
}

extern mp_obj_t BrushlessCluster_v_duty(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return brushless_leg_duty(n_args, pos_args, kw_args, 1);
}

extern mp_obj_t BrushlessCluster_w_duty(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return brushless_leg_duty(n_args, pos_args, kw_args, 2);
}

extern mp_obj_t BrushlessCluster_duties(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motors, ARG_u, ARG_v, ARG_w, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motors, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_u, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_v, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_w, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _BrushlessCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError(MP_ERROR_TEXT("this cluster does not have any motors"));
    else {
        float u = mp_obj_get_float(args[ARG_u].u_obj);
        float v = mp_obj_get_float(args[ARG_v].u_obj);
        float w = mp_obj_get_float(args[ARG_w].u_obj);

        // Determine what motor(s) to modify
        const mp_obj_t object = args[ARG_motors].u_obj;
        if(mp_obj_is_int(object)) {
            int motor = mp_obj_get_int(object);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                self->cluster->duties((uint)motor, u, v, w, args[ARG_load].u_bool);
        }
        else {
            size_t length = 0;
            mp_obj_t *items = nullptr;
            brushless_tuple_or_list(object, &items, &length);

            // Create and populate a local array of motor indices
            uint8_t *motors = brushless_motors_from_items(items, length, motor_count);

            self->cluster->duties(motors, length, u, v, w, args[ARG_load].u_bool);
            m_free(motors);
        }
    }

    return mp_const_none;
}

extern mp_obj_t BrushlessCluster_all_to_duties(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_u, ARG_v, ARG_w, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_u, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_v, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_w, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _BrushlessCluster_obj_t);

    float u = mp_obj_get_float(args[ARG_u].u_obj);
    float v = mp_obj_get_float(args[ARG_v].u_obj);
    float w = mp_obj_get_float(args[ARG_w].u_obj);
    self->cluster->all_to_duties(u, v, w, args[ARG_load].u_bool);

    return mp_const_none;
}

extern mp_obj_t BrushlessCluster_phase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motors, ARG_phase, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motors, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_phase, MP_ARG_OBJ, { .u_obj = mp_const_none }},
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _BrushlessCluster_obj_t);

    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError(MP_ERROR_TEXT("this cluster does not have any motors"));
    else {
        if(n_args <= 2) {
            int motor = mp_obj_get_int(args[ARG_motors].u_obj);
            if(motor < 0 || motor >= motor_count)
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
            else
                return mp_obj_new_float(self->cluster->phase((uint)motor));
        }
        else {
            // Determine what motor(s) to modify
            const mp_obj_t object = args[ARG_motors].u_obj;
            if(mp_obj_is_int(object)) {
                int motor = mp_obj_get_int(object);
                if(motor < 0 || motor >= motor_count)
                    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
                else {
                    float phase = mp_obj_get_float(args[ARG_phase].u_obj);
                    self->cluster->phase((uint)motor, phase, args[ARG_load].u_bool);
                }
            }
            else {
                size_t length = 0;
                mp_obj_t *items = nullptr;
                brushless_tuple_or_list(object, &items, &length);

                // Create and populate a local array of motor indices
                uint8_t *motors = brushless_motors_from_items(items, length, motor_count);

                float phase = mp_obj_get_float(args[ARG_phase].u_obj);
                self->cluster->phase(motors, length, phase, args[ARG_load].u_bool);
                m_free(motors);
            }
        }
    }

    return mp_const_none;
}

extern mp_obj_t BrushlessCluster_all_to_phase(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_phase, ARG_load };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_phase, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_load, MP_ARG_BOOL, { .u_bool = true }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _BrushlessCluster_obj_t);

    float phase = mp_obj_get_float(args[ARG_phase].u_obj);
    self->cluster->all_to_phase(phase, args[ARG_load].u_bool);

    return mp_const_none;
}

extern mp_obj_t BrushlessCluster_frequency(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_freq, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _BrushlessCluster_obj_t);

    if(n_args <= 1) {
        return mp_obj_new_float(self->cluster->frequency());
    }
    else {
        float freq = mp_obj_get_float(args[ARG_freq].u_obj);

        if(!self->cluster->frequency(freq))
            mp_raise_ValueError(MP_ERROR_TEXT("freq out of range. Expected 10Hz to 200KHz"));
        else
            return mp_const_none;
    }
}

extern mp_obj_t BrushlessCluster_direction(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_motor, ARG_direction };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_motor, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_direction, MP_ARG_OBJ, { .u_obj = mp_const_none }},
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(args[ARG_self].u_obj, _BrushlessCluster_obj_t);

    int motor = args[ARG_motor].u_int;
    int motor_count = (int)self->cluster->count();
    if(motor_count == 0)
        mp_raise_ValueError(MP_ERROR_TEXT("this cluster does not have any motors"));
    else if(motor < 0 || motor >= motor_count)
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("motor out of range. Expected 0 to %d"), motor_count - 1);
    else {
        if(n_args <= 2) {
            return mp_obj_new_int((int)self->cluster->direction((uint)motor));
        }
        else {
            int direction = mp_obj_get_int(args[ARG_direction].u_obj);
            if(direction < 0 || direction > 1) {
                mp_raise_ValueError(MP_ERROR_TEXT("direction out of range. Expected NORMAL_DIR (0) or REVERSED_DIR (1)"));
            }
            self->cluster->direction((uint)motor, (Direction)direction);
        }
    }
    return mp_const_none;
}

extern mp_obj_t BrushlessCluster_load(mp_obj_t self_in) {
    _BrushlessCluster_obj_t *self = MP_OBJ_TO_PTR2(self_in, _BrushlessCluster_obj_t);
    self->cluster->load();
    return mp_const_none;
}
}
