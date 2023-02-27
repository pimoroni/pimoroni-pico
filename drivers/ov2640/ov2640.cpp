#include "ov2640.hpp"
#include "ov2640_init.h"
#include "hardware/dma.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "ov2640.pio.h"

#include <cstdio>

namespace {
    pimoroni::OV2640* ov2640_inst = nullptr;
    uint ov2640_int_channel_mask = 0;
    void dma_interrupt_handler() {
        if (!(dma_hw->ints0 & ov2640_int_channel_mask)) {
            // Interrupt for a different channel.
            return;
        }

        // Clear the interrupt flag
        dma_hw->ints0 = ov2640_int_channel_mask;

        // Handle the interrupt
        ov2640_inst->buffer_done_interrupt();
    }
}

namespace pimoroni {
    void OV2640::init(ImageSize size, ImageMode mode) {
        // XCLK generation (~15.62 MHz with RP2040 at 125MHz)
        gpio_set_function(pin_xclk, GPIO_FUNC_PWM);
        uint slice_num = pwm_gpio_to_slice_num(pin_xclk);
        // 8 cycles (0 to 7), 125 MHz / 8 = ~15.62 MHz wrap rate
        pwm_set_wrap(slice_num, 7);
        pwm_set_gpio_level(pin_xclk, 4);
        pwm_set_enabled(slice_num, true);

        // Initialise reset pin
        gpio_init(pin_resetb);
        gpio_set_dir(pin_resetb, GPIO_OUT);

        gpio_init(pin_vsync);
        gpio_set_dir(pin_vsync, GPIO_IN);

        // Reset camera, and give it some time to wake back up
        gpio_put(pin_resetb, 0);
        sleep_ms(100);
        gpio_put(pin_resetb, 1);
        sleep_ms(100);

        // Initialise the camera itself over SCCB
        regs_write(ov2640_init);
        //ov2640_regs_write(config, ov2640_uxga_cif);

        // TODO: Support other sizes
        set_image_size(size);
        set_image_mode(mode);

        // Enable image RX PIO
        uint offset = pio_add_program(pio, &ov2640_program);
        pio_sm = pio_claim_unused_sm(pio, true);
        ov2640_program_init(pio, pio_sm, offset, pin_y2_pio_base);

        // Claim DMA channels
        dma_channel = dma_claim_unused_channel(true);
        chain_dma_channel = dma_claim_unused_channel(true);

        // Clear any leftover interrupt status from previous users of the channels
        dma_hw->ints0 = 1u << dma_channel;
        dma_hw->ints0 = 1u << chain_dma_channel;

        // Set up DMA IRQ - originally this used a shared handler, but there aren't enough by default
        irq_set_exclusive_handler(DMA_IRQ_0, dma_interrupt_handler);
        dma_channel_set_irq0_enabled(chain_dma_channel, true);
        irq_set_enabled(DMA_IRQ_0, true);
    }

    void OV2640::set_image_size(ImageSize size) {
        if (size == SIZE_352x288){
            regs_write(ov2640_cif);
        }
        else if (size == SIZE_800x600){
            regs_write(ov2640_svga);
        }
        else if (size == SIZE_1600x1200)
        {
            regs_write(ov2640_uxga);
        }
        else{
            return;
        }
        current_size = size;
    }

    void OV2640::set_image_mode(ImageMode mode) {
        // Set output mode
        i2c->reg_write_uint8(OV2640_I2C_ADDRESS, 0xff, 0x00);  // Select sensor bank
        if (mode == MODE_RGB565) {
            i2c->reg_write_uint8(OV2640_I2C_ADDRESS, 0xDA, 0x09);
        }
        else if (mode == MODE_YUYV) {
            i2c->reg_write_uint8(OV2640_I2C_ADDRESS, 0xDA, 0x00);
        }
        else if (mode == MODE_RAW) {
            i2c->reg_write_uint8(OV2640_I2C_ADDRESS, 0xDA, 0x04);
        }
        else if (mode == MODE_JPEG){
            i2c->reg_write_uint8(OV2640_I2C_ADDRESS, 0xDA, 0x10);
        }
        else{
            return;
        }

        current_mode = mode;
    }

    void OV2640::test_mode(){

    }

    void OV2640::reset(){
        
    }

    uint32_t OV2640::get_image_len_in_bytes() const {
        switch (current_size) {
            case SIZE_1600x1200: return 1600 * 1200 * 2;
            case SIZE_800x600: return 800 * 600 * 2;
            case SIZE_352x288: return 352 * 288 * 2;
            default: return 0;
        }
    }

    uint32_t OV2640::start_capture(uint32_t** buffers, uint32_t num_buffers_, uint32_t buffer_len_, std::function<void(uint32_t* buffer)> callback) {
        num_buffers = num_buffers_;
        buffer_len = buffer_len_;
        next_buffer_idx = 0;
        for (uint32_t i = 0; i < num_buffers_; ++i) {
            buffer_ptrs[i] = buffers[i];
        }
        transfer_buffer_callback = callback;
        chain_cb.next_buffer_address = buffer_ptrs[1];
        chain_cb.next_transfer_len = buffer_len;
        next_buffer_idx = 2 % num_buffers;
        remaining_transfer_len = (get_image_len_in_bytes() >> 2) - 2 * buffer_len;

        dma_channel_config c = dma_channel_get_default_config(dma_channel);
        channel_config_set_read_increment(&c, false);
        channel_config_set_write_increment(&c, true);
        channel_config_set_dreq(&c, pio_get_dreq(pio, pio_sm, false));
        channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
        channel_config_set_bswap(&c, true);
        channel_config_set_chain_to(&c, chain_dma_channel);
        
        dma_channel_configure(
            dma_channel, &c,
            buffer_ptrs[0],
            &pio->rxf[pio_sm],
            buffer_len,
            false
        );

        c = dma_channel_get_default_config(chain_dma_channel);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, true);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_32);

        dma_channel_configure(
            chain_dma_channel, &c,
            &dma_hw->ch[dma_channel].al1_write_addr,
            &chain_cb,
            2,
            false
        );

        // Set up interrupt handler to point to this instance
        ov2640_inst = this;
        ov2640_int_channel_mask = 1u << chain_dma_channel;

        // Wait for vsync rising edge to start frame
        while (gpio_get(pin_vsync) == true);
        while (gpio_get(pin_vsync) == false);
        pio_sm_clear_fifos(pio, pio_sm);

        dma_channel_start(dma_channel);
        capture_in_progress = true;

        return get_image_len_in_bytes() >> 2;
    }

    void OV2640::buffer_done_interrupt() {
        // The buffer just completed is 2 before the next
        const uint32_t completed_buffer_idx = (next_buffer_idx + num_buffers - 2) % num_buffers;

        if (chain_cb.next_transfer_len == 0) {
            capture_in_progress = false;
        }

        // Set the next buffer ready for the chain
        chain_cb.next_buffer_address = buffer_ptrs[next_buffer_idx];
        next_buffer_idx = (next_buffer_idx + 1) % num_buffers;

        // Determine the next transfer len.  This will automatically terminate the
        // transfer when 0 is written as the transfer length
        uint32_t next_transfer_len = buffer_len;
        if (remaining_transfer_len < buffer_len) {
            chain_cb.next_transfer_len = remaining_transfer_len;
            next_transfer_len = remaining_transfer_len;
        }
        remaining_transfer_len -= next_transfer_len;
        dma_channel_set_write_addr(chain_dma_channel, &dma_hw->ch[dma_channel].al1_write_addr, false);
        dma_channel_set_read_addr(chain_dma_channel, &chain_cb, false);

        // Make the callback
        transfer_buffer_callback(buffer_ptrs[completed_buffer_idx]);
    }

    void OV2640::regs_write(const uint8_t (*regs_list)[2]) {
        while (1) {
            uint8_t reg = (*regs_list)[0];
            uint8_t value = (*regs_list)[1];

            if (reg == 0x00 && value == 0x00) {
                break;
            }

            i2c->reg_write_uint8(OV2640_I2C_ADDRESS, reg, value);

            regs_list++;
        }
    }
}
