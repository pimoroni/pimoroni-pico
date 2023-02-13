#pragma once

#include <stdint.h>
#include "hardware/pio.h"

namespace pimoroni {
    class APS6404 {
        public:
            static constexpr int RAM_SIZE = 8 * 1024 * 1024;
            static constexpr int PAGE_SIZE = 1024;
            static constexpr int MAX_QUEUED_BUFFERS = 8;

            APS6404(uint pin_csn = 17, uint pin_d0 = 19, PIO pio = pio0)
                : pin_csn(pin_csn)
                , pin_d0(pin_d0)
                , pio(pio)
            {}

            void init();

            // Start a write, this completes asynchronously, this function blocks if another 
            // transfer is already in progress
            void write(uint32_t addr, uint32_t* data, uint32_t len_in_words);

            // Start a read, this completes asynchronously, this function only blocks if another 
            // transfer is already in progress
            // len_in_words must be <= 256
            void read(uint32_t addr, uint32_t* read_buf, uint32_t len_in_words);

            // Read and block until complete
            // Lengths longer than one page are supported.
            void read_blocking(uint32_t addr, uint32_t* read_buf, uint32_t len_in_words);

            // Block until any outstanding read or write completes
            void wait_for_finish_blocking();

            // Start or enqueue a write, this function never blocks, but can fail if the maximum
            // number of writes are already queued
            bool enqueue_write(uint32_t addr, uint32_t* data, uint32_t len_in_words);

            void transfer_done_interrupt();

            uint32_t write_queue_idx = 0;  // First enqueued buffer idx
            uint32_t num_queued_buffers = 0;
        private:
            uint pin_csn;  // CSn, SCK must be next pin after CSn
            uint pin_d0;   // D0, D1, D2, D3 must be consecutive

            PIO pio;
            uint pio_sm;
            uint pio_offset;

            uint dma_channel;

            struct {
                uint32_t addr;
                uint32_t* buffer;
                uint32_t buffer_len;
            } write_queue[MAX_QUEUED_BUFFERS];
    };
}
