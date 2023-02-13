#include "aps6404.hpp"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"
#include "aps6404.pio.h"

namespace {
    pimoroni::APS6404* aps6404_inst = nullptr;
    uint aps6404_int_channel_mask = 0;
    void dma_interrupt_handler() {
        if (!(dma_hw->ints1 & aps6404_int_channel_mask)) {
            // Interrupt for a different channel.
            return;
        }

        // Disable interrupts while processing the interrupt to avoid
        // inconsistencies in the write queue state
        uint32_t interrupt_status = save_and_disable_interrupts();

        // Clear the interrupt flag
        dma_hw->ints1 = aps6404_int_channel_mask;

        // Handle the interrupt
        aps6404_inst->transfer_done_interrupt();

        restore_interrupts(interrupt_status);
    }
}

namespace pimoroni {
    void APS6404::init() {
        // Initialize data pins
        for (int i = 0; i < 4; ++i) {
            gpio_init(pin_d0 + i);
            gpio_disable_pulls(pin_d0 + i);
        }

        pio_offset = pio_add_program(pio, &sram_reset_program);
        pio_sm = pio_claim_unused_sm(pio, true);
        aps6404_reset_program_init(pio, pio_sm, pio_offset, pin_csn, pin_d0);

        sleep_us(200);
        pio_sm_put_blocking(pio, pio_sm, 0x00000007u);
        pio_sm_put_blocking(pio, pio_sm, 0x66000000u);
        pio_sm_put_blocking(pio, pio_sm, 0x00000007u);
        pio_sm_put_blocking(pio, pio_sm, 0x99000000u);
        pio_sm_put_blocking(pio, pio_sm, 0x00000007u);
        pio_sm_put_blocking(pio, pio_sm, 0x35000000u);
        sleep_us(300);
        sleep_us(300);
        pio_sm_set_enabled(pio, pio_sm, false);

        pio_remove_program(pio, &sram_reset_program, pio_offset);

        pio_offset = pio_add_program(pio, &sram_program);
        //printf("SRAM program loaded to PIO at offset %d\n", offset);
        aps6404_program_init(pio, pio_sm, pio_offset, pin_csn, pin_d0);

        // Claim DMA channel
        dma_channel = dma_claim_unused_channel(true);
        aps6404_int_channel_mask = 1u << dma_channel;

        // Clear any previously set interrupt state for the channel
        dma_hw->ints1 = aps6404_int_channel_mask;

        // Set up DMA IRQ - originally this used shared handlers but there aren't enough by default(!)
        irq_set_exclusive_handler(DMA_IRQ_1, dma_interrupt_handler);
        dma_channel_set_irq1_enabled(dma_channel, true);
        irq_set_enabled(DMA_IRQ_1, true);
        aps6404_inst = this;
    }

    void APS6404::write(uint32_t addr, uint32_t* data, uint32_t len_in_words) {
        wait_for_finish_blocking();

        dma_channel_config c = dma_channel_get_default_config(dma_channel);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, false);
        channel_config_set_dreq(&c, pio_get_dreq(pio, pio_sm, true));
        channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
        
        dma_channel_configure(
            dma_channel, &c,
            &pio->txf[pio_sm],
            data,
            len_in_words,
            false
        );

        pio_sm_put_blocking(pio, pio_sm, (len_in_words * 8) - 1);
        pio_sm_put_blocking(pio, pio_sm, 0x38000000u | addr);
        pio_sm_put_blocking(pio, pio_sm, pio_offset + sram_offset_do_write);

        dma_channel_start(dma_channel);
    }

    void APS6404::read(uint32_t addr, uint32_t* read_buf, uint32_t len_in_words) {
        wait_for_finish_blocking();

        dma_channel_config c = dma_channel_get_default_config(dma_channel);
        channel_config_set_read_increment(&c, false);
        channel_config_set_write_increment(&c, true);
        channel_config_set_dreq(&c, pio_get_dreq(pio, pio_sm, false));
        channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
        
        dma_channel_configure(
            dma_channel, &c,
            read_buf,
            &pio->rxf[pio_sm],
            len_in_words,
            true
        );

        pio_sm_put_blocking(pio, pio_sm, (len_in_words * 8) - 4);
        pio_sm_put_blocking(pio, pio_sm, 0xeb000000u | addr);
        pio_sm_put_blocking(pio, pio_sm, pio_offset + sram_offset_do_read);
    }

    void APS6404::read_blocking(uint32_t addr, uint32_t* read_buf, uint32_t len_in_words) {
        while (len_in_words > 0) {
            uint32_t len = len_in_words;
            if (len > (PAGE_SIZE >> 2)) len = PAGE_SIZE >> 2;
            read(addr, read_buf, len);

            addr += len << 2;
            read_buf += len;
            len_in_words -= len;
        }

        wait_for_finish_blocking();
    }

    void APS6404::wait_for_finish_blocking() {
        dma_channel_wait_for_finish_blocking(dma_channel);
    }

    bool APS6404::enqueue_write(uint32_t addr, uint32_t* data, uint32_t len_in_words) {
        if (num_queued_buffers == MAX_QUEUED_BUFFERS) {
            return false;
        }

        // Temporarily disable handling for the transfer interrupt
        irq_set_enabled(DMA_IRQ_1, false);

        if (!dma_channel_is_busy(dma_channel) && !(dma_hw->ints1 & aps6404_int_channel_mask)) {
            // DMA is not running and the interrupt state is not set, start the write immediately
            // The interrupt handler can't be in progress because it disables interrupts before
            // clearing the interrupt status.
            //printf("\nStart\n");
            write(addr, data, len_in_words);
        }
        else
        {
            // We can't be interrupting the transfer interrupt itself, because that disables
            // interrupts.  It also can't interrupt us, so it is safe to manipulate the shared state
            // and we are guaranteed that this buffer will be transferred when previous buffers finish.
            //printf("\nQueue\n");
            uint32_t next_queue_idx = (write_queue_idx + num_queued_buffers) % MAX_QUEUED_BUFFERS;
            write_queue[next_queue_idx].addr = addr;
            write_queue[next_queue_idx].buffer = data;
            write_queue[next_queue_idx].buffer_len = len_in_words;
            ++num_queued_buffers;
        }

        // Re-enable the interrupt
        irq_set_enabled(DMA_IRQ_1, true);

        return true;
    }

    void APS6404::transfer_done_interrupt() {
        if (num_queued_buffers > 0) {
            auto& queue_entry = write_queue[write_queue_idx];
            write(queue_entry.addr, queue_entry.buffer, queue_entry.buffer_len);
            write_queue_idx = (write_queue_idx + 1) % MAX_QUEUED_BUFFERS;
            --num_queued_buffers;
        }
    }
}

