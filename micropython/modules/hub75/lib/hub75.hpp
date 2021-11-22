#include <stdint.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hub75.pio.h"

const uint DATA_BASE_PIN = 0;
const uint DATA_N_PINS = 6;
const uint ROWSEL_BASE_PIN = 6;
const uint ROWSEL_N_PINS = 5;

// This gamma table is used to correct our 8-bit (0-255) colours up to 11-bit,
// allowing us to gamma correct without losing dynamic range.
constexpr uint16_t GAMMA_12BIT[256] = {
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
    uint16_t _;
    uint16_t r;
    uint16_t g;
    uint16_t b;
    constexpr Pixel() : _(0), r(0), g(0), b(0) {};
    constexpr Pixel(uint8_t r, uint8_t g, uint8_t b) : _(0), r(GAMMA_12BIT[r]), g(GAMMA_12BIT[g]), b(GAMMA_12BIT[b]) {};
};
#pragma pack(pop)

class Hub75 {
    public:
    uint8_t width;
    uint8_t height;
    Pixel *front_buffer;
    Pixel *back_buffer;
    bool running = false;

    // DMA & PIO
    uint dma_channel = 0;
    volatile bool do_flip = false;
    uint bit = 0;
    uint row = 0;

    PIO pio = pio0;
    uint sm_data = 0;
    uint sm_row = 1;

    uint data_prog_offs = 0;
    uint row_prog_offs = 0;


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

    Hub75(uint8_t width, uint8_t height, Pixel *buffer);
    ~Hub75();

    void FM6126A_write_register(uint16_t value, uint8_t position);
    void set_rgb(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
    void set_hsv(uint8_t x, uint8_t y, float r, float g, float b);
    void display_update();
    void clear();
    void start(irq_handler_t handler);
    void stop(irq_handler_t handler);
    void flip();
    void dma_complete();
};