#include <stdio.h>
#include "pico/stdlib.h"
#include "ioexpander.hpp"

static const uint8_t PIN_ENC_A = 12;
static const uint8_t PIN_ENC_B = 3;
static const uint8_t PIN_ENC_C = 11;

static const uint8_t ENC_CHANNEL = 1;

using namespace pimoroni;

IOExpander ioe(0x0f);

int main() {
  stdio_init_all();

  sleep_ms(5000); //Delay to give chance to connect terminal

  const uint LED_PIN = 25;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  if(ioe.init()) {
    printf("IOExpander found...\n");

    ioe.setup_rotary_encoder(ENC_CHANNEL, PIN_ENC_A, PIN_ENC_B, PIN_ENC_C);

    bool toggle = false;
    int16_t count = 0;
    while(true) {
      gpio_put(LED_PIN, toggle);
      toggle = !toggle;

      //Polling encoder directly works
      //printf("%d\n", ioe.read_rotary_encoder(ENC_CHANNEL)); 

      //Only reading encoder when the interrupt pin changes does not.
      if(ioe.get_interrupt_flag()) {
        printf("new count: ");
        count = ioe.read_rotary_encoder(ENC_CHANNEL);
        ioe.clear_interrupt_flag();
      }

      printf("%d\n", count);
      

      sleep_ms(20);
    }  
  }
  else {
    printf("IOExpander not found :'(\n");
    gpio_put(LED_PIN, 1);
  }
  
  return 0;
}
