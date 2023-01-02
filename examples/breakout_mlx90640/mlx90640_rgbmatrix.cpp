#include "pico/stdlib.h"
#include "hardware/vreg.h"
#include "stdio.h"

#include <algorithm>
#include <cmath>

#include "mlx90640.hpp"

#include "hub75.hpp"

using namespace pimoroni;

// Display size in pixels
// Should be either 64x64 or 32x32 but perhaps 64x32 an other sizes will work.
// Note: this example uses only 5 address lines so it's limited to 32*2 pixels.
const uint8_t WIDTH = 32;
const uint8_t HEIGHT = 32;

// min and max temperature range (in degrees C) for scaling false colour
const float temp_min = 18.0f;
const float temp_max = 38.0f;

// colour brightness - crushes dynamic range so use wisely!
const float brightness = 0.5f;

Hub75 hub75(WIDTH, HEIGHT, nullptr);

// Dirty hack to overclock the Pico before class initialisation takes place
// since i2c uses the current clock frequency when determining baudrate.
class OC {
    public:
    OC(uint32_t freq_khz, vreg_voltage voltage) {
        vreg_set_voltage(voltage);
        sleep_us(100);
        set_sys_clock_khz(freq_khz, true);
    }
};

OC oc(266000, VREG_VOLTAGE_1_20);

// 1MHz i2c for higher framerates
I2C i2c(20, 21, 1000000UL);

MLX90640 mlx90640(&i2c);

void __isr dma_complete() {
    hub75.dma_complete();
}

void set_pixel_false_colour(int x, int y, float v) {
    const int colours = 8;
    static float color[colours][3] = {
        {0,     0,     0},
        {0,     0,     255.0f},
        {0,     255.0f,255.0f},
        {0,     255.0f,0},
        {255.0f,255.0f,0},
        {255.0f,0,     0},
        {255.0f,0,     255.0f},
        {255.0f,255.0f,255.0f}
    };
    int idx1, idx2;
    float blend = 0.0f;
    const float temp_range = temp_max - temp_min;
    v -= temp_min;
    v /= temp_range;
    if(v <= 0) {idx1 = idx2 = 0;}
    else if(v >= 1) {idx1 = idx2 = colours - 1;}
    else
    {
        v *= (colours - 1);
        idx1 = std::floor(v);
        idx2 = idx1 + 1;
        blend = v - float(idx1);
    }

    int r = (int)((((color[idx2][0] - color[idx1][0]) * blend) + color[idx1][0]) * brightness);
    int g = (int)((((color[idx2][1] - color[idx1][1]) * blend) + color[idx1][1]) * brightness);
    int b = (int)((((color[idx2][2] - color[idx1][2]) * blend) + color[idx1][2]) * brightness);

    hub75.set_rgb(x, y, r, g, b);
}

  int main() {
    stdio_init_all();
    hub75.start(dma_complete);

    mlx90640.setup(32);

    while(true) {
        mlx90640.get_frame();
        for(auto y = 0u; y < 24; y++) {
            for(auto x = 0u; x < 32; x++) {
                int offset = y * 32 + x;
                float v = mlx90640.mlx90640To[offset]; // / 30000.0f;
                set_pixel_false_colour(x, y, v);
            }
        }
        hub75.flip();
    }

    return 0;
}
