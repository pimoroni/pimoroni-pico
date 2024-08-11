#include <stdint.h>
#include "pico/stdlib.h"

#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "libraries/pico_graphics/pico_graphics.hpp"

#ifndef NO_QSTR
#include "hub75.pio.h"
#endif

namespace pimoroni {
const uint DATA_BASE_PIN = 0;
const uint DATA_N_PINS = 6;
const uint ROWSEL_BASE_PIN = 6;
const uint ROWSEL_N_PINS = 5;
const uint BIT_DEPTH = 10;

// This gamma table is used to correct our 8-bit (0-255) colours up to 11-bit,
// allowing us to gamma correct without losing dynamic range.
constexpr uint16_t GAMMA_10BIT[256] = {
    0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
    8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16,
    16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 25,
    26, 27, 29, 30, 31, 33, 34, 35, 37, 38, 40, 41, 43, 44, 46, 47,
    49, 51, 53, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78,
    80, 82, 85, 87, 89, 92, 94, 96, 99, 101, 104, 106, 109, 112, 114, 117,
    120, 122, 125, 128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 161, 164,
    168, 171, 174, 178, 181, 185, 188, 192, 195, 199, 202, 206, 210, 214, 217, 221,
    225, 229, 233, 237, 241, 245, 249, 253, 257, 261, 265, 270, 274, 278, 283, 287,
    291, 296, 300, 305, 309, 314, 319, 323, 328, 333, 338, 343, 347, 352, 357, 362,
    367, 372, 378, 383, 388, 393, 398, 404, 409, 414, 420, 425, 431, 436, 442, 447,
    453, 459, 464, 470, 476, 482, 488, 494, 499, 505, 511, 518, 524, 530, 536, 542,
    548, 555, 561, 568, 574, 580, 587, 593, 600, 607, 613, 620, 627, 633, 640, 647,
    654, 661, 668, 675, 682, 689, 696, 703, 711, 718, 725, 733, 740, 747, 755, 762,
    770, 777, 785, 793, 800, 808, 816, 824, 832, 839, 847, 855, 863, 872, 880, 888,
    896, 904, 912, 921, 929, 938, 946, 954, 963, 972, 980, 989, 997, 1006, 1015, 1023
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

class Hub75 {
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
    Pixel *back_buffer;
    bool managed_buffer = false;
    PanelType panel_type;
    bool inverted_stb = false;
    COLOR_ORDER color_order;
    Pixel background = 0;

    // DMA & PIO
    uint dma_channel = 0;
    uint bit = 0;
    uint row = 0;

    PIO pio = pio0;
    uint sm_data = 0;
    uint sm_row = 1;

    uint data_prog_offs = 0;
    uint row_prog_offs = 0;

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

    Hub75(uint width, uint height) : Hub75(width, height, nullptr) {};
    Hub75(uint width, uint height, Pixel *buffer) : Hub75(width, height, buffer, PANEL_GENERIC) {};
    Hub75(uint width, uint height, Pixel *buffer, PanelType panel_type) : Hub75(width, height, buffer, panel_type, false) {};
    Hub75(uint width, uint height, Pixel *buffer, PanelType panel_type, bool inverted_stb, COLOR_ORDER color_order=COLOR_ORDER::RGB);
    ~Hub75();

    void FM6126A_write_register(uint16_t value, uint8_t position);
    void FM6126A_setup();
    void set_color(uint x, uint y, Pixel c);

    void set_pixel(uint x, uint y, uint8_t r, uint8_t g, uint8_t b);
    void display_update();
    void clear();
    void start(irq_handler_t handler);
    void stop(irq_handler_t handler);
    void dma_complete();
    void update(PicoGraphics *graphics);
    };
}