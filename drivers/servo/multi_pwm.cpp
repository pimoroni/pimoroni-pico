#include "multi_pwm.hpp"
#include "common/pimoroni_common.hpp"
#include <cstdio>
#include "hardware/gpio.h"
#include <vector>
#include <algorithm>

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
static const uint LOADING_ZONE_SIZE = 3;
struct Sequence {
    uint32_t size;
    Transition data[BUFFER_SIZE];
    Sequence() : size(1), data({Transition()}) {};
};


Sequence sequences[NUM_BUFFERS];
uint sequence_index = 0;

volatile uint read_index = 0;
volatile uint last_written_index = 0;

//Sequence loading_zone = {3, {Transition(), Transition(), Transition()}}; //Need 6 words to create loading zone behaviour with normal FIFO
//Sequence loading_zone = {5, {Transition(), Transition(), Transition(), Transition(), Transition()}}; //Need 6 words to create loading zone behaviour with normal FIFO
const bool use_loading_zone = true;


uint irq_gpio = 15;
uint write_gpio = 16;
uint sideset_gpio = 17;

void __isr pwm_dma_handler() {
    // Clear the interrupt request.
    dma_hw->ints0 = 1u << data_dma_channel;

    gpio_put(irq_gpio, 1); //TOREMOVE Just for debug

    // If new data been written since the last time, switch to reading that buffer
    if(last_written_index != read_index) {
        read_index = last_written_index;
    }

    uint32_t transitions = sequences[read_index].size * 2;
    uint32_t* buffer = (uint32_t *)sequences[read_index].data;

    dma_channel_set_trans_count(data_dma_channel, transitions, false);
    dma_channel_set_read_addr(data_dma_channel, buffer, true);

    gpio_put(irq_gpio, 0); //TOREMOVE Just for debug
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
    

MultiPWM::MultiPWM(PIO pio, uint sm, uint pin_mask) : pio(pio), sm(sm), pin_mask(pin_mask) {
    pio_program_offset = pio_add_program(pio, &multi_pwm_program);

    gpio_init(irq_gpio);
    gpio_set_dir(irq_gpio, GPIO_OUT);
    gpio_init(write_gpio);
    gpio_set_dir(write_gpio, GPIO_OUT);

    if(pin_mask != 0) {
        // Initialise all the pins this PWM will control
        for(uint pin = 0; pin < 32; pin++) { // 32 is number of bits
            if((pin_mask & (1u << pin)) != 0) {
                pio_gpio_init(pio, pin);
                pin_duty[pin] = 0u;
            }
        }

        // Set their default state and direction
        pio_sm_set_pins_with_mask(pio, sm, 0x00, pin_mask);
        pio_sm_set_pindirs_with_mask(pio, sm, pin_mask, pin_mask);
    }

    pio_gpio_init(pio, sideset_gpio);
    pio_sm_set_consecutive_pindirs(pio, sm, sideset_gpio, 1, true);

    pio_sm_config c = multi_pwm_program_get_default_config(pio_program_offset);
    sm_config_set_out_pins(&c, 0, irq_gpio); //TODO change this to be 32
    sm_config_set_sideset_pins(&c, sideset_gpio);
    sm_config_set_out_shift(&c, false, true, 32);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_NONE); // We actively do not want a joined FIFO even though we are not needing the RX


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

    // Set up the transition buffers
    for(uint i = 0; i < NUM_BUFFERS; i++) {
        Sequence& seq = sequences[i];
        seq = Sequence();
    }

    // Manually call the handler once, to trigger the first transfer
    pwm_dma_handler();

    for(uint i = 0; i < 32; i++) {
        pin_duty[i] = 0;
    }

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
    // Reset all the pins this PWM will control back to an unused state
    for(uint pin = 0; pin < 32; pin++) { // 32 is number of bits
        if((1u << pin) != 0) {
            gpio_set_function(pin, GPIO_FUNC_NULL);
        }
    }
    
}

bool MultiPWM::start(uint sequence_num) {
    gpio_put(write_gpio, 1);
    // Read | Last W = Write
    // 0    | 0      = 1 (or 2)
    // 0    | 1      = 2
    // 0    | 2      = 1
    // 1    | 0      = 2
    // 1    | 1      = 2 (or 0)
    // 1    | 2      = 0
    // 2    | 0      = 1
    // 2    | 1      = 0
    // 2    | 2      = 0 (or 1)

    // Choose the write index based on the last index
    // There's probably a single equation for this, but I couldn't work it out
    uint write_index = (read_index + 1) % NUM_BUFFERS;
    if(write_index == last_written_index) {
        write_index = (write_index + 1) % NUM_BUFFERS;
    }

    switch(sequence_num) {
        case 0:
        {
            Sequence& seq = sequences[write_index];
            Transition* trans = seq.data;
            trans[0].mask = (1u << 0);
            trans[0].delay = 1000 - 1;

            trans[1].mask = (1u << 1);
            trans[1].delay = 1000 - 1;

            trans[2].mask = (1u << 1);
            trans[2].delay = 1000 - 1;

            trans[3].mask = 0;
            trans[3].delay = (20000 - 3000) - 1;
            seq.size = 4;

            if(use_loading_zone){
                trans[seq.size - 1].delay -= LOADING_ZONE_SIZE;
                for(uint i = 0; i < LOADING_ZONE_SIZE; i++) {
                    trans[seq.size].mask = 0;
                    trans[seq.size].delay = 0;
                    seq.size += 1;
                }
            }
        }
        break;

        case 1:
        {
            Sequence& seq = sequences[write_index];
            Transition* trans = seq.data;
            trans[0].mask = (1u << 5);
            trans[0].delay = 10000 - 1;

            trans[1].mask = 0;
            trans[1].delay = (20000 - 10000) - 1;
            seq.size = 2;

            if(use_loading_zone){
                trans[seq.size - 1].delay -= LOADING_ZONE_SIZE;
                for(uint i = 0; i < LOADING_ZONE_SIZE; i++) {
                    trans[seq.size].mask = 0;
                    trans[seq.size].delay = 0;
                    seq.size += 1;
                }
            }
        }
        break;

        case 2:
        {
            Sequence& seq = sequences[write_index];
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
            seq.size = last + 1;

            if(use_loading_zone){
                trans[seq.size - 1].delay -= LOADING_ZONE_SIZE;
                for(uint i = 0; i < LOADING_ZONE_SIZE; i++) {
                    trans[seq.size].mask = 0;
                    trans[seq.size].delay = 0;
                    seq.size += 1;
                }
            }
        }
        break;

        case 3:
        default:
        {
            Sequence& seq = sequences[write_index];
            Transition* trans = seq.data;
            trans[0].mask = 0;
            trans[0].delay = 20000 - 1;
            seq.size = 1;

            if(use_loading_zone){
                trans[seq.size - 1].delay -= LOADING_ZONE_SIZE;
                for(uint i = 0; i < LOADING_ZONE_SIZE; i++) {
                    trans[seq.size].mask = 0;
                    trans[seq.size].delay = 0;
                    seq.size += 1;
                }
            }
        }
        break;
    }
    last_written_index = write_index;
    gpio_put(write_gpio, 0);

    return true;
}

void MultiPWM::set_servo_duty(uint servo, uint32_t duty) {
    if(servo > 0 && servo <= 18) {
        pin_duty[servo - 1] = std::min<uint32_t>(duty, 20000);
    }
}

struct myclass {
  bool operator() (const TransitionData& i, const TransitionData& j) { return i.compare(j); }
} myobject;

void MultiPWM::sorted_insert(TransitionData array[], uint &size, const TransitionData &data) {
    uint i;
    for(i = size; (i > 0 && !array[i - 1].compare(data)); i--) {
        array[i] = array[i - 1];
    }
    array[i] = data;
    //printf("Added %d, %ld, %d\n", data.servo, data.time, data.state);
    size++;
}

void MultiPWM::apply_servo_duty() {
    gpio_put(write_gpio, 1);
    const uint window = 20000;

    TransitionData transitions[64];
    uint data_size = 0;

    // Go through each pin that we are assigned to
    for(uint pin = 0; pin < 32; pin++) {
        if(((1u << pin) & pin_mask) != 0) {
            // If the duty is greater than zero, add a transition to high
            if(pin_duty[pin] > 0) {
                MultiPWM::sorted_insert(transitions, data_size, TransitionData(pin, 0, true));
            }
            // If the duty is less than the window size, add a transition to low
            if(pin_duty[pin] < window) {
                MultiPWM::sorted_insert(transitions, data_size, TransitionData(pin, pin_duty[pin], false));
            }
        }
    }


    //for(uint i = 0; i < data_size; i++) {
    //    printf("Added %d, %ld, %d\n", transitions[i].servo, transitions[i].time, transitions[i].state);
    //}

    // Read | Last W = Write
    // 0    | 0      = 1 (or 2)
    // 0    | 1      = 2
    // 0    | 2      = 1
    // 1    | 0      = 2
    // 1    | 1      = 2 (or 0)
    // 1    | 2      = 0
    // 2    | 0      = 1
    // 2    | 1      = 0
    // 2    | 2      = 0 (or 1)

    // Choose the write index based on the last index
    // There's probably a single equation for this, but I couldn't work it out
    uint write_index = (read_index + 1) % NUM_BUFFERS;
    if(write_index == last_written_index) {
        write_index = (write_index + 1) % NUM_BUFFERS;
    }

    Sequence& seq = sequences[write_index];
    seq.size = 0; // Reset the sequence, otherwise we end up appending, and weird things happen
    //printf("Write Index = %d\n", write_index);

    uint pin_states = 0;


    uint current_time = 0;
    uint data_start = 0;

    while(data_start < data_size) {
        uint next_time = window; // Set the next time to be the Window, initially

        do {
            // Is the time of this transition the same as the current time?
            if(transitions[data_start].time <= current_time) {
                // Yes, so update the state of this pin
                if(transitions[data_start].state)
                    pin_states = pin_states | (1u << transitions[data_start].servo);
                else
                    pin_states = pin_states & ~(1u << transitions[data_start].servo);
                data_start++;
            }
            else {
                // No, so set the next time to be the time of this transition, and break out of the loop
                next_time = transitions[data_start].time;
                break;
            }

        } while(data_start < data_size);

        //#print("0b{:016b}".format(pin_mask))
        // Add the transition
        seq.data[seq.size].mask = pin_states;
        seq.data[seq.size].delay = (next_time - current_time) - 1;

        //printf("%#010x, %ld\n", pin_states, seq.data[seq.size].delay + 1);
        seq.size++;
        //#print((next_time - time) - 1)

        current_time = next_time;
    }

    if(use_loading_zone) {
        seq.data[seq.size - 1].delay -= LOADING_ZONE_SIZE;
        for(uint i = 0; i < LOADING_ZONE_SIZE; i++) {
            seq.data[seq.size].mask = 0;
            seq.data[seq.size].delay = 0;
            seq.size++;
        }
    }

    last_written_index = write_index;
    gpio_put(write_gpio, 0);
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