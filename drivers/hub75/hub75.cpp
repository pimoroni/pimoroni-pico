
#include <cstring>
#include <algorithm>
#include <cmath>

#include "hardware/clocks.h"

#include "hub75.hpp"

namespace pimoroni
{
    Hub75::Hub75(uint width, uint height, Pixel *buffer, PanelType panel_type, bool inverted_stb, COLOR_ORDER color_order)
        : width(width), height(height), panel_type(panel_type), inverted_stb(inverted_stb), color_order(color_order), buffer(buffer)
    {
        self = this;

        // Set up allllll the GPIO
        gpio_init(pin_r0);
        gpio_set_function(pin_r0, GPIO_FUNC_SIO);
        gpio_set_dir(pin_r0, true);
        gpio_put(pin_r0, 0);
        gpio_init(pin_g0);
        gpio_set_function(pin_g0, GPIO_FUNC_SIO);
        gpio_set_dir(pin_g0, true);
        gpio_put(pin_g0, 0);
        gpio_init(pin_b0);
        gpio_set_function(pin_b0, GPIO_FUNC_SIO);
        gpio_set_dir(pin_b0, true);
        gpio_put(pin_b0, 0);

        gpio_init(pin_r1);
        gpio_set_function(pin_r1, GPIO_FUNC_SIO);
        gpio_set_dir(pin_r1, true);
        gpio_put(pin_r1, 0);
        gpio_init(pin_g1);
        gpio_set_function(pin_g1, GPIO_FUNC_SIO);
        gpio_set_dir(pin_g1, true);
        gpio_put(pin_g1, 0);
        gpio_init(pin_b1);
        gpio_set_function(pin_b1, GPIO_FUNC_SIO);
        gpio_set_dir(pin_b1, true);
        gpio_put(pin_b1, 0);

        gpio_init(pin_row_a);
        gpio_set_function(pin_row_a, GPIO_FUNC_SIO);
        gpio_set_dir(pin_row_a, true);
        gpio_put(pin_row_a, 0);
        gpio_init(pin_row_b);
        gpio_set_function(pin_row_b, GPIO_FUNC_SIO);
        gpio_set_dir(pin_row_b, true);
        gpio_put(pin_row_b, 0);
        gpio_init(pin_row_c);
        gpio_set_function(pin_row_c, GPIO_FUNC_SIO);
        gpio_set_dir(pin_row_c, true);
        gpio_put(pin_row_c, 0);
        gpio_init(pin_row_d);
        gpio_set_function(pin_row_d, GPIO_FUNC_SIO);
        gpio_set_dir(pin_row_d, true);
        gpio_put(pin_row_d, 0);
        gpio_init(pin_row_e);
        gpio_set_function(pin_row_e, GPIO_FUNC_SIO);
        gpio_set_dir(pin_row_e, true);
        gpio_put(pin_row_e, 0);

        gpio_init(pin_clk);
        gpio_set_function(pin_clk, GPIO_FUNC_SIO);
        gpio_set_dir(pin_clk, true);
        gpio_put(pin_clk, !clk_polarity);
        gpio_init(pin_stb);
        gpio_set_function(pin_stb, GPIO_FUNC_SIO);
        gpio_set_dir(pin_stb, true);
        gpio_put(pin_clk, !stb_polarity);
        gpio_init(pin_oe);
        gpio_set_function(pin_oe, GPIO_FUNC_SIO);
        gpio_set_dir(pin_oe, true);
        gpio_put(pin_clk, !oe_polarity);

        if (brightness == 0)
        {
#if PICO_RP2350
            brightness = 6;
#else
            if (width >= 64)
                brightness = 6;
            if (width >= 96)
                brightness = 3;
            if (width >= 128)
                brightness = 2;
            if (width >= 160)
                brightness = 1;
#endif
        }

        switch (color_order)
        {
        case COLOR_ORDER::RGB:
            r_shift = 0;
            g_shift = 10;
            b_shift = 20;
            break;
        case COLOR_ORDER::RBG:
            r_shift = 0;
            g_shift = 20;
            b_shift = 10;
            break;
        case COLOR_ORDER::GRB:
            r_shift = 20;
            g_shift = 0;
            b_shift = 10;
            break;
        case COLOR_ORDER::GBR:
            r_shift = 10;
            g_shift = 20;
            b_shift = 0;
            break;
        case COLOR_ORDER::BRG:
            r_shift = 10;
            g_shift = 00;
            b_shift = 20;
            break;
        case COLOR_ORDER::BGR:
            r_shift = 20;
            g_shift = 10;
            b_shift = 0;
            break;
        }
    }

    void Hub75::set_color(uint x, uint y, Pixel c)
    {
        int offset = 0;
        if (x >= width || y >= height)
            return;
        if (y >= height / 2)
        {
            y -= height / 2;
            offset = (y * width + x) * 2;
            offset += 1;
        }
        else
        {
            offset = (y * width + x) * 2;
        }

        frame_buffer[offset] = c.color;
    }

    void Hub75::set_pixel(uint x, uint y, uint8_t r, uint8_t g, uint8_t b)
    {
        int offset = 0;
        if (x >= width || y >= height)
            return;
        if (y >= height / 2)
        {
            y -= height / 2;
            offset = (y * width + x) * 2;
            offset += 1;
        }
        else
        {
            offset = (y * width + x) * 2;
        }
        frame_buffer[offset] = (GAMMA_10BIT[b] << b_shift) | (GAMMA_10BIT[g] << g_shift) | (GAMMA_10BIT[r] << r_shift);
    }

    void Hub75::FM6126A_write_register(uint16_t value, uint8_t position)
    {
        gpio_put(pin_clk, !clk_polarity);
        gpio_put(pin_stb, !stb_polarity);

        uint8_t threshold = width - position;
        for (auto i = 0u; i < width; i++)
        {
            auto j = i % 16;
            bool b = value & (1 << j);

            gpio_put(pin_r0, b);
            gpio_put(pin_g0, b);
            gpio_put(pin_b0, b);
            gpio_put(pin_r1, b);
            gpio_put(pin_g1, b);
            gpio_put(pin_b1, b);

            // Assert strobe/latch if i > threshold
            // This somehow indicates to the FM6126A which register we want to write :|
            gpio_put(pin_stb, i > threshold);
            gpio_put(pin_clk, clk_polarity);
            sleep_us(10);
            gpio_put(pin_clk, !clk_polarity);
        }
    }

    void Hub75::FM6126A_setup()
    {
        // Ridiculous register write nonsense for the FM6126A-based 64x64 matrix
        FM6126A_write_register(0b1111111111111110, 12);
        FM6126A_write_register(0b0000001000000000, 13);
    }

    /**
     * @brief Configures the PIO state machines for HUB75 matrix control.
     *
     * This function sets up the PIO state machines responsible for shifting
     * pixel data and controlling row addressing. If a PIO state machine cannot
     * be claimed, it prints an error message.
     */
    void Hub75::configure_pio()
    {
        if (!pio_claim_free_sm_and_add_program(&hub75_data_rgb888_program, &pio_config.data_pio, &pio_config.sm_data, &pio_config.data_prog_offs))
        {
            fprintf(stderr, "Failed to claim PIO state machine for hub75_data_rgb888_program\n");
        }

        if (inverted_stb)
        {
            if (!pio_claim_free_sm_and_add_program(&hub75_row_inverted_program, &pio_config.row_pio, &pio_config.sm_row, &pio_config.row_prog_offs))
            {
                fprintf(stderr, "Failed to claim PIO state machine for hub75_row_inverted_program\n");
            }
        }
        else
        {
            if (!pio_claim_free_sm_and_add_program(&hub75_row_program, &pio_config.row_pio, &pio_config.sm_row, &pio_config.row_prog_offs))
            {
                fprintf(stderr, "Failed to claim PIO state machine for hub75_row_program\n");
            }
        }

        hub75_data_rgb888_program_init(pio_config.data_pio, pio_config.sm_data, pio_config.data_prog_offs, DATA_BASE_PIN, pin_clk);
        hub75_row_program_init(pio_config.row_pio, pio_config.sm_row, pio_config.row_prog_offs, ROWSEL_BASE_PIN, ROWSEL_N_PINS, pin_stb);
    }

    /**
     * @brief Claims an available DMA channel.
     *
     * Attempts to claim an unused DMA channel. If no channels are available,
     * prints an error message and exits the program.
     *
     * @param channel_name A descriptive name for the channel, used in error messages.
     * @return The claimed DMA channel number.
     */
    inline int Hub75::claim_dma_channel(const char *channel_name)
    {
        int dma_channel = dma_claim_unused_channel(true);
        if (dma_channel < 0)
        {
            fprintf(stderr, "Failed to claim DMA channel for %s\n", channel_name);
            exit(EXIT_FAILURE); // Stop execution
        }
        return dma_channel;
    }

    /**
     * @brief Configures and claims DMA channels for HUB75 control.
     *
     * This function assigns DMA channels to handle pixel data transfer,
     * dummy pixel data, output enable signal, and output enable completion.
     * If a DMA channel cannot be claimed, the function prints an error message and exits.
     */
    void Hub75::configure_dma_channels()
    {
        pixel_chan = claim_dma_channel("pixel channel");
        dummy_pixel_chan = claim_dma_channel("dummy pixel channel");
        oen_chan = claim_dma_channel("output enable channel");
        oen_finished_chan = claim_dma_channel("output enable has finished channel");
    }

    /**
     * @brief Configures a DMA input channel for transferring data to a PIO state machine.
     *
     * This function sets up a DMA channel to transfer data from memory to a PIO
     * state machine. It configures transfer size, address incrementing, and DMA
     * chaining to ensure seamless operation.
     *
     * @param channel DMA channel number to configure.
     * @param transfer_count Number of data transfers per DMA transaction.
     * @param dma_size Data transfer size (8, 16, or 32-bit).
     * @param read_incr Whether the read address should increment after each transfer.
     * @param chain_to DMA channel to chain the transfer to, enabling automatic triggering.
     * @param pio PIO instance that will receive the transferred data.
     * @param sm State machine within the PIO instance that will process the data.
     */
    void Hub75::dma_input_channel_setup(uint channel,
                                        uint transfer_count,
                                        enum dma_channel_transfer_size dma_size,
                                        bool read_incr,
                                        uint chain_to,
                                        PIO pio,
                                        uint sm)
    {
        dma_channel_config conf = dma_channel_get_default_config(channel);
        channel_config_set_transfer_data_size(&conf, dma_size);
        channel_config_set_read_increment(&conf, read_incr);
        channel_config_set_write_increment(&conf, false);
        channel_config_set_dreq(&conf, pio_get_dreq(pio, sm, true));

        channel_config_set_chain_to(&conf, chain_to);

        dma_channel_configure(
            channel,        // Channel to be configured
            &conf,          // DMA configuration
            &pio->txf[sm],  // Write address: PIO TX FIFO
            NULL,           // Read address: set later
            transfer_count, // Number of transfers per transaction
            false           // Do not start transfer immediately
        );
    }

    /**
     * @brief Sets up DMA transfers for the HUB75 matrix.
     *
     * Configures multiple DMA channels to transfer pixel data, dummy pixel data,
     * and output enable signal, to the PIO state machines controlling the HUB75 matrix.
     * Also configures the DMA channel which gets active when an output enable signal has finished
     */
    void Hub75::setup_dma_transfers()
    {
        dma_input_channel_setup(pixel_chan, row_width, DMA_SIZE_32, true, dummy_pixel_chan, pio_config.data_pio, pio_config.sm_data);
        dma_input_channel_setup(dummy_pixel_chan, 8, DMA_SIZE_32, false, oen_chan, pio_config.data_pio, pio_config.sm_data);
        dma_input_channel_setup(oen_chan, 1, DMA_SIZE_32, true, oen_chan, pio_config.row_pio, pio_config.sm_row);

        dma_channel_set_read_addr(dummy_pixel_chan, dummy_pixel_data, false);

        row_in_bit_plane = row_address | ((brightness << bit_plane) << 5);
        dma_channel_set_read_addr(oen_chan, &row_in_bit_plane, false);

        dma_channel_config oen_finished_config = dma_channel_get_default_config(oen_finished_chan);
        channel_config_set_transfer_data_size(&oen_finished_config, DMA_SIZE_32);
        channel_config_set_read_increment(&oen_finished_config, false);
        channel_config_set_write_increment(&oen_finished_config, false);
        channel_config_set_dreq(&oen_finished_config, pio_get_dreq(pio_config.row_pio, pio_config.sm_row, false));
        dma_channel_configure(oen_finished_chan, &oen_finished_config, &oen_finished_data, &pio_config.row_pio->rxf[pio_config.sm_row], 1, false);
    }

    /**
     * @brief Sets up and enables the DMA interrupt handler.
     *
     * Registers the interrupt service routine (ISR) for the output enable finished DMA channel.
     * This is the channel that triggers the end of the output enable signal, which in turn
     * triggers the start of the next row's pixel data transfer.
     */
    void Hub75::setup_dma_irq()
    {
        irq_set_exclusive_handler(DMA_IRQ_0, oen_finished_handler_wrapper);
        dma_channel_set_irq0_enabled(oen_finished_chan, true);
        irq_set_enabled(DMA_IRQ_0, true);

        irq_handler_t t = irq_get_exclusive_handler(DMA_IRQ_1);
    }

    /**
     * @brief Starts the DMA transfers for the HUB75 display driver.
     *
     * This function initializes the DMA transfers by setting up the write address
     * for the Output Enable finished DMA channel and the read address for pixel data.
     * It ensures that the display begins processing frames.
     */
    void Hub75::start_hub75_driver()
    {
        dma_channel_set_write_addr(oen_finished_chan, &oen_finished_data, true);
        dma_channel_set_read_addr(pixel_chan, &frame_buffer[row_address * row_width], true);
    }

    /**
     * @brief Initializes the HUB75 display by setting up DMA and PIO subsystems.
     *
     * This function configures the necessary hardware components to drive a HUB75
     * LED matrix display. It initializes DMA channels, PIO state machines, and
     * interrupt handlers.
     *
     * @param w Width of the HUB75 display in pixels.
     * @param h Height of the HUB75 display in pixels.
     */
    void Hub75::create_hub75_driver(uint w, uint h)
    {
        row_width = w << 1;
        row_threshold = h >> 1;

        row_offset = w * row_threshold;

        frame_buffer = new uint32_t[w * h](); // Allocate memory for frame buffer

        if (buffer != nullptr)
        {
            uint j = 0;
            for (int i = 0; i < w * h; i += 2)
            {
                frame_buffer[i] = GAMMA_10BIT[(buffer[j].color & 0x0000ff) >> 0] << b_shift | GAMMA_10BIT[(buffer[j].color & 0x00ff00) >> 8] << g_shift | GAMMA_10BIT[(buffer[j].color & 0xff0000) >> 16] << r_shift;
                frame_buffer[i + 1] = GAMMA_10BIT[(buffer[j + row_offset].color & 0x0000ff) >> 0] << b_shift | GAMMA_10BIT[(buffer[j + row_offset].color & 0x00ff00) >> 8] << g_shift | GAMMA_10BIT[(buffer[j + row_offset].color & 0xff0000) >> 16] << r_shift;;
                j++;
            }
        }

        configure_pio();
        configure_dma_channels();
        setup_dma_transfers();
        setup_dma_irq();
    }

    void Hub75::start(irq_handler_t handler)
    {
        handler = handler;
        start();
    }

    void Hub75::start()
    {
        if (panel_type == PANEL_FM6126A)
        {
            FM6126A_setup();
        }

        create_hub75_driver(width, height);
        start_hub75_driver();
    }

    void Hub75::stop()
    {
        irq_set_enabled(DMA_IRQ_0, false);

        if (oen_finished_chan != -1 && dma_channel_is_claimed(oen_finished_chan))
        {
            dma_channel_set_irq0_enabled(oen_finished_chan, false);
            irq_remove_handler(DMA_IRQ_0, oen_finished_handler_wrapper);

            dma_channel_abort(oen_finished_chan);
            dma_channel_acknowledge_irq0(oen_finished_chan);
            dma_channel_unclaim(oen_finished_chan);
        }

        if (oen_chan != -1 && dma_channel_is_claimed(oen_chan))
        {
            dma_channel_abort(oen_chan);
            dma_channel_acknowledge_irq0(oen_chan);
            dma_channel_unclaim(oen_chan);
        }

        if (dummy_pixel_chan != -1 && dma_channel_is_claimed(dummy_pixel_chan))
        {
            dma_channel_abort(dummy_pixel_chan);
            dma_channel_acknowledge_irq0(dummy_pixel_chan);
            dma_channel_unclaim(dummy_pixel_chan);
        }

        if (pixel_chan != -1 && dma_channel_is_claimed(pixel_chan))
        {
            dma_channel_abort(pixel_chan);
            dma_channel_acknowledge_irq0(pixel_chan);
            dma_channel_unclaim(pixel_chan);
        }

        if (pio_sm_is_claimed(pio, sm_data))
        {
            pio_sm_set_enabled(pio, sm_data, false);
            pio_sm_drain_tx_fifo(pio, sm_data);
            pio_remove_program(pio, &hub75_data_rgb888_program, data_prog_offs);
            pio_sm_unclaim(pio, sm_data);
        }

        if (pio_sm_is_claimed(pio, sm_row))
        {
            pio_sm_set_enabled(pio, sm_row, false);
            pio_sm_drain_tx_fifo(pio, sm_row);
            if (inverted_stb)
            {
                pio_remove_program(pio, &hub75_row_inverted_program, row_prog_offs);
            }
            else
            {
                pio_remove_program(pio, &hub75_row_program, row_prog_offs);
            }
            pio_sm_unclaim(pio, sm_row);
        }

        // Make sure the GPIO is in a known good state
        // since we don't know what the PIO might have done with it
        gpio_put_masked(0b111111 << pin_r0, 0);
        gpio_put_masked(0b11111 << pin_row_a, 0);
        gpio_put(pin_clk, !clk_polarity);
        gpio_put(pin_clk, !oe_polarity);
    }

    Hub75::~Hub75()
    {
        delete[] frame_buffer;
        self = nullptr;
    }

    void Hub75::clear()
    {
        for (int i = 0; i < width * height; i += 2)
        {
            frame_buffer[i] = 0;
            frame_buffer[i + 1] = 0;
        }
    }

    void Hub75::copy_to_back_buffer(void *data, size_t len, int start_x, int start_y, int g_width, int g_height)
    {
        uint8_t *p = (uint8_t *)data;

        if (g_width == int32_t(width / 2) && g_height == int32_t(height * 2))
        {
            for (int y = start_y; y < g_height; y++)
            {
                int offsety = 0;
                int sy = y;
                int basex = 0;

                // Assuming our canvas is 128x128 and our display is 256x64,
                // consisting of 2x128x64 panels, remap the bottom half
                // of the canvas to the right-half of the display,
                // This gives us an optional square arrangement.
                if (sy >= int(height))
                {
                    sy -= height;
                    basex = width / 2;
                }

                // Interlace the top and bottom halves of the panel.
                // Since these are scanned out simultaneously to two chains
                // of shift registers we need each pair of rows
                // (N and N + height / 2) to be adjacent in the buffer.
                offsety = width * 2;
                if (sy >= int(height / 2))
                {
                    sy -= height / 2;
                    offsety *= sy;
                    offsety += 1;
                }
                else
                {
                    offsety *= sy;
                }

                for (int x = start_x; x < g_width; x++)
                {
                    int sx = x;
                    uint8_t b = *p++;
                    uint8_t g = *p++;
                    uint8_t r = *p++;

                    // Assumes width / 2 is even.
                    if (basex & 1)
                    {
                        sx = basex - sx;
                    }
                    else
                    {
                        sx += basex;
                    }
                    int offset = offsety + sx * 2;

                    frame_buffer[offset] = (GAMMA_10BIT[b] << b_shift) | (GAMMA_10BIT[g] << g_shift) | (GAMMA_10BIT[r] << r_shift);

                    // Skip the empty byte in out 32-bit aligned 24-bit colour.
                    p++;

                    len -= 4;

                    if (len == 0)
                    {
                        return;
                    }
                }
            }
        }
        else
        {
            for (uint y = start_y; y < height; y++)
            {
                for (uint x = start_x; x < width; x++)
                {
                    int offset = 0;
                    int sy = y;
                    int sx = x;
                    uint8_t b = *p++;
                    uint8_t g = *p++;
                    uint8_t r = *p++;

                    // Interlace the top and bottom halves of the panel.
                    // Since these are scanned out simultaneously to two chains
                    // of shift registers we need each pair of rows
                    // (N and N + height / 2) to be adjacent in the buffer.
                    offset = width * 2;
                    if (sy >= int(height / 2))
                    {
                        sy -= height / 2;
                        offset *= sy;
                        offset += 1;
                    }
                    else
                    {
                        offset *= sy;
                    }
                    offset += sx * 2;

                    frame_buffer[offset] = (GAMMA_10BIT[b] << b_shift) | (GAMMA_10BIT[g] << g_shift) | (GAMMA_10BIT[r] << r_shift);

                    // Skip the empty byte in out 32-bit aligned 24-bit colour.
                    p++;

                    len -= 4;

                    if (len == 0)
                    {
                        return;
                    }
                }
            }
        }
    }
    /**
     * @brief Updates the frame buffer with pixel data from the source array.
     *
     * This function takes a source array of pixel data and updates the frame buffer
     * with interleaved pixel values. The pixel values are gamma-corrected to 10 bits using a lookup table.
     *
     * @param src Pointer to the source pixel data array (RGB888 format).
     */
    void Hub75::update(PicoGraphics *graphics)
    {
        if (graphics->pen_type == PicoGraphics::PEN_RGB888)
        {
            uint32_t const *src = static_cast<uint32_t const *>(graphics->frame_buffer);

            // Ramp up color resolution from 8 to 10 bits via gamma table look-up
            // Interweave pixels from intermediate buffer into target image to fit the format expected by Hub75 LED panel.
            uint j = 0;
            for (int i = 0; i < width * height; i += 2)
            {
                frame_buffer[i] = GAMMA_10BIT[(src[j] & 0x0000ff) >> 0] << b_shift | GAMMA_10BIT[(src[j] & 0x00ff00) >> 8] << g_shift | GAMMA_10BIT[(src[j] & 0xff0000) >> 16] << r_shift;
                frame_buffer[i + 1] = GAMMA_10BIT[(src[j + row_offset] & 0x0000ff) >> 0] << b_shift | GAMMA_10BIT[(src[j + row_offset] & 0x00ff00) >> 8] << g_shift | GAMMA_10BIT[(src[j + row_offset] & 0xff0000) >> 16] << r_shift;
                j++;
            }
        }
        else
        {
            unsigned int offset = 0;
            graphics->frame_convert(PicoGraphics::PEN_RGB888, [this, &offset, &graphics](void *data, size_t length)
                                    {
            if (length > 0) {
                int offset_y = offset / graphics->bounds.w;
                int offset_x = offset - (offset_y * graphics->bounds.w);
                copy_to_back_buffer(data, length, offset_x, offset_y, graphics->bounds.w, graphics->bounds.h);
                offset += length / sizeof(RGB888);
            } });
        }
    }
}
