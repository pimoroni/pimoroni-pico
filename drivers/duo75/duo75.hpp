#include <stdint.h>
#include "pico/stdlib.h"

#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "libraries/pico_graphics/pico_graphics.hpp"

#ifndef NO_QSTR
#include "duo75.pio.h"
#endif

namespace pimoroni {
const uint DATA_BASE_PIN = 0;
const uint DATA_N_PINS = 6;
const uint ROWSEL_BASE_PIN = 6;
const uint ROWSEL_N_PINS = 5;
const uint BIT_DEPTH = 10;

/*
10-bit gamma table, allowing us to gamma correct our 8-bit colour values up
to 10-bit without losing dynamic range.

Calculated with the following Python code:

gamma_lut = [int(round(1024 * (x / (256 - 1)) ** 2.2)) for x in range(256)]

Post-processed to enforce a minimum difference of 1 between adjacent values,
and no leading zeros:

for i in range(1, len(gamma_lut)):
	if gamma_lut[i] <= gamma_lut[i - 1]:
		gamma_lut[i] = gamma_lut[i - 1] + 1
*/
constexpr uint16_t GAMMA_10BIT[256] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 82, 84, 87, 89, 91, 94, 96, 98, 101, 103, 106, 109, 111, 114, 117,
    119, 122, 125, 128, 130, 133, 136, 139, 142, 145, 148, 151, 155, 158, 161, 164,
    167, 171, 174, 177, 181, 184, 188, 191, 195, 198, 202, 206, 209, 213, 217, 221,
    225, 228, 232, 236, 240, 244, 248, 252, 257, 261, 265, 269, 274, 278, 282, 287,
    291, 295, 300, 304, 309, 314, 318, 323, 328, 333, 337, 342, 347, 352, 357, 362,
    367, 372, 377, 382, 387, 393, 398, 403, 408, 414, 419, 425, 430, 436, 441, 447,
    452, 458, 464, 470, 475, 481, 487, 493, 499, 505, 511, 517, 523, 529, 535, 542,
    548, 554, 561, 567, 573, 580, 586, 593, 599, 606, 613, 619, 626, 633, 640, 647,
    653, 660, 667, 674, 681, 689, 696, 703, 710, 717, 725, 732, 739, 747, 754, 762,
    769, 777, 784, 792, 800, 807, 815, 823, 831, 839, 847, 855, 863, 871, 879, 887,
    895, 903, 912, 920, 928, 937, 945, 954, 962, 971, 979, 988, 997, 1005, 1014, 1023
};


struct Pixel {
    uint32_t color;
    constexpr Pixel() : color(0) {};
    constexpr Pixel(uint32_t color) : color(color) {};
    constexpr Pixel(uint8_t r, uint8_t g, uint8_t b) : color((GAMMA_10BIT[b] << 20) | (GAMMA_10BIT[g] << 10) | GAMMA_10BIT[r]) {};
};

enum PanelType {
    PANEL_GENERIC = 0,
    PANEL_FM6126A,
};

Pixel hsv_to_rgb(float h, float s, float v);

class Duo75 {
    public:
    enum class COLOR_ORDER {
        RGB,
        RBG,
        GRB,
        GBR,
        BRG,
        BGR
    };
    uint width;
    uint height;
    uint r_shift = 0;
    uint g_shift = 10;
    uint b_shift = 20;
    Pixel *back_buffer;
    bool managed_buffer = false;
    PanelType panel_type;
    bool inverted_stb = false;
    COLOR_ORDER color_order;
    Pixel background = 0;

    // DMA & PIO
    int dma_channel_a = -1;
    int dma_channel_b = -1;

    uint bit_a = 0;
    uint bit_b = 0;
    uint row_a = 0;
    uint row_b = 0;

    PIO pio_a = pio0;
    PIO pio_b = pio1;

    uint sm_data_a = 0;
    uint sm_row_a = 1;

    uint sm_data_b = 0;
    uint sm_row_b = 1;

    uint data_prog_offs_a = 0;
    uint row_prog_offs_a = 0;

    uint data_prog_offs_b = 0;
    uint row_prog_offs_b = 0;

    uint brightness = 6;

    // Top half of display - 16 rows on a 32x32 panel
    unsigned int pin_r0 = 0;
    unsigned int pin_g0 = 1;
    unsigned int pin_b0 = 2;

    // Bottom half of display - 16 rows on a 64x64 panel
    unsigned int pin_r1 = 3;
    unsigned int pin_g1 = 4;
    unsigned int pin_b1 = 5;

    // Address pins, 5 lines = 2^5 = 32 values (max 64x64 display)
    unsigned int pin_row_a = 6;
    unsigned int pin_row_b = 7;
    unsigned int pin_row_c = 8;
    unsigned int pin_row_d = 9;
    unsigned int pin_row_e = 10;

    // Sundry things
    unsigned int pin_clk = 11;    // Clock
    unsigned int pin_stb = 12;    // Strobe/Latch
    unsigned int pin_oe = 13;     // Output Enable

    const bool clk_polarity = 1;
    const bool stb_polarity = 1;
    const bool oe_polarity = 0;

    // User buttons and status LED
    unsigned int pin_sw_a = 14;
    unsigned int pin_sw_user = 23;

    unsigned int pin_led_r = 16;
    unsigned int pin_led_g = 17;
    unsigned int pin_led_b = 18;

    Duo75(uint width, uint height) : Duo75(width, height, nullptr) {};
    Duo75(uint width, uint height, Pixel *buffer) : Duo75(width, height, buffer, PANEL_GENERIC) {};
    Duo75(uint width, uint height, Pixel *buffer, PanelType panel_type) : Duo75(width, height, buffer, panel_type, false) {};
    Duo75(uint width, uint height, Pixel *buffer, PanelType panel_type, bool inverted_stb, COLOR_ORDER color_order=COLOR_ORDER::RGB);
    ~Duo75();

    void FM6126A_write_register(uint16_t value, uint8_t position);
    void FM6126A_setup();
    void set_color(uint x, uint y, Pixel c);

    void set_pixel(uint x, uint y, uint8_t r, uint8_t g, uint8_t b);
    void copy_to_back_buffer(void *data, size_t len, int start_x, int start_y);
    void display_update();
    void clear();
    void start(irq_handler_t handler);
    void stop(irq_handler_t handler);
    void dma_complete();
    void update(PicoGraphics *graphics);
    };
}