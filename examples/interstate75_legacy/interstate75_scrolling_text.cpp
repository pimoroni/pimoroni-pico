#include <stdio.h>
#include <cmath>
#include <cstdint>
#include <string>
#include <string_view>

#include "pico/stdlib.h"
#include  "hardware/vreg.h"

#include "common/pimoroni_common.hpp"

#include "hub75.hpp"
#include "font_outline_10x14.hpp"

using namespace pimoroni;

// Display size in pixels
// Should be either 64x64 or 32x32 but perhaps 64x32 an other sizes will work.
// Note: this example uses only 5 address lines so it's limited to 64 pixel high displays (32*2).
const uint8_t WIDTH = 64;
const uint8_t HEIGHT = 64;

Hub75 hub75(WIDTH, HEIGHT, nullptr, PANEL_GENERIC, true);

void __isr dma_complete() {
    hub75.dma_complete();
}

void scroll_text(std::string_view text, uint y, float t, Pixel color) {
    uint text_length = text.length();
    uint x = uint(t);
    uint letter = uint((x / letter_width) % text_length);
    uint pixel = x % letter_width;
    char c = text.at(letter);
    for (auto s_x = 0u; s_x < WIDTH; s_x++) {
        uint32_t col = font[c - 32][pixel];
        for (auto s_y = 0u; s_y < letter_height; s_y++) {
            // Calculate a bit of wiggle!
            uint o_y = int(sinf((t / 3.0f) + s_x / 30.0f) * 8);
            // Step through the uint32 that represents a single column
            // of the current character, and render pixels for set bits.
            if(col & (1 << s_y)) {
                hub75.set_color(s_x, s_y + y + o_y, color);
            }
        }
        // Advance to the text pixel/character
        pixel++;
        if (pixel == letter_width) {
            pixel = 0;
            letter++;
            if (letter == text_length) letter = 0;
            c = text.at(letter);
        }
    }
}

int main() {
    vreg_set_voltage(VREG_VOLTAGE_1_20);
    sleep_us(100);
    set_sys_clock_khz(266000, true);

    hub75.start(dma_complete);

    std::string text = "  Hello World! How are you today?  ";

    // Basic loop to draw something to the screen.
    // This gets the distance from the middle of the display and uses it to paint a circular colour cycle.
    while (true) {
        hub75.background = hsv_to_rgb(millis() / 10000.0f, 1.0f, 0.5f);

        // Shadow
        scroll_text(text, HEIGHT / 2 - letter_height / 2 + 1, (millis() + 50) / 50.0f, Pixel(0, 0, 0));
        // Text
        scroll_text(text, HEIGHT / 2 - letter_height / 2, millis() / 50.0f, Pixel(255, 255, 255));

        hub75.flip(false); // Flip and clear to the background colour
        sleep_ms(1000 / 60);
    }
}
