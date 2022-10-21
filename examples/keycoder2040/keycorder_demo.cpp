#include <stdio.h>
#include <math.h>
#include <cstdint>

#include "pico/stdlib.h"
#include "libraries/keycoder2040/keycoder2040.hpp"
#include "common/pimoroni_i2c.hpp"



using namespace pimoroni;
using namespace keycoder2040;



void from_hsv(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
    float i = floor(h * 6.0f);
    float f = h * 6.0f - i;
    v *= 255.0f;
    uint8_t p = v * (1.0f - s);
    uint8_t q = v * (1.0f - f * s);
    uint8_t t = v * (1.0f - (1.0f - f) * s);

    switch (int(i) % 6) {
      case 0: r = v; g = t; b = p; break;
      case 1: r = q; g = v; b = p; break;
      case 2: r = p; g = v; b = t; break;
      case 3: r = p; g = q; b = v; break;
      case 4: r = t; g = p; b = v; break;
      case 5: r = v; g = p; b = q; break;
    }
  }



int main(){
    stdio_init_all();
    sleep_ms(3000);
    Keycoder2040 keycoder_interface(DEFAULT_PIO, 0);
    //keycoder_interface.init();

    LEDs rgbleds;

    //rgbleds.lookup_pixel(2);
    
    if (rgbleds.init()){
        printf("leds ready \n");
    }
    else{
        printf("leds fail \n");
        return 1;
    }
    
    printf("Hello world\n");
    uint8_t r;
    uint8_t g;
    uint8_t b;
    float counter = 0.0;
    while (1){
        for (uint8_t led_index = 0 ; led_index < 16 ; led_index++){

            printf("value %f, ",(float)led_index * 10.0f );
            from_hsv(((float)led_index + counter) / 16.0f , 1.0f, 1.0f, r, g, b);      

            printf("R: %d, B: %d, G: %d", r, g, b);  


            rgbleds.set_pixel(led_index, r, g, b);

            //sleep_ms(100);
        }
        rgbleds.update();
        counter = counter + 0.1;
        sleep_ms(10);
        printf("\n");

    }

    while (1){
        //keycoder_interface.process_steps(Keycoder2040::keycoder2040_pio, Keycoder2040::keycoder2040_sm);
        //printf("N:%b L:%b \n", keycoder_interface.new_state, keycoder_interface.get_last_state());
        for (uint state_index=0; state_index < 16 ; state_index++){
            printf("%d, ", keycoder_interface.encoders_count[state_index]);
        }
        printf("\n");
        sleep_ms(500);

    }



}