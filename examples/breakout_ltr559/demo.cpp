#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "breakout_ltr559.hpp"

using namespace pimoroni;

LTR559 ltr559;

int main() {
    setup_default_uart();

    if (ltr559.init() == 1) {
        printf("Failed to set up LTR559\n");
        return 1;
    }

    uint8_t part_id = ltr559.part_id();
    printf("Found LTR559. Part ID: 0x%02x\n", part_id);

    while(true){
        bool new_data = ltr559.get_reading();
        if(new_data) {
            printf("Lux: %d Prox: %d\n", ltr559.data.lux, ltr559.data.proximity);
        }
    };
    return 0;
}
