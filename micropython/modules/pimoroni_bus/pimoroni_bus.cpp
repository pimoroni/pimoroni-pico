#include "common/pimoroni_bus.hpp"
#include "micropython/modules/util.hpp"
#include <cstdio>

using namespace pimoroni;

extern "C" {
#include "pimoroni_bus.h"
#include "py/mperrno.h"
#include "machine_pin.h"


void PimoroniBus_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    _PimoroniBus_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PimoroniBus_obj_t);
    if(self->base.type == &SPIPins_type) {
        SPIPins *pins = (SPIPins *)self->pins;
        mp_printf(print, "SPIBus(%d, %d, %d, %d, %d, %d, %d)",
            pins->spi == spi0 ? 0 : 1,
            pins->cs,
            pins->sck,
            pins->mosi,
            pins->miso == PIN_UNUSED ? -1 : pins->miso,
            pins->dc   == PIN_UNUSED ? -1 : pins->dc,
            pins->bl   == PIN_UNUSED ? -1 : pins->bl);
    } else {
        ParallelPins *pins = (ParallelPins *)self->pins;
        mp_printf(print, "ParallelBus(%d, %d, %d, %d, %d, %d)",
            pins->cs,
            pins->dc,
            pins->wr_sck,
            pins->rd_sck,
            pins->d0,
            pins->bl == PIN_UNUSED ? -1 : pins->bl);
    }
    (void)kind;
}

mp_obj_t SPIPins_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_cs, ARG_dc, ARG_sck, ARG_mosi, ARG_miso, ARG_bl };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_cs, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(SPI_BG_FRONT_CS)} },
        { MP_QSTR_dc, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(SPI_DEFAULT_MISO)} },
        { MP_QSTR_sck, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(SPI_DEFAULT_SCK)} },
        { MP_QSTR_mosi, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(SPI_DEFAULT_MOSI)} },
        { MP_QSTR_miso, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(PIN_UNUSED)} },
        { MP_QSTR_bl, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(PIN_UNUSED)} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _PimoroniBus_obj_t *self = mp_obj_malloc(_PimoroniBus_obj_t, &SPIPins_type);
    self->pins = new(m_new(SPIPins, 1)) SPIPins{
        ((mp_hal_get_pin_obj(args[ARG_sck].u_obj) >> 3) & 0b1) == 0 ? spi0 : spi1,
        mp_hal_get_pin_obj(args[ARG_cs].u_obj),
        mp_hal_get_pin_obj(args[ARG_sck].u_obj),
        mp_hal_get_pin_obj(args[ARG_mosi].u_obj),
        mp_hal_get_pin_obj(args[ARG_miso].u_obj),
        mp_hal_get_pin_obj(args[ARG_dc].u_obj),
        mp_hal_get_pin_obj(args[ARG_bl].u_obj)
    };

    return MP_OBJ_FROM_PTR(self);
}

// Factory function.
// Returns an "SPIBus" instance for the given SPI slot.
mp_obj_t SPISlot(mp_obj_t slot_in) {
    int slot = mp_obj_get_int(slot_in);

    _PimoroniBus_obj_t *obj = mp_obj_malloc(_PimoroniBus_obj_t, &SPIPins_type);

    SPIPins slot_pins = get_spi_pins((BG_SPI_SLOT)slot);
    
    obj->pins = new(m_new(SPIPins, 1)) SPIPins(slot_pins);

    return MP_OBJ_FROM_PTR(obj);
}

mp_obj_t ParallelPins_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_cs, ARG_dc, ARG_wr_sck, ARG_rd_sck, ARG_d0, ARG_bl };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_cs, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(10)} },
        { MP_QSTR_dc, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(11)} },
        { MP_QSTR_wr_sck, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(12)} },
        { MP_QSTR_rd_sck, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(13)} },
        { MP_QSTR_d0, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(14)} },
        { MP_QSTR_bl, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(2)} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _PimoroniBus_obj_t *self = mp_obj_malloc(_PimoroniBus_obj_t, &ParallelPins_type);
    self->pins = new(m_new(ParallelPins, 1)) ParallelPins{
        mp_hal_get_pin_obj(args[ARG_cs].u_obj),
        mp_hal_get_pin_obj(args[ARG_dc].u_obj),
        mp_hal_get_pin_obj(args[ARG_wr_sck].u_obj),
        mp_hal_get_pin_obj(args[ARG_rd_sck].u_obj),
        mp_hal_get_pin_obj(args[ARG_d0].u_obj),
        args[ARG_bl].u_obj == mp_const_none ? PIN_UNUSED : mp_hal_get_pin_obj(args[ARG_bl].u_obj)
    };

    return MP_OBJ_FROM_PTR(self);
}

}