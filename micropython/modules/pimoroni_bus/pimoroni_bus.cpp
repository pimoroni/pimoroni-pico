#include "common/pimoroni_bus.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>

using namespace pimoroni;

extern "C" {
#include "pimoroni_bus.h"
#include "py/mperrno.h"


void PimoroniBus_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    _PimoroniBus_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PimoroniBus_obj_t);
    if(self->base.type == &SPIPins_type) {
        SPIPins *pins = (SPIPins *)self->pins;
        mp_print_str(print, "SPIBus(");
        mp_obj_print_helper(print, mp_obj_new_int((pins->spi == spi0) ? 0 : 1), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_int(pins->cs), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_int(pins->sck), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_int(pins->mosi), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_int(pins->miso == PIN_UNUSED ? -1 : pins->miso), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_int(pins->dc == PIN_UNUSED ? -1 : pins->dc), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_int(pins->bl == PIN_UNUSED ? -1 : pins->bl), PRINT_REPR);
        mp_print_str(print, ")");
    } else {
        ParallelPins *pins = (ParallelPins *)self->pins;
        mp_print_str(print, "ParallelBus(");
        mp_obj_print_helper(print, mp_obj_new_int(pins->cs), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_int(pins->dc), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_int(pins->wr_sck), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_int(pins->rd_sck), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_int(pins->d0), PRINT_REPR);
        mp_print_str(print, ", ");
        mp_obj_print_helper(print, mp_obj_new_int(pins->bl == PIN_UNUSED ? -1 : pins->bl), PRINT_REPR);
        mp_print_str(print, ")");
    }
    (void)kind;
}

mp_obj_t SPIPins_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_cs, ARG_dc, ARG_sck, ARG_mosi, ARG_miso, ARG_bl };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_cs, MP_ARG_INT, {.u_int = SPI_BG_FRONT_CS} },
        { MP_QSTR_dc, MP_ARG_INT, {.u_int = SPI_DEFAULT_MISO} },
        { MP_QSTR_sck, MP_ARG_INT, {.u_int = SPI_DEFAULT_SCK} },
        { MP_QSTR_mosi, MP_ARG_INT, {.u_int = SPI_DEFAULT_MOSI} },
        { MP_QSTR_miso, MP_ARG_INT, {.u_int = PIN_UNUSED} },
        { MP_QSTR_bl, MP_ARG_INT, {.u_int = PIN_UNUSED} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _PimoroniBus_obj_t *self = m_new_obj(_PimoroniBus_obj_t);
    self->base.type = &SPIPins_type;
    self->pins = new(m_new(SPIPins, 1)) SPIPins{
        ((args[ARG_sck].u_int >> 3) & 0b1) == 0 ? spi0 : spi1,
        (uint)args[ARG_cs].u_int,
        (uint)args[ARG_sck].u_int,
        (uint)args[ARG_mosi].u_int,
        (uint)args[ARG_miso].u_int,
        (uint)args[ARG_dc].u_int,
        (uint)args[ARG_bl].u_int
    };

    return MP_OBJ_FROM_PTR(self);
}

// Factory function.
// Returns an "SPIBus" instance for the given SPI slot.
mp_obj_t SPISlot(mp_obj_t slot_in) {
    int slot = mp_obj_get_int(slot_in);

    _PimoroniBus_obj_t *obj = m_new_obj(_PimoroniBus_obj_t);
    obj->base.type = &SPIPins_type;

    SPIPins slot_pins = get_spi_pins((BG_SPI_SLOT)slot);
    
    obj->pins = new(m_new(SPIPins, 1)) SPIPins(slot_pins);

    return MP_OBJ_FROM_PTR(obj);
}

mp_obj_t ParallelPins_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_cs, ARG_dc, ARG_wr_sck, ARG_rd_sck, ARG_d0, ARG_bl };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_cs, MP_ARG_INT, {.u_int = 10} },
        { MP_QSTR_dc, MP_ARG_INT, {.u_int = 11} },
        { MP_QSTR_wr_sck, MP_ARG_INT, {.u_int = 12} },
        { MP_QSTR_rd_sck, MP_ARG_INT, {.u_int = 13} },
        { MP_QSTR_d0, MP_ARG_INT, {.u_int = 14} },
        { MP_QSTR_bl, MP_ARG_INT, {.u_int = 2} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _PimoroniBus_obj_t *self = m_new_obj(_PimoroniBus_obj_t);
    self->base.type = &ParallelPins_type;
    self->pins = new(m_new(ParallelPins, 1)) ParallelPins{
        (uint)args[ARG_cs].u_int,
        (uint)args[ARG_dc].u_int,
        (uint)args[ARG_wr_sck].u_int,
        (uint)args[ARG_rd_sck].u_int,
        (uint)args[ARG_d0].u_int,
        args[ARG_bl].u_int == -1 ? PIN_UNUSED : (uint)args[ARG_bl].u_int
    };

    return MP_OBJ_FROM_PTR(self);
}

}