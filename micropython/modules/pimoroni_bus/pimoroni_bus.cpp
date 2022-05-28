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
        mp_print_str(print, "SPIBus()");
    } else {
        mp_print_str(print, "ParallelBus()");
    }
    (void)kind;
}

mp_obj_t SPIPins_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_cs, ARG_dc, ARG_sck, ARG_mosi, ARG_bl };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_cs, MP_ARG_INT, {.u_int = SPI_BG_FRONT_CS} },
        { MP_QSTR_dc, MP_ARG_INT, {.u_int = SPI_DEFAULT_MISO} },
        { MP_QSTR_sck, MP_ARG_INT, {.u_int = SPI_DEFAULT_SCK} },
        { MP_QSTR_mosi, MP_ARG_INT, {.u_int = SPI_DEFAULT_MOSI} },
        { MP_QSTR_bl, MP_ARG_INT, {.u_int = SPI_BG_FRONT_PWM} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _PimoroniBus_obj_t *self = m_new_obj(_PimoroniBus_obj_t);
    self->base.type = &SPIPins_type;
    self->pins = m_new(SPIPins, 1);

    SPIPins *pins = (SPIPins *)self->pins;
    
    pins->spi = ((args[ARG_sck].u_int >> 3) & 0b1) == 0 ? spi0 : spi1;
    pins->cs = (uint)args[ARG_cs].u_int;
    pins->sck = (uint)args[ARG_sck].u_int;
    pins->mosi = (uint)args[ARG_mosi].u_int;
    pins->miso = PIN_UNUSED;
    pins->dc = (uint)args[ARG_dc].u_int;
    pins->bl = args[ARG_bl].u_int == -1 ? PIN_UNUSED : (uint)args[ARG_bl].u_int;

    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t SPISlot_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_slot };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_slot, MP_ARG_INT | MP_ARG_REQUIRED },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _PimoroniBus_obj_t *self = m_new_obj(_PimoroniBus_obj_t);
    self->base.type = &SPIPins_type;
    //self->pins = m_new(SPIPins, 1);

    //SPIPins *pins = (SPIPins *)self->pins;

    SPIPins slot_pins = get_spi_pins((BG_SPI_SLOT)args[ARG_slot].u_int);
    
    self->pins = new(m_new(SPIPins, 1)) SPIPins(slot_pins);

   /* pins->spi = slot_pins.spi;
    pins->cs = slot_pins.cs;
    pins->sck = slot_pins.sck;
    pins->mosi = slot_pins.mosi;
    pins->miso = slot_pins.miso;
    pins->dc = slot_pins.dc;
    pins->bl = slot_pins.bl;*/

    return MP_OBJ_FROM_PTR(self);
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
    self->pins = m_new(ParallelPins, 1);

    ParallelPins *pins = (ParallelPins *)self->pins;
    
    pins->cs = (uint)args[ARG_cs].u_int;
    pins->dc = (uint)args[ARG_dc].u_int;
    pins->wr_sck = (uint)args[ARG_wr_sck].u_int;
    pins->rd_sck = (uint)args[ARG_rd_sck].u_int;
    pins->d0 = (uint)args[ARG_d0].u_int;
    pins->bl = args[ARG_bl].u_int == -1 ? PIN_UNUSED : (uint)args[ARG_bl].u_int;

    return MP_OBJ_FROM_PTR(self);
}

}