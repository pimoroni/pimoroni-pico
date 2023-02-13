#pragma once

#include <functional>
#include "common/pimoroni_i2c.hpp"
#include "hardware/pio.h"

namespace pimoroni {
    // Driver for the OV2640 camera module
    //
    // Because captured images are normally much larger than the available RAM on a RP2040,
    // the normal mode of operation is for this module to capture data to a ring buffer
    // and for another module (e.g. a PSRAM) to copy the data out of the ring buffer as
    // it is captured.
    class OV2640 {
        public:
            static constexpr uint8_t OV2640_I2C_ADDRESS = 0x30;

            // Ring buffer of size 8kB.
            static constexpr uint32_t MAX_TRANSFER_BUFFERS = 8;

            // TODO: Changing image size not supported yet
            enum ImageSize {
                SIZE_1600x1200,
                //SIZE_800x600,
                //SIZE_352x288,
            };

            enum ImageMode {
                MODE_RGB565,
                MODE_YUYV,
            };

            OV2640() : OV2640(new I2C()) {}
            OV2640(I2C* i2c, uint pin_resetb = 8, uint pin_xclk = 0, uint pin_vsync = 28, uint pin_data2 = 9, PIO pio = pio0)
                : i2c(i2c) 
                , pin_resetb(pin_resetb)
                , pin_xclk(pin_xclk)
                , pin_vsync(pin_vsync)
                , pin_y2_pio_base(pin_data2)
                , pio(pio)
                , capture_in_progress(false)
            {}

            void init(ImageSize size, ImageMode mode);

            void set_image_size(ImageSize size);
            ImageSize get_image_size() const { return current_size; }

            void set_image_mode(ImageMode mode);
            ImageMode get_image_mode() const { return current_mode; }

            uint32_t get_image_len_in_bytes() const;

            // Wait for the next frame to begin and start reading the image to the supplied buffers
            // by DMA.  When each buffer is filled, the callback will be made from an interrupt.
            // Buffers are automatically re-used so must be read out in a timely fashion.
            // num_buffers specifies the number of buffers in the array, and must be <= MAX_TRANSFER_BUFFERS
            // buffer_len specifies the length in words of each buffer.
            // Returns the number of words that will be captured in total.
            uint32_t start_capture(uint32_t** buffers, uint32_t num_buffers, uint32_t buffer_len, std::function<void(uint32_t* buffer)> callback);

            bool is_capture_in_progress() const { return capture_in_progress; }

            // Called internally by the DMA interrupt handler
            void buffer_done_interrupt();

        private:
            void regs_write(const uint8_t (*regs_list)[2]);

            I2C *i2c;

            uint pin_resetb;
            uint pin_xclk;
            uint pin_vsync;
            uint pin_y2_pio_base;

            PIO pio;
            uint pio_sm;

            uint dma_channel;
            uint chain_dma_channel;

            uint32_t* buffer_ptrs[MAX_TRANSFER_BUFFERS];
            uint32_t num_buffers;
            uint32_t buffer_len;
            uint32_t next_buffer_idx;
            uint32_t remaining_transfer_len;
            struct {
                uint32_t* next_buffer_address;
                uint32_t next_transfer_len;
            } chain_cb;
            std::function<void(uint32_t* buffer)> transfer_buffer_callback;
            bool capture_in_progress;

            ImageSize current_size;
            ImageMode current_mode;
    };
}
