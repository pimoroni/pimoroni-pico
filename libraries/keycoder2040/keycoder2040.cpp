#include <math.h>
#include <cfloat>
#include <climits>
#include <cstdlib>
#include <string.h>
#include <time.h>
#include <cstdio>

#include "pico/stdlib.h"
#include "libraries/keycoder2040/keycoder2040.hpp"
#include "common/pimoroni_common.hpp"
#include "drivers/is31fl3731/is31fl3731.hpp"



namespace pimoroni{


    namespace keycoder2040{


            uint32_t Keycoder2040::last_state = 0x00;
            uint32_t Keycoder2040::new_state = 0x00;
            PIO Keycoder2040::keycoder2040_pio;
            uint Keycoder2040::keycoder2040_sm;
            uint Keycoder2040::pio_program_offset[] = { 0, 0 };
            uint8_t Keycoder2040::states[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

            

            Keycoder2040::Keycoder2040(PIO pio, uint sm, Direction direction, float counts_per_rev, bool count_microsteps, uint16_t freq_divider)
                : pio(pio)
                , sm(sm)
                , enc_direction(direction)
                , enc_counts_per_rev(MAX(counts_per_rev, FLT_EPSILON))
                , count_microsteps(count_microsteps)
                , freq_divider(freq_divider) {

                    Keycoder2040::keycoder2040_pio = pio0;
                    Keycoder2040::keycoder2040_sm = sm;

                    
                    


                }

            



            bool Keycoder2040::init(){

                pio_sm_claim(pio, sm);
                uint pio_idx = pio_get_index(pio);

                

                // If this is the first time using an encoder on this PIO, add the program to the PIO memory
                
                pio_program_offset[pio_idx] = pio_add_program(pio, &keycoder_program);
                

                // Initilise all required 
                pio_sm_set_consecutive_pindirs(pio, sm, ENCODERS_START, ENCODERS_WIDTH, false);
                pio_sm_set_consecutive_pindirs(pio, sm, COLUMN_START, COLUMN_WIDTH, true);
                for (uint i = ENCODERS_START; i < ENCODERS_START + ENCODERS_WIDTH; ++i){
                    pio_gpio_init(pio, i);
                }
                for (uint i = COLUMN_START; i < COLUMN_START + COLUMN_WIDTH; ++i){
                    pio_gpio_init(pio, i);
                }
                for (int index=0 ; index < COLUMN_WIDTH ; index++ ){
                    gpio_disable_pulls(COLUMN_START + index);
                }
                
                for (int index=0 ; index < ENCODERS_WIDTH ; index++ ){
                    gpio_pull_down(ENCODERS_START + index);
                }
                

                
                
                /*
                gpio_init_mask(ENCODERS_MASK);
                gpio_set_dir_masked(ENCODERS_MASK , false); //set the encoder rows IO to input 
                for (int index=0 ; index < ENCODERS_WIDTH ; index++ ){
                    gpio_pull_down(ENCODERS_START + index);
                    gpio_set_function(ENCODERS_START + index, GPIO_FUNC_PIO0);
                }

                gpio_init_mask(COLUMN_MASK);
                gpio_set_dir_masked(COLUMN_MASK , true); //set the column drivers to output
                for (int index=0 ; index < COLUMN_WIDTH ; index++ ){
                    gpio_disable_pulls(COLUMN_START + index);
                    
                    //gpio_put(COLUMN_START + index, true);
                    gpio_set_function(COLUMN_START + index, GPIO_FUNC_PIO0);
                }
                */
                // Create PIO configuration handler
                pio_sm_config c = keycoder_program_get_default_config(pio_program_offset[pio_idx]);
                
                // Set Input pins base 
                sm_config_set_in_pins(&c, ENCODERS_START);

                // Setup FIFO Config autopush off 
                sm_config_set_in_shift(&c, false, false, 1);
                sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);

                // Setup Column pins in sideset configuration 
                sm_config_set_sideset(&c , COLUMN_WIDTH, false, false);
                sm_config_set_sideset_pins(&c, COLUMN_START);
                

                // Set the PIO clock speed as a division of clk_sys
                sm_config_set_clkdiv_int_frac(&c, freq_divider, 0);

                // Write settings into PIO             
                pio_sm_init(pio, sm, pio_program_offset[pio_idx], &c);
                
                //put any inital PIO sm instructions here 
                //pio_sm_exec(pio, sm, pio_encode_set(pio_x, (uint)enc_state_a << 1 | (uint)enc_state_b));

                // Set sm running
                pio_sm_set_enabled(pio, sm, true);

               

                hw_set_bits(&pio->inte1, PIO_IRQ1_INTE_SM0_RXNEMPTY_BITS << sm);

        // Configure the processor to run pio_handler() when PIO IRQ 0 is asserted
        if(pio_idx == 0) {
          irq_add_shared_handler(PIO0_IRQ_1, pio0_interrupt_handler, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
          irq_set_enabled(PIO0_IRQ_1, true);
        }
        else {
          irq_add_shared_handler(PIO1_IRQ_1, pio1_interrupt_handler, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
          irq_set_enabled(PIO1_IRQ_1, true);
        }


                return true;

            }

            void Keycoder2040::process_steps(PIO pio, uint sm){
                //printf("Procossing step\n");



                uint32_t local_new_state;

                
                local_new_state = pio_sm_get(pio, sm);
                new_state = local_new_state;

                for (uint state_index=0; state_index < 31 ; state_index += 2){
                    if ((last_state & (0b11 << state_index)) != (new_state & (0b11 << state_index))){
                        states[state_index/2] ++;

                    }
                

                    
                }
                // TODO check which encoder moved and update steps

                last_state = local_new_state;


            }


            void Keycoder2040::pio_interrupt_handler(uint pio_idx) {
                // Go through each SM on the PIO to see which triggered this interrupt,
                // and if there's an associated encoder, have it update its state
                //printf("interupt handled\n");
                process_steps(keycoder2040_pio, keycoder2040_sm);
                
                }
                

            void Keycoder2040::pio0_interrupt_handler() {
            pio_interrupt_handler(0);
            
            }

            void Keycoder2040::pio1_interrupt_handler() {
            pio_interrupt_handler(1);
            }



            uint32_t Keycoder2040::get_last_state(){
                return last_state;
            };

        
        }

                    



    }
