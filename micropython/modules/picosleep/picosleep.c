#include "py/runtime.h"
#include "sleep.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "hardware/clocks.h"
#include "rosc.h"
#include "hardware/structs/scb.h"

static void sleep_callback(void)
{
    return;
}

static void rtc_sleep_seconds(uint32_t seconds_to_sleep)
{

    // Hangs if we attempt to sleep for 1 second....
    // Guard against this and perform a normal sleep
    if (seconds_to_sleep == 1)
    {
        sleep_ms(1000);
        return;
    }

    int y = 2020, m = 6, d = 5, hour = 15, mins = 45, secs = 0;
    struct tm t = {.tm_year = y - 1900,
                   .tm_mon = m - 1,
                   .tm_mday = d,
                   .tm_hour = hour,
                   .tm_min = mins,
                   .tm_sec = secs};

    t.tm_sec += seconds_to_sleep;
    mktime(&t);

    datetime_t t_alarm = {
        .year = t.tm_year + 1900,
        .month = t.tm_mon + 1,
        .day = t.tm_mday,
        .dotw = t.tm_wday, // 0 is Sunday, so 5 is Friday
        .hour = t.tm_hour,
        .min = t.tm_min,
        .sec = t.tm_sec};

    sleep_goto_sleep_until(&t_alarm, &sleep_callback);
}

void recover_from_sleep(uint scb_orig, uint clock0_orig, uint clock1_orig)
{

    // Re-enable ring Oscillator control
    rosc_write(&rosc_hw->ctrl, ROSC_CTRL_ENABLE_BITS);

    // reset procs back to default
    scb_hw->scr = scb_orig;
    clocks_hw->sleep_en0 = clock0_orig;
    clocks_hw->sleep_en1 = clock1_orig;

    // reset clocks
    clocks_init();
    stdio_init_all();

    return;
}

STATIC mp_obj_t picosleep_seconds(mp_obj_t seconds_obj)
{
    mp_int_t seconds = mp_obj_get_int(seconds_obj);
    stdio_init_all();
    // save values for later
    uint scb_orig = scb_hw->scr;
    uint clock0_orig = clocks_hw->sleep_en0;
    uint clock1_orig = clocks_hw->sleep_en1;

    // crudely reset the clock each time
    // to the value below
    datetime_t t = {
        .year = 2020,
        .month = 06,
        .day = 05,
        .dotw = 5, // 0 is Sunday, so 5 is Friday
        .hour = 15,
        .min = 45,
        .sec = 00};

    // Start the Real time clock
    rtc_init();
    sleep_run_from_xosc();
    rtc_set_datetime(&t);
    rtc_sleep_seconds(seconds);
    recover_from_sleep(scb_orig, clock0_orig, clock1_orig);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(picosleep_seconds_obj, picosleep_seconds);

STATIC const mp_rom_map_elem_t picosleep_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picosleep)},
    {MP_ROM_QSTR(MP_QSTR_seconds), MP_ROM_PTR(&picosleep_seconds_obj)},
};
STATIC MP_DEFINE_CONST_DICT(picosleep_module_globals, picosleep_module_globals_table);

const mp_obj_module_t mp_module_picosleep = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&picosleep_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_picosleep, mp_module_picosleep);