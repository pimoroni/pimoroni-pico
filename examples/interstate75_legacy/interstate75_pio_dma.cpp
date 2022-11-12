#include <stdio.h>
#include <math.h>
#include <cstdint>
#include <cstring>

#include "pico/stdlib.h"

#include "common/pimoroni_common.hpp"

#include "hub75.hpp"

using namespace pimoroni;

// Display size in pixels
// Should be either 64x64 or 32x32 but perhaps 64x32 an other sizes will work.
// Note: this example uses only 5 address lines so it's limited to 32*2 pixels.
const uint8_t WIDTH = 32;
const uint8_t HEIGHT = 32;

Hub75 hub75(WIDTH, HEIGHT, nullptr);

void __isr dma_complete() {
    hub75.dma_complete();
}

int main() {
    hub75.start(dma_complete);

    // Basic loop to draw something to the screen.
    // This gets the distance from the middle of the display and uses it to paint a circular colour cycle.
    while (true) {
        float offset = millis() / 10000.0f;
        for(auto x = 0u; x < WIDTH; x++) {
            for(auto y = 0u; y < HEIGHT; y++) {
                // Center our rainbow circles
                float x1 = ((int)x - WIDTH / 2);
                float y1 = ((int)y - HEIGHT / 2);
                // Get hue as the distance from the display center as float from 0.0 to 1.0f.
                float h = float(x1*x1 + y1*y1) / float(WIDTH*WIDTH + HEIGHT*HEIGHT);
                // Offset our hue to animate the effect
                h -= offset;
                hub75.set_hsv(x, y, h, 1.0f, 1.0f);
            }
        }

        hub75.flip();

        sleep_ms(1000 / 60);
    }
}
