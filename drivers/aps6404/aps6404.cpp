#include <algorithm>
#include "aps6404.hpp"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include "aps6404.pio.h"
#ifndef MICROPY_BUILD_TYPE
#define mp_printf(_, ...) printf(__VA_ARGS__);
#else
extern "C" {
#include "py/runtime.h"
}
#endif

static const pio_program* pio_prog[2] = {nullptr, nullptr};
static uint16_t pio_offset[2] = {0xffff, 0xffff};

const void pio_remove_exclusive_program(PIO pio) {
    uint8_t pio_index = pio == pio0 ? 0 : 1;
    const pio_program* current_program = pio_prog[pio_index];
    uint16_t current_offset = pio_offset[pio_index];
    if(current_program) {
        pio_remove_program(pio, current_program, current_offset);
        pio_prog[pio_index] = nullptr;
        pio_offset[pio_index] = 0xffff;
    }
}

const uint16_t pio_change_exclusive_program(PIO pio, const pio_program* prog) {
    pio_remove_exclusive_program(pio);
    uint8_t pio_index = pio == pio0 ? 0 : 1;
    pio_prog[pio_index] = prog;
    pio_offset[pio_index] = pio_add_program(pio, prog);
    return pio_offset[pio_index];
};

namespace pimoroni {
    APS6404::APS6404(uint pin_csn, uint pin_d0, PIO pio)
                : pin_csn(pin_csn)
                , pin_d0(pin_d0)
                , pio(pio)
    {
        // Initialize data pins
        for (int i = 0; i < 4; ++i) {
            gpio_init(pin_d0 + i);
            gpio_disable_pulls(pin_d0 + i);
        }

        pio_sm = pio_claim_unused_sm(pio, true);

        // Claim DMA channels
        dma_channel = dma_claim_unused_channel(true);
        read_cmd_dma_channel = dma_claim_unused_channel(true);
    }

    void APS6404::init() {
        pio_sm_set_enabled(pio, pio_sm, false);

        pio_offset = pio_change_exclusive_program(pio, &sram_reset_program);
        aps6404_reset_program_init(pio, pio_sm, pio_offset, pin_csn, pin_d0);

        sleep_us(200);
        pio_sm_put_blocking(pio, pio_sm, 0x00000007u);
        pio_sm_put_blocking(pio, pio_sm, 0x66000000u);
        pio_sm_put_blocking(pio, pio_sm, 0x00000007u);
        pio_sm_put_blocking(pio, pio_sm, 0x99000000u);
        pio_sm_put_blocking(pio, pio_sm, 0x00000007u);
        pio_sm_put_blocking(pio, pio_sm, 0x35000000u);
        sleep_us(500);

        adjust_clock();
    }

    void APS6404::set_qpi() {
        pio_sm_set_enabled(pio, pio_sm, false);

        pio_offset = pio_change_exclusive_program(pio, &sram_reset_program);
        aps6404_reset_program_init(pio, pio_sm, pio_offset, pin_csn, pin_d0);
        pio_sm_put_blocking(pio, pio_sm, 0x00000007u);
        pio_sm_put_blocking(pio, pio_sm, 0x35000000u);

        while (!pio_sm_is_tx_fifo_empty(pio, pio_sm) || pio->sm[pio_sm].addr != pio_offset);

        adjust_clock();
    }

    void APS6404::set_spi() {
        pio_sm_set_enabled(pio, pio_sm, false);

        pio_offset = pio_change_exclusive_program(pio, &sram_reset_qpi_program);
        aps6404_program_init(pio, pio_sm, pio_offset, pin_csn, pin_d0, false, false, true);
        pio_sm_put_blocking(pio, pio_sm, 0x00000001u);
        pio_sm_put_blocking(pio, pio_sm, 0xF5000000u);
    }

    void APS6404::adjust_clock() {
        pio_sm_set_enabled(pio, pio_sm, false);

        if (clock_get_hz(clk_sys) > 296000000) {
            pio_offset = pio_change_exclusive_program(pio, &sram_fast_program);
            aps6404_program_init(pio, pio_sm, pio_offset, pin_csn, pin_d0, false, true, false);
        }
        else if (clock_get_hz(clk_sys) < 130000000) {
            pio_offset = pio_change_exclusive_program(pio, &sram_slow_program);
            aps6404_program_init(pio, pio_sm, pio_offset, pin_csn, pin_d0, true, false, false);
        }
        else {
            pio_offset = pio_change_exclusive_program(pio, &sram_program);
            aps6404_program_init(pio, pio_sm, pio_offset, pin_csn, pin_d0, false, false, false);
        }

        last_cmd_was_write = false;
    }

    void APS6404::write(uint32_t addr, uint32_t* data, uint32_t len_in_bytes) {
        if (!last_cmd_was_write) {
            dma_channel_config c = dma_channel_get_default_config(dma_channel);
            channel_config_set_read_increment(&c, true);
            channel_config_set_write_increment(&c, false);
            channel_config_set_dreq(&c, pio_get_dreq(pio, pio_sm, true));
            channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
            channel_config_set_bswap(&c, true);
            
            wait_for_finish_blocking();
            setup_cmd_buffer_dma(true);
            last_cmd_was_write = true;

            dma_channel_configure(
                dma_channel, &c,
                &pio->txf[pio_sm],
                nullptr,
                1,
                false
            );
        }

        for (int len = len_in_bytes, page_len = std::min(PAGE_SIZE, len); 
             len > 0; 
             addr += page_len, data += page_len >> 2, len -= page_len, page_len = std::min(PAGE_SIZE, len))
        {
            wait_for_finish_blocking();
            hw_set_bits(&dma_hw->ch[dma_channel].al1_ctrl, DMA_CH0_CTRL_TRIG_INCR_READ_BITS);

            pio_sm_put_blocking(pio, pio_sm, (page_len << 1) - 1);
            pio_sm_put_blocking(pio, pio_sm, 0x38000000u | addr);
            pio_sm_put_blocking(pio, pio_sm, pio_offset + sram_offset_do_write);

            dma_channel_transfer_from_buffer_now(dma_channel, data, (page_len >> 2) + 1);
        }
    }

    void APS6404::write_repeat(uint32_t addr, uint32_t data, uint32_t len_in_bytes) {
        if (!last_cmd_was_write) {
            dma_channel_config c = dma_channel_get_default_config(dma_channel);
            channel_config_set_read_increment(&c, false);
            channel_config_set_write_increment(&c, false);
            channel_config_set_dreq(&c, pio_get_dreq(pio, pio_sm, true));
            channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
            channel_config_set_bswap(&c, true);
            
            wait_for_finish_blocking();
            setup_cmd_buffer_dma(true);
            last_cmd_was_write = true;

            dma_channel_configure(
                dma_channel, &c,
                &pio->txf[pio_sm],
                nullptr,
                1,
                false
            );
        }

        for (int len = len_in_bytes, page_len = std::min(PAGE_SIZE, len); 
             len > 0; 
             addr += page_len, len -= page_len, page_len = std::min(PAGE_SIZE, len))
        {
            wait_for_finish_blocking();
            hw_clear_bits(&dma_hw->ch[dma_channel].al1_ctrl, DMA_CH0_CTRL_TRIG_INCR_READ_BITS);
            repeat_data = data;

            pio_sm_put_blocking(pio, pio_sm, (page_len << 1) - 1);
            pio_sm_put_blocking(pio, pio_sm, 0x38000000u | addr);
            pio_sm_put_blocking(pio, pio_sm, pio_offset + sram_offset_do_write);

            dma_channel_transfer_from_buffer_now(dma_channel, &repeat_data, (page_len >> 2) + 1);
        }
    }

    void APS6404::read(uint32_t addr, uint32_t* read_buf, uint32_t len_in_words) {
        start_read(read_buf, len_in_words);

        uint32_t first_page_len = (PAGE_SIZE - (addr & (PAGE_SIZE - 1)));

        if (first_page_len >= len_in_words << 2) {
            pio_sm_put_blocking(pio, pio_sm, (len_in_words * 8) - 4);
            pio_sm_put_blocking(pio, pio_sm, 0xeb000000u | addr);
            pio_sm_put_blocking(pio, pio_sm, pio_offset + sram_offset_do_read);
        }
        else {
            // Must always reset the cmd buffer DMA because writes use the same DREQ
            setup_cmd_buffer_dma();
            uint32_t* cmd_buf = add_read_to_cmd_buffer(multi_read_cmd_buffer, addr, len_in_words);
            dma_channel_transfer_from_buffer_now(read_cmd_dma_channel, multi_read_cmd_buffer, cmd_buf - multi_read_cmd_buffer);
        }
    }

    void APS6404::multi_read(uint32_t* addresses, uint32_t* lengths, uint32_t num_reads, uint32_t* read_buf, int chain_channel) {
        uint32_t total_len = 0;
        uint32_t* cmd_buf = multi_read_cmd_buffer;
        for (uint32_t i = 0; i < num_reads; ++i) {
            total_len += lengths[i];
            cmd_buf = add_read_to_cmd_buffer(cmd_buf, addresses[i], lengths[i]);
        }

        start_read(read_buf, total_len, chain_channel);
        setup_cmd_buffer_dma();

        dma_channel_transfer_from_buffer_now(read_cmd_dma_channel, multi_read_cmd_buffer, cmd_buf - multi_read_cmd_buffer);
    }

    void APS6404::start_read(uint32_t* read_buf, uint32_t total_len_in_words, int chain_channel) {
        dma_channel_config c = dma_channel_get_default_config(dma_channel);
        channel_config_set_read_increment(&c, false);
        channel_config_set_write_increment(&c, true);
        channel_config_set_dreq(&c, pio_get_dreq(pio, pio_sm, false));
        channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
        channel_config_set_bswap(&c, true);
        if (chain_channel >= 0) {
            channel_config_set_chain_to(&c, chain_channel);
        }
        
        last_cmd_was_write = false;
        wait_for_finish_blocking();

        dma_channel_configure(
            dma_channel, &c,
            read_buf,
            &pio->rxf[pio_sm],
            total_len_in_words,
            true
        );
    }

    void APS6404::setup_cmd_buffer_dma(bool clear) {
        dma_channel_config c = dma_channel_get_default_config(read_cmd_dma_channel);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, false);
        if (!clear) channel_config_set_dreq(&c, pio_get_dreq(pio, pio_sm, true));
        channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
        
        dma_channel_configure(
            read_cmd_dma_channel, &c,
            &pio->txf[pio_sm],
            multi_read_cmd_buffer,
            0,
            false
        );
    }

    uint32_t* APS6404::add_read_to_cmd_buffer(uint32_t* cmd_buf, uint32_t addr, uint32_t len_in_words) {
        int32_t len_remaining = len_in_words << 2;
        uint32_t len = std::min((PAGE_SIZE - (addr & (PAGE_SIZE - 1))), (uint32_t)len_remaining);
        bool clear_isr = false;

        while (true) {
            if (len < 2) {
                // This is guaranteed to leave at least one byte in the ISR, 
                // which we then clear with an additional command
                len = 2;
                clear_isr = true;
            }
            *cmd_buf++ = (len * 2) - 4;
            *cmd_buf++ = 0xeb000000u | addr;
            *cmd_buf++ = pio_offset + sram_offset_do_read;
            len_remaining -= len;
            addr += len;

            if (len_remaining <= 0) break;

            len = len_remaining;
            if (len > PAGE_SIZE) len = PAGE_SIZE;
        }

        if (clear_isr) {
            *cmd_buf++ = 0;
            *cmd_buf++ = 0xeb000000u | addr;
            *cmd_buf++ = pio_offset + sram_offset_do_clear;
        }

        return cmd_buf;
    }

    void APS6404::wait_for_finish_blocking() {
        dma_channel_wait_for_finish_blocking(dma_channel);
    }
}
