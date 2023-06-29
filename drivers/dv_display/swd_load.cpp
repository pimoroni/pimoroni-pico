#include "pico/stdlib.h"
#include "stdio.h"
#include "swd.pio.h"
#include <algorithm>
#ifndef MICROPY_BUILD_TYPE
#define mp_printf(_, ...) printf(__VA_ARGS__);
#else
extern "C" {
#include "py/runtime.h"
}
#endif

static uint pio_offset;
static uint pio_sm;
static const pio_program_t* pio_prog;

PIO swd_pio = pio0;

extern const uint16_t pio_change_exclusive_program(PIO pio, const pio_program* prog);
extern const void pio_remove_exclusive_program(PIO pio);

static void wait_for_idle() {
    uint pull_offset = (pio_prog == &swd_raw_write_program) ? 2 : 
                       (pio_prog == &swd_raw_read_program) ? 0 : 5;
    while (!pio_sm_is_tx_fifo_empty(swd_pio, pio_sm) || swd_pio->sm[pio_sm].addr != pio_offset + pull_offset);
}

static void switch_program(bool read, bool raw = false) {
    wait_for_idle();
    pio_sm_set_enabled(swd_pio, pio_sm, false);
    pio_prog = raw ? (read ? &swd_raw_read_program : &swd_raw_write_program) :
                     (read ? &swd_read_program : &swd_write_program);
    pio_offset = pio_change_exclusive_program(swd_pio, pio_prog);
    if (raw) {
        swd_raw_program_init(swd_pio, pio_sm, pio_offset, 2, 3, read);
    } else {
        swd_program_init(swd_pio, pio_sm, pio_offset, 2, 3, read);
        wait_for_idle();
        swd_pio->irq = 1;
    }
}

static void unload_pio() {
    pio_sm_set_enabled(swd_pio, pio_sm, false);
    pio_remove_exclusive_program(swd_pio);
    pio_sm_unclaim(swd_pio, pio_sm);
}

static bool write_cmd(uint cmd, uint data) {
    if (pio_prog != &swd_write_program) {
        switch_program(false);
    }
    pio_sm_put_blocking(swd_pio, pio_sm, cmd);
    pio_sm_put_blocking(swd_pio, pio_sm, data);
    wait_for_idle();
    if (swd_pio->irq & 0x1) {
        return false;
    }
    return true;
}

static bool write_block(uint addr, const uint* data, uint len_in_words) {
    if (!write_cmd(0x0B, addr)) return false;
    for (uint i = 0; i < len_in_words; ++i) {
        if (!write_cmd(0x3B, *data++)) return false;
    }
    return true;
}

static bool write_reg(uint addr, uint data) {
    return write_block(addr, &data, 1);
}

static bool read_cmd(uint cmd, uint& data) {
    if (pio_prog != &swd_read_program) {
        switch_program(true);
    }
    pio_sm_put_blocking(swd_pio, pio_sm, cmd);
    wait_for_idle();
    if (swd_pio->irq & 0x1) {
        mp_printf(&mp_plat_print, "Read ID failed\n");
        return false;
    }
    data = pio_sm_get_blocking(swd_pio, pio_sm);
    return true;
}

static bool read_reg(uint addr, uint &data) {
    if (!write_cmd(0x0B, addr)) return false;
    if (!read_cmd(0x1F, data)) return false;
    if (!read_cmd(0x3D, data)) return false;
    return true;
}

static void idle() {
    switch_program(false, true);
    pio_sm_put_blocking(swd_pio, pio_sm, 7);
    pio_sm_put_blocking(swd_pio, pio_sm, 0);
}

static bool connect(bool first = true, uint core = 0) {
    if (first) {
        pio_prog = &swd_raw_write_program;
        pio_offset = pio_change_exclusive_program(swd_pio, &swd_raw_write_program);
        pio_sm = pio_claim_unused_sm(swd_pio, true);

        swd_initial_init(swd_pio, pio_sm, 2, 3);

        swd_raw_program_init(swd_pio, pio_sm, pio_offset, 2, 3, false);
    } else {
        switch_program(false, true);
    }

    // Begin transaction: 8 clocks, data low
    mp_printf(&mp_plat_print, "Begin transaction\n");
    pio_sm_put_blocking(swd_pio, pio_sm, 7);
    pio_sm_put_blocking(swd_pio, pio_sm, 0);

    // Go to SWD mode:
    // 8 clocks, data high
    // 0x6209F392, 0x86852D95, 0xE3DDAFE9, 0x19BC0EA2
    // 4 clocks, data low
    // 0x1A
    mp_printf(&mp_plat_print, "SWD Mode\n");
    pio_sm_put_blocking(swd_pio, pio_sm, 8-1);
    pio_sm_put_blocking(swd_pio, pio_sm, 0xFF);

    mp_printf(&mp_plat_print, "Tag\n");
    pio_sm_put_blocking(swd_pio, pio_sm, 32*4+4+8-1);
    pio_sm_put_blocking(swd_pio, pio_sm, 0x6209F392);
    pio_sm_put_blocking(swd_pio, pio_sm, 0x86852D95);
    pio_sm_put_blocking(swd_pio, pio_sm, 0xE3DDAFE9);
    pio_sm_put_blocking(swd_pio, pio_sm, 0x19BC0EA2);
    pio_sm_put_blocking(swd_pio, pio_sm, 0x1A0);

    // Line Reset: 50 high, 8 low
    mp_printf(&mp_plat_print, "Line Reset\n");
    pio_sm_put_blocking(swd_pio, pio_sm, 58-1);
    pio_sm_put_blocking(swd_pio, pio_sm, 0xFFFFFFFF);
    pio_sm_put_blocking(swd_pio, pio_sm, 0x003FFFF);

    mp_printf(&mp_plat_print, "Target Select\n"); // Must ignore error response
    wait_for_idle();
    pio_sm_set_enabled(swd_pio, pio_sm, false);
    pio_prog = &swd_write_ignore_error_program;
    pio_offset = pio_change_exclusive_program(swd_pio, pio_prog);
    swd_program_init(swd_pio, pio_sm, pio_offset, 2, 3, false);
    wait_for_idle();
    swd_pio->irq = 1;
    pio_sm_put_blocking(swd_pio, pio_sm, 0x19);
    pio_sm_put_blocking(swd_pio, pio_sm, 0x01002927 | (core << 28));

    mp_printf(&mp_plat_print, "Read ID\n");
    uint id;
    if (!read_cmd(0x25, id)) {
        mp_printf(&mp_plat_print, "Read ID failed\n");
        return false;
    }
    mp_printf(&mp_plat_print, "Received ID: %08x\n", id);

    if (core != 0xf && id != 0x0bc12477) return false;

    mp_printf(&mp_plat_print, "Abort\n");
    if (!write_cmd(0x01, 0x1E)) {
        mp_printf(&mp_plat_print, "Abort failed\n");
        return false;
    }

    mp_printf(&mp_plat_print, "Select\n");
    if (!write_cmd(0x31, 0)) {
        mp_printf(&mp_plat_print, "Select failed\n");
        return false;
    }

    mp_printf(&mp_plat_print, "Ctrl/Stat\n");
    if (!write_cmd(0x29, 0x50000001)) {
        mp_printf(&mp_plat_print, "Ctrl power up failed\n");
        return false;
    }

    uint status;
    if (!read_cmd(0x0D, status)) {
        mp_printf(&mp_plat_print, "Read status on power up failed\n");
        return false;
    }
    mp_printf(&mp_plat_print, "Status: %08x\n", status);
    if ((status & 0xA0000000) != 0xA0000000) {
        mp_printf(&mp_plat_print, "Power up not acknowledged\n");
        return false;
    }

    if (core != 0xf) {
        mp_printf(&mp_plat_print, "Setup memory access\n");
        if (!write_cmd(0x23, 0xA2000052)) {
            mp_printf(&mp_plat_print, "Memory access setup failed\n");
            return false;
        }

        mp_printf(&mp_plat_print, "Halt CPU\n");
        if (!write_reg(0xe000edf0, 0xA05F0003)) {
            mp_printf(&mp_plat_print, "Halt failed\n");
            return false;
        }
    }
    else {
        if (!write_cmd(0x29, 0x00000001)) {
            mp_printf(&mp_plat_print, "Clear reset failed\n");
            return false;
        }
    }

    idle();

    return true;
}

static bool load(uint address, const uint* data, uint len_in_bytes) {
    mp_printf(&mp_plat_print, "Loading %d bytes at %08x\n", len_in_bytes, address);
    idle();

    constexpr uint BLOCK_SIZE = 1024;
    uint block_len_in_words = std::min((BLOCK_SIZE - (address & (BLOCK_SIZE - 1))) >> 2, len_in_bytes >> 2);
    for (uint i = 0; i < len_in_bytes; ) {
        if (!write_block(address + i, &data[i >> 2], block_len_in_words)) {
            mp_printf(&mp_plat_print, "Block write failed\n");
            return false;
        }
        i += block_len_in_words << 2;
        block_len_in_words = std::min(BLOCK_SIZE >> 2, (len_in_bytes - i) >> 2);
    }

#ifdef FULL_VERIFY
    for (uint j = 0; j < len_in_bytes; j += 4) 
#else
    uint j = 0;
#endif
    {
        uint check_data;
        if (!read_reg(address + j, check_data)) {
            mp_printf(&mp_plat_print, "Read failed\n");
            return false;
        }
        if (check_data != data[j >> 2]) {
            mp_printf(&mp_plat_print, "Verify failed at %08x, %08x != %08x\n", address + j, check_data, data[j >> 2]);
            return false;
        }
    } 

    idle();

    return true;
}

static bool start(uint pc, uint sp) {
    idle();

    // Start from boot address, which can be read from ROM at 0x4
    uint rom_pc, rom_sp;
    read_reg(0x0, rom_sp);
    read_reg(0x4, rom_pc);

    mp_printf(&mp_plat_print, "Set PC\n");
    if (!write_reg(0xe000edf8, rom_pc) ||
        !write_reg(0xe000edf4, 0x1000F))
    {
        mp_printf(&mp_plat_print, "Failed to set PC\n");
        return false;
    }
    mp_printf(&mp_plat_print, "Set SP\n");
    if (!write_reg(0xe000edf8, rom_sp) ||
        !write_reg(0xe000edf4, 0x1000D))
    {
        mp_printf(&mp_plat_print, "Failed to set SP\n");
        return false;
    }
    idle();

    // If a PC has been given, go through watchdog boot sequence to start there
    if (pc != 0) {
        uint watchdog_data[4] = {0xb007c0d3, 0x4ff83f2d ^ pc, sp, pc};
        mp_printf(&mp_plat_print, "Setup watchdog\n");
        if (!write_block(0x4005801c, watchdog_data, 4)) {
            mp_printf(&mp_plat_print, "Failed to setup watchdog for reset\n");
        }
    }

#if 0
    uint data;
    write_reg(0xe000edf4, 0x0000F);
    idle();
    read_reg(0xe000edf8, data);
    mp_printf(&mp_plat_print, "PC is %08x\n", data);

    for (int i = 0; i < 16; ++i) {
        write_reg(0xe000edf4, i);
        idle();
        read_reg(0xe000edf8, data);
        mp_printf(&mp_plat_print, "R%d is %08x\n", i, data);
    }

    for (int i = 0; i < 4; ++i) {
        read_reg(0x4005801c + 4*i, data);
        mp_printf(&mp_plat_print, "WD%d is %08x\n", i, data);
    }
#endif

    mp_printf(&mp_plat_print, "Start CPU\n");
    if (!write_reg(0xe000edf0, 0xA05F0001)) {
        mp_printf(&mp_plat_print, "Start failed\n");
        return false;
    }

    idle();
    wait_for_idle();

#if 0
    sleep_ms(10);

    connect(false);
    mp_printf(&mp_plat_print, "Halt CPU\n");
    if (!write_reg(0xe000edf0, 0xA05F0003)) {
        mp_printf(&mp_plat_print, "Halt failed\n");
        return false;
    }

    for (int i = 0; i < 16; ++i) {
        write_reg(0xe000edf4, i);
        idle();
        read_reg(0xe000edf8, data);
        mp_printf(&mp_plat_print, "R%d is %08x\n", i, data);
    }

    for (int i = 0; i < 4; ++i) {
        read_reg(0x4005801c + 4*i, data);
        mp_printf(&mp_plat_print, "WD%d is %08x\n", i, data);
    }
#endif

    return true;
}

static bool swd_reset_internal() {
    gpio_init(2);
    gpio_init(3);
    gpio_disable_pulls(2);
    gpio_pull_up(3);

    bool ok = connect(true, 0xf);
    mp_printf(&mp_plat_print, "Reset %s\n", ok ? "OK" : "Fail");
    return ok;
}

bool swd_reset() {
    bool ok = swd_reset_internal();
    unload_pio();
    return ok;
}

static bool swd_load_program_internal(const uint* addresses, const uint** data, const uint* data_len_in_bytes, uint num_sections, uint pc, uint sp, bool use_xip_as_ram) {
    bool ok = swd_reset_internal();
    if (!ok) {
        return false;
    }

    mp_printf(&mp_plat_print, "Connecting\n");
    ok = connect(false, 0);    

    mp_printf(&mp_plat_print, "Connected core 0 %s\n", ok ? "OK" : "Fail");
    if (!ok) {
        return false;
    }

    if (use_xip_as_ram) {
        mp_printf(&mp_plat_print, "Disable XIP\n");
        if (!write_reg(0x14000000, 0)) {
            mp_printf(&mp_plat_print, "Disable XIP failed\n");
            return false;
        }
    }

    for (uint i = 0; i < num_sections; ++i)
    {
        if (!load(addresses[i], data[i], data_len_in_bytes[i])) {
            mp_printf(&mp_plat_print, "Failed to load section %d\n", i);
            return false;
        }   
    }

    ok = start(pc, sp);

    ok |= connect(false, 1);

    ok |= start(0, 0x20041000);

    return ok;
}

bool swd_load_program(const uint* addresses, const uint** data, const uint* data_len_in_bytes, uint num_sections, uint pc = 0x20000001, uint sp = 0x20042000, bool use_xip_as_ram = false) {
    bool ok = swd_load_program_internal(addresses, data, data_len_in_bytes, num_sections, pc, sp, use_xip_as_ram);
    unload_pio();
    return ok;
}