#include <stdio.h>
#include <math.h>
#include <cstdint>

#include "pico/stdlib.h"
#include "libraries/keycoder2040/keycoder2040.hpp"
#include "common/pimoroni_common.hpp"



using namespace pimoroni;




int main(){
    stdio_init_all();
    sleep_ms(1000);
    Interface keycorder_interface(pio0, 0);
    printf("Hello world\n");



}