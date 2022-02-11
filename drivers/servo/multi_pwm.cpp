#include "multi_pwm.hpp"
#include "common/pimoroni_common.hpp"
#include <cstdio>
#include "hardware/gpio.h"

namespace servo {


int data_dma_channel;
int ctrl_dma_channel;

static const uint BUFFER_SIZE = 64;     // Set to 64, the maximum number of single rises and falls for 32 channels within a looping time period
struct alignas(8) Transition {
    uint32_t mask;
    uint32_t delay;
    Transition() : mask(0), delay(0) {};
};
static const uint NUM_BUFFERS = 3;
struct Sequence {
    uint32_t size;
    Transition data[BUFFER_SIZE];
};

Sequence sequences[NUM_BUFFERS];
uint sequence_index = 0;

//Sequence loading_zone = {3, {Transition(), Transition(), Transition()}}; //Need 6 words to create loading zone behaviour with normal FIFO
Sequence loading_zone = {5, {Transition(), Transition(), Transition(), Transition(), Transition()}}; //Need 6 words to create loading zone behaviour with normal FIFO
bool enter_loading_zone = false;
const bool use_loading_zone = true;
uint loading_zone_size = 3;

uint gpio = 15;

void __isr pwm_dma_handler() {
    // Clear the interrupt request.
    dma_hw->ints0 = 1u << data_dma_channel;

    // if(enter_loading_zone) {
    //     gpio_put(gpio+1, 1);
    //     uint32_t transitions = loading_zone.size * 2;
    //     uint32_t* buffer = (uint32_t *)loading_zone.data;
    //     dma_channel_set_trans_count(data_dma_channel, transitions, false);
    //     dma_channel_set_read_addr(data_dma_channel, buffer, true);

    //     enter_loading_zone = false;
    //     gpio_put(gpio+1, 0);
    // }
    // else {
    gpio_put(gpio, 1);
    uint32_t transitions = sequences[sequence_index].size * 2;
    uint32_t* buffer = (uint32_t *)sequences[sequence_index].data;

    dma_channel_set_trans_count(data_dma_channel, transitions, false);
    dma_channel_set_read_addr(data_dma_channel, buffer, true);

    // For some reason sequence 0 is output to the PIO twice, rather than once, despite the below line shifting the index along...
    // ^ Seemed related to filling an 8 long fifo buffer. Reducing to 4 removed it.
    sequence_index = (sequence_index + 1) % NUM_BUFFERS;

    gpio_put(gpio, 0);
    //}
}

    /***
     * From RP2040 datasheet
     * *
     * One disadvantage of this technique is that we don’t start to reconfigure the channel until some time after the channel
makes its last transfer. If there is heavy interrupt activity on the processor, this may be quite a long time, and therefore
quite a large gap in transfers, which is problematic if we need to sustain a high data throughput.
This is solved by using two channels, with their CHAIN_TO fields crossed over, so that channel A triggers channel B when it
completes, and vice versa. At any point in time, one of the channels is transferring data, and the other is either already
configured to start the next transfer immediately when the current one finishes, or it is in the process of being
reconfigured. When channel A completes, it immediately starts the cued-up transfer on channel B. At the same time, the
interrupt is fired, and the handler reconfigures channel A so that it is ready for when channel B completes.
     * */
    

MultiPWM::MultiPWM(PIO pio, uint sm, uint pin) : pio(pio), sm(sm) {
    pio_program_offset = pio_add_program(pio, &multi_pwm_program);

    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_OUT);
    gpio_init(gpio+1);
    gpio_set_dir(gpio+1, GPIO_OUT);

    for(uint i = pin; i < pin + 15; i++)
        pio_gpio_init(pio, i);

    pio_gpio_init(pio, 17);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 17, true);
    pio_sm_set_consecutive_pindirs(pio, sm, 17, 1, true);

    pio_sm_config c = multi_pwm_program_get_default_config(pio_program_offset);
    sm_config_set_out_pins(&c, pin, 17);
    sm_config_set_sideset_pins(&c, 17);
    sm_config_set_out_shift(&c, false, true, 32);
    //sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX); // Joining the FIFOs makes the DMA interrupts occur earlier than we would like
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_NONE);

    float div = clock_get_hz(clk_sys) / 5000000;
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm, pio_program_offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    data_dma_channel = dma_claim_unused_channel(true);
    /*ctrl_dma_channel = dma_claim_unused_channel(true);

    dma_channel_config ctrl_config = dma_channel_get_default_config(ctrl_dma_channel);
    channel_config_set_transfer_data_size(&ctrl_config, DMA_SIZE_32);
    //channel_config_set_read_increment(&ctrl_config, false);
    //channel_config_set_write_increment(&ctrl_config, false);
    channel_config_set_read_increment(&ctrl_config, true);
    channel_config_set_write_increment(&ctrl_config, true);
    channel_config_set_ring(&ctrl_config, true, 3); // 1 << 3 byte boundary on write ptr
    channel_config_set_ring(&ctrl_config, false, 3); // 1 << 3 byte boundary on read ptr

    dma_channel_configure(
        ctrl_dma_channel,
        &ctrl_config,
        //The below two work
        //&dma_hw->ch[data_dma_channel].al1_transfer_count_trig,
        //&transfer_count,
        //1,
        //These two do not
        //&dma_hw->ch[data_dma_channel].al3_read_addr_trig,
        //&((uint32_t *)buffer),
        &dma_hw->ch[data_dma_channel].al3_transfer_count, // Initial write address
        &control_blocks[0],
        2,
        false
    );*/

    dma_channel_config data_config = dma_channel_get_default_config(data_dma_channel);
    channel_config_set_bswap(&data_config, false);
    channel_config_set_dreq(&data_config, pio_get_dreq(pio, sm, true));
    channel_config_set_transfer_data_size(&data_config, DMA_SIZE_32);
    channel_config_set_read_increment(&data_config, true);
    //channel_config_set_chain_to(&data_config, ctrl_dma_channel);
    //channel_config_set_ring(&data_config, false, 7);

    dma_channel_configure(
        data_dma_channel,
        &data_config,
        &pio->txf[sm],
        NULL,
        0,
        false);

    dma_channel_set_irq0_enabled(data_dma_channel, true);

    // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
    irq_set_exclusive_handler(DMA_IRQ_0, pwm_dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    {
        Sequence& seq = sequences[0];
        Transition* trans = seq.data;
        trans[0].mask = (1u << 0);
        trans[0].delay = 1000 - 1;

        trans[1].mask = (1u << 1);
        trans[1].delay = 1000 - 1;

        trans[2].mask = (1u << 1);
        trans[2].delay = 1000 - 1;

        trans[3].mask = 0;
        trans[3].delay = (20000 - 3000) - 1;

        //if(use_loading_zone)
            //trans[4].delay -= loading_zone.size;

        seq.size = 4;

        if(use_loading_zone){
            trans[seq.size - 1].delay -= loading_zone_size;
            for(uint i = 0; i < loading_zone_size; i++) {
                trans[seq.size].mask = 0;
                trans[seq.size].delay = 0;
                seq.size += 1;
            }
        }
    }

    {
        Sequence& seq = sequences[1];
        Transition* trans = seq.data;
        trans[0].mask = (1u << 5);
        trans[0].delay = 10000 - 1;

        trans[1].mask = 0;
        trans[1].delay = (20000 - 10000) - 1;

        //if(use_loading_zone)
            //trans[1].delay -= loading_zone.size;

        seq.size = 2;

        if(use_loading_zone){
            trans[seq.size - 1].delay -= loading_zone_size;
            for(uint i = 0; i < loading_zone_size; i++) {
                trans[seq.size].mask = 0;
                trans[seq.size].delay = 0;
                seq.size += 1;
            }
        }
    }

    {
        Sequence& seq = sequences[2];
        Transition* trans = seq.data;

        uint count = 0;
        uint last = 14;
        for(uint i = 0; i < last; i++) {
            trans[i].mask = (1u << i);
            trans[i].delay = 1000 - 1;
            count += 1000;
        }

        trans[last].mask = 0;
        trans[last].delay = (20000 - count) - 1;

        //if(use_loading_zone)
        //    trans[last].delay -= loading_zone.size;

        seq.size = last + 1;

        if(use_loading_zone){
            trans[seq.size - 1].delay -= loading_zone_size;
            for(uint i = 0; i < loading_zone_size; i++) {
                trans[seq.size].mask = 0;
                trans[seq.size].delay = 0;
                seq.size += 1;
            }
        }
    }

    // Manually call the handler once, to trigger the first transfer
    pwm_dma_handler();

    //dma_start_channel_mask(1u << ctrl_dma_channel);
}

MultiPWM::~MultiPWM() {
    stop();
    clear();
    dma_channel_unclaim(data_dma_channel);
    dma_channel_unclaim(ctrl_dma_channel);
    pio_sm_set_enabled(pio, sm, false);
    pio_remove_program(pio, &multi_pwm_program, pio_program_offset);
#ifndef MICROPY_BUILD_TYPE
    // pio_sm_unclaim seems to hardfault in MicroPython
    pio_sm_unclaim(pio, sm);
#endif
}

bool MultiPWM::start(uint fps) {
    //add_repeating_timer_ms(-(1000 / fps), dma_timer_callback, (void*)this, &timer);
    return true;
}

bool MultiPWM::stop() {
    return true;//cancel_repeating_timer(&timer);
}

void MultiPWM::clear() {
    //for (auto i = 0u; i < num_leds; ++i) {
    //    set_rgb(i, 0, 0, 0);
    //}
}
}