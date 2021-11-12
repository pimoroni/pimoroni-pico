#include <stdio.h>
#include <math.h>
#include <cstdint>
#include <cstring>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/vreg.h"

#include "common/pimoroni_common.hpp"

using namespace pimoroni;

/* Interstate 75 - HUB75 from basic principles.

While trying to get our I75 running I soon realised that I couldn't find any documentation or basics on the HUB75 protocol.

Yes there were libraries and tidbits that were seemingly loosely related, but nothing really laying out clear steps to update the 32x32 display on my desk.

So I wrote this.

This code may not be technically correct, but the results are visually appealing and fast enough.

More importantly, the code is legible and doesn't hide any implementation details beneath the inscrutable veneer of PIO.

No interpolators, no DMA, no PIO here- if you want a beautifully optimised library for HUB75 output then this... isn't it.

Just pure C. Pure CPU. Running on RP2040s Core1.

*/

// Display size in pixels
// Should be either 64x64 or 32x32 but perhaps 64x32 an other sizes will work.
// Note: this example uses only 5 address lines so it's limited to 32*2 pixels.
const uint8_t WIDTH = 64;
const uint8_t HEIGHT = 64;

// Settings below are correct for I76, change them to suit your setup:

// Top half of display - 16 rows on a 32x32 panel
const uint PIN_R0 = 0;
const uint PIN_G0 = 1;
const uint PIN_B0 = 2;

// Bottom half of display - 16 rows on a 64x64 panel
const uint PIN_R1 = 3;
const uint PIN_G1 = 4;
const uint PIN_B1 = 5;

// Address pins, 5 lines = 2^5 = 32 values (max 64x64 display)
const uint PIN_ROW_A = 6;
const uint PIN_ROW_B = 7;
const uint PIN_ROW_C = 8;
const uint PIN_ROW_D = 9;
const uint PIN_ROW_E = 10;

// Sundry things
const uint PIN_CLK = 11;    // Clock
const uint PIN_STB = 12;    // Strobe/Latch
const uint PIN_OE = 13;     // Output Enable

const bool CLK_POLARITY = 1;
const bool STB_POLARITY = 1;
const bool OE_POLARITY = 0;

// User buttons and status LED
const uint PIN_SW_A = 14;
const uint PIN_SW_USER = 23;

const uint PIN_LED_R = 16;
const uint PIN_LED_G = 17;
const uint PIN_LED_B = 18;

volatile bool flip = false;

// This gamma table is used to correct our 8-bit (0-255) colours up to 11-bit,
// allowing us to gamma correct without losing dynamic range.
const uint16_t GAMMA_12BIT[256] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 47, 50,
    52, 54, 57, 59, 62, 65, 67, 70, 73, 76, 79, 82, 85, 88, 91, 94,
     98, 101, 105, 108, 112, 115, 119, 123, 127, 131, 135, 139, 143, 147, 151, 155,
    160, 164, 169, 173, 178, 183, 187, 192, 197, 202, 207, 212, 217, 223, 228, 233,
    239, 244, 250, 255, 261, 267, 273, 279, 285, 291, 297, 303, 309, 316, 322, 328,
    335, 342, 348, 355, 362, 369, 376, 383, 390, 397, 404, 412, 419, 427, 434, 442,
    449, 457, 465, 473, 481, 489, 497, 505, 513, 522, 530, 539, 547, 556, 565, 573,
    582, 591, 600, 609, 618, 628, 637, 646, 656, 665, 675, 685, 694, 704, 714, 724,
    734, 744, 755, 765, 775, 786, 796, 807, 817, 828, 839, 850, 861, 872, 883, 894,
    905, 917, 928, 940, 951, 963, 975, 987, 998, 1010, 1022, 1035, 1047, 1059, 1071, 1084,
    1096, 1109, 1122, 1135, 1147, 1160, 1173, 1186, 1199, 1213, 1226, 1239, 1253, 1266, 1280, 1294,
    1308, 1321, 1335, 1349, 1364, 1378, 1392, 1406, 1421, 1435, 1450, 1465, 1479, 1494, 1509, 1524,
    1539, 1554, 1570, 1585, 1600, 1616, 1631, 1647, 1663, 1678, 1694, 1710, 1726, 1743, 1759, 1775,
    1791, 1808, 1824, 1841, 1858, 1875, 1891, 1908, 1925, 1943, 1960, 1977, 1994, 2012, 2029, 2047};


// We don't *need* to make Pixel a fancy struct with RGB values, but it helps.
#pragma pack(push, 1)
struct alignas(4) Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t _;
    constexpr Pixel() : r(0), g(0), b(0), _(0) {};
    constexpr Pixel(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), _(0) {};
    constexpr Pixel(float r, float g, float b) : r((uint8_t)(r * 255.0f)), g((uint8_t)(g * 255.0f)), b((uint8_t)(b * 255.0f)), _(0) {};
};
#pragma pack(pop)

// Create our front and back buffers.
// We'll draw into the frontbuffer and then copy everything into the backbuffer which will be used to refresh the screen.
// Double-buffering the display avoids screen tearing with fast animations or slow update rates.
Pixel backbuffer[WIDTH][HEIGHT];
Pixel frontbuffer[WIDTH][HEIGHT];

// Basic function to convert Hue, Saturation and Value to an RGB colour
Pixel hsv_to_rgb(float h, float s, float v) {
    if(h < 0.0f) {
        h = 1.0f + fmod(h, 1.0f);
    }

    int i = int(h * 6);
    float f = h * 6 - i;

    v = v * 255.0f;

    float sv = s * v;
    float fsv = f * sv;

    auto p = uint8_t(-sv + v);
    auto q = uint8_t(-fsv + v);
    auto t = uint8_t(fsv - sv + v);

    uint8_t bv = uint8_t(v);

    switch (i % 6) {
        default:
        case 0: return Pixel(bv, t, p);
        case 1: return Pixel(q, bv, p);
        case 2: return Pixel(p, bv, t);
        case 3: return Pixel(p, q, bv);
        case 4: return Pixel(t, p, bv);
        case 5: return Pixel(bv, p, q);
    }
}

// Required for FM6126A-based displays which need some register config/init to work properly
void FM6126A_write_register(uint16_t value, uint8_t position) {
    uint8_t threshold = WIDTH - position;
    for(auto i = 0u; i < WIDTH; i++) {
        auto j = i % 16;
        bool b = value & (1 << j);
        gpio_put(PIN_R0, b);
        gpio_put(PIN_G0, b);
        gpio_put(PIN_B0, b);
        gpio_put(PIN_R1, b);
        gpio_put(PIN_G1, b);
        gpio_put(PIN_B1, b);

        // Assert strobe/latch if i > threshold
        // This somehow indicates to the FM6126A which register we want to write :|
        gpio_put(PIN_STB, i > threshold);
        gpio_put(PIN_CLK, CLK_POLARITY);
        sleep_us(10);
        gpio_put(PIN_CLK, !CLK_POLARITY);
    }
}

void hub75_flip () {
    flip = true; // TODO: rewrite to semaphore
}

void hub75_display_update() {
    // Ridiculous register write nonsense for the FM6126A-based 64x64 matrix
    FM6126A_write_register(0b1111111111111110, 12);
    FM6126A_write_register(0b0000001000000000, 13);

    while (true) {
        // 0. Copy the contents of the front buffer into our backbuffer for output to the display.
        //    This uses another whole backbuffer worth of memory, but prevents visual tearing at low frequencies.
        if (flip) {
            memcpy((uint8_t *)backbuffer, (uint8_t *)frontbuffer, WIDTH * HEIGHT * sizeof(Pixel));
            flip = false;
        }

        // Step through 0b00000001, 0b00000010, 0b00000100 etc
        for(auto bit = 1u; bit < 1 << 11; bit <<= 1) {
            // Since the display is in split into two equal halves, we step through y from 0 to HEIGHT / 2
            for(auto y = 0u; y < HEIGHT / 2; y++) {

                // 1. Shift out pixel data
                //    Shift out WIDTH pixels to the top and bottom half of the display
                for(auto x = 0u; x < WIDTH; x++) {
                    // Get the current pixel for top/bottom half
                    // This is easy since we just need the pixels at X/Y and X/Y+HEIGHT/2
                    Pixel pixel_top     = backbuffer[x][y];
                    Pixel pixel_bottom  = backbuffer[x][y + HEIGHT / 2];

                    // Gamma correct the colour values from 8-bit to 11-bit
                    uint16_t pixel_top_b  = GAMMA_12BIT[pixel_top.b];
                    uint16_t pixel_top_g  = GAMMA_12BIT[pixel_top.g];
                    uint16_t pixel_top_r  = GAMMA_12BIT[pixel_top.r];
            
                    uint16_t pixel_bottom_b = GAMMA_12BIT[pixel_bottom.b];
                    uint16_t pixel_bottom_g = GAMMA_12BIT[pixel_bottom.g];
                    uint16_t pixel_bottom_r = GAMMA_12BIT[pixel_bottom.r];
    
                    // Set the clock low while we set up the data pins
                    gpio_put(PIN_CLK, !CLK_POLARITY);

                    // Top half
                    gpio_put(PIN_R0, (bool)(pixel_top_r & bit));
                    gpio_put(PIN_G0, (bool)(pixel_top_g & bit));
                    gpio_put(PIN_B0, (bool)(pixel_top_b & bit));

                    // Bottom half
                    gpio_put(PIN_R1, (bool)(pixel_bottom_r & bit));
                    gpio_put(PIN_G1, (bool)(pixel_bottom_g & bit));
                    gpio_put(PIN_B1, (bool)(pixel_bottom_b & bit));

                    // Wiggle the clock
                    // The gamma correction above will ensure our clock stays asserted
                    // for some small amount of time, avoiding the need for an explicit delay.
                    gpio_put(PIN_CLK, CLK_POLARITY);
                }


                // 2. Set address pins
                //    Set the address pins to reflect the row to light up: 0 through 15 for 32x32 pixel panels
                //    We decode our 5-bit row address out onto the 5 GPIO pins by masking each bit in turn.
                gpio_put_masked(0b11111 << PIN_ROW_A, y << PIN_ROW_A);

                // 3. Assert latch/strobe signal (STB)
                //    This latches all the values we've just clocked into the column shift registers.
                //    The values will appear on the output pins, ready for the display to be driven.
                gpio_put(PIN_STB, STB_POLARITY);

                // 4. Asset the output-enable signal (OE)
                //    This turns on the display for a brief period to light the selected rows/columns.
                gpio_put(PIN_OE, OE_POLARITY);

                // 4. Delay
                //    Delay for a period of time coressponding to "bit"'s significance
                for(auto s = 0u; s < bit; ++s) {
                    // The basic premise here is that "bit" will step through the values:
                    // 1, 2, 4, 8, 16, 32, 64, etc in sequence.
                    // If we plug this number into a delay loop, we'll get different magnitudes
                    // of delay which correspond exactly to the significance of each bit.
                    // The longer we delay here, the slower the overall panel refresh rate will be.
                    // But we need to delay *just enough* that we're not under-driving the panel and
                    // losing out on brightness.
                    asm volatile("nop \nnop"); // Batman!
                }

                // 5. De-assert latch/strobe signal (STB) + output-enable signal (OE)
                //    Ready to go again!
                gpio_put(PIN_STB, !STB_POLARITY);
                gpio_put(PIN_OE, !OE_POLARITY);

                // 6. GOTO 1.
            }
            sleep_us(1);
        }
    }
}

int main() {
    // 1.3v allows overclock to ~280000-300000 but YMMV. Faster clock = faster screen update rate!
    // vreg_set_voltage(VREG_VOLTAGE_1_30);
    // sleep_ms(100);

    // 200MHz is roughly about the lower limit for driving a 64x64 display smoothly.
    // Just don't look at it out of the corner of your eye.
    set_sys_clock_khz(200000, false);

    // Set up allllll the GPIO
    gpio_init(PIN_R0); gpio_set_function(PIN_R0, GPIO_FUNC_SIO); gpio_set_dir(PIN_R0, true);
    gpio_init(PIN_G0); gpio_set_function(PIN_G0, GPIO_FUNC_SIO); gpio_set_dir(PIN_G0, true);
    gpio_init(PIN_B0); gpio_set_function(PIN_B0, GPIO_FUNC_SIO); gpio_set_dir(PIN_B0, true);

    gpio_init(PIN_R1); gpio_set_function(PIN_R1, GPIO_FUNC_SIO); gpio_set_dir(PIN_R1, true);
    gpio_init(PIN_G1); gpio_set_function(PIN_G1, GPIO_FUNC_SIO); gpio_set_dir(PIN_G1, true);
    gpio_init(PIN_B1); gpio_set_function(PIN_B1, GPIO_FUNC_SIO); gpio_set_dir(PIN_B1, true);

    gpio_init(PIN_ROW_A); gpio_set_function(PIN_ROW_A, GPIO_FUNC_SIO); gpio_set_dir(PIN_ROW_A, true);
    gpio_init(PIN_ROW_B); gpio_set_function(PIN_ROW_B, GPIO_FUNC_SIO); gpio_set_dir(PIN_ROW_B, true);
    gpio_init(PIN_ROW_C); gpio_set_function(PIN_ROW_C, GPIO_FUNC_SIO); gpio_set_dir(PIN_ROW_C, true);
    gpio_init(PIN_ROW_D); gpio_set_function(PIN_ROW_D, GPIO_FUNC_SIO); gpio_set_dir(PIN_ROW_D, true);
    gpio_init(PIN_ROW_E); gpio_set_function(PIN_ROW_E, GPIO_FUNC_SIO); gpio_set_dir(PIN_ROW_E, true);

    gpio_init(PIN_CLK); gpio_set_function(PIN_CLK, GPIO_FUNC_SIO); gpio_set_dir(PIN_CLK, true);
    gpio_init(PIN_STB); gpio_set_function(PIN_STB, GPIO_FUNC_SIO); gpio_set_dir(PIN_STB, true);
    gpio_init(PIN_OE); gpio_set_function(PIN_OE, GPIO_FUNC_SIO); gpio_set_dir(PIN_OE, true);

    // Launch the display update routine on Core 1, it's hungry for cycles!
    multicore_launch_core1(hub75_display_update);

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
                frontbuffer[x][y] = hsv_to_rgb(h, 1.0f, 1.0f);
            }
        }

        hub75_flip();

        sleep_ms(1000 / 60);
    }
}
