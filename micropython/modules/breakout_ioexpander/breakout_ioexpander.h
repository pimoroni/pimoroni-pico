// Include MicroPython API.
#include "py/runtime.h"

enum {
    IOE_PIN_IN      = 0b00010,
    IOE_PIN_IN_PU   = 0b10000,
    IOE_PIN_OUT     = 0b00001,
    IOE_PIN_OD      = 0b00011,
    IOE_PIN_PWM     = 0b00101,
    IOE_PIN_ADC     = 0b01010,

    IOE_NUM_PINS    = 14,

    IOE_LOW         = 0,
    IOE_HIGH        = 1,
};

/***** Extern of Class Definition *****/
extern const mp_obj_type_t breakout_ioexpander_BreakoutIOExpander_type;

/***** Extern of Class Methods *****/
extern mp_obj_t BreakoutIOExpander_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern mp_obj_t BreakoutIOExpander_get_chip_id(mp_obj_t self_in);
extern mp_obj_t BreakoutIOExpander_set_address(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutIOExpander_get_adc_vref(mp_obj_t self_in);
extern mp_obj_t BreakoutIOExpander_set_adc_vref(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutIOExpander_enable_interrupt_out(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutIOExpander_disable_interrupt_out(mp_obj_t self_in);
extern mp_obj_t BreakoutIOExpander_get_interrupt_flag(mp_obj_t self_in);
extern mp_obj_t BreakoutIOExpander_clear_interrupt_flag(mp_obj_t self_in);
extern mp_obj_t BreakoutIOExpander_set_pin_interrupt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutIOExpander_pwm_load(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutIOExpander_pwm_loading(mp_obj_t self_in);
extern mp_obj_t BreakoutIOExpander_pwm_clear(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutIOExpander_pwm_clearing(mp_obj_t self_in);
extern mp_obj_t BreakoutIOExpander_set_pwm_control(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutIOExpander_set_pwm_period(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutIOExpander_get_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutIOExpander_set_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutIOExpander_input(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutIOExpander_input_as_voltage(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutIOExpander_output(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutIOExpander_setup_rotary_encoder(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
extern mp_obj_t BreakoutIOExpander_read_rotary_encoder(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);