#include "pico/stdlib.h"

#include <algorithm>
#include <math.h>
#include <iostream>
#include <deque>

#include "mlx90640.hpp"
#include "st7789.hpp"

using namespace std;
using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);

MLX90640 mlx90640(&i2c);
ST7789 st7789(240, 240, pimoroni::ROTATE_90, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB565 graphics(st7789.width, st7789.height, nullptr);
Pen BLACK = graphics.create_pen(0, 0, 0);

const uint8_t PIXEL_SIZE = 7;
const uint8_t X_MARGIN = 8;
const uint8_t Y_MARGIN = 36;

struct Range {
    float min;
    float max;
};

void write_pixel(uint8_t cam_x, uint8_t cam_y) {
    auto x_base = cam_x * PIXEL_SIZE + X_MARGIN;
    auto y_base = cam_y * PIXEL_SIZE + Y_MARGIN;

    for (auto x = x_base; x < x_base + PIXEL_SIZE; x++) {
        for (auto y = y_base; y < y_base + PIXEL_SIZE; y++) {
            graphics.set_pixel(Point(x, y));
        }
    }
}

Range get_frame_temp_range(float frame[24 * 32]) {
    auto min = frame[0];
    auto max = frame[0];

    for (auto i = 1; i < 768; i++) {
        min = std::min(min, frame[i]);
        max = std::max(max, frame[i]);
    }

    return {min, max};
}

Range get_abs_range(deque<Range> buf) {
    auto min = buf[0].min;
    auto max = buf[0].max;

    for (uint8_t i = 1; i < buf.size(); i++) {
        min = floor(std::min(min, buf[i].min));
        max = ceil(std::max(max, buf[i].max));
    }

    return {min, max};
}

float get_temp_gradient(float temp, Range range) {
    return (temp - range.min) / (range.max - range.min);
}

void set_pen_grayscale(float g) {
    auto v = g * 255;

    graphics.set_pen(v, v, v);
}

void set_pen_hue(float h) {
    h = std::fmod(h, 1.0f);
    if (h < 0) {
        h += 1.0f;
    }
    h *= 6;

    float kr = fmod(5 + h, 6);
    float kg = fmod(3 + h, 6);
    float kb = fmod(1 + h, 6);

    auto r = 1 - max(0.0f, min(1.0f, min(4 - kr, kr)));
    auto g = 1 - max(0.0f, min(1.0f, min(4 - kg, kg)));
    auto b = 1 - max(0.0f, min(1.0f, min(4 - kb, kb)));

    graphics.set_pen(r * 255, g * 255, b * 255);
}

int main() {
    stdio_init_all();
    st7789.set_backlight(128);

    // Any higher framerate and it will require weird Pico overclocking
    mlx90640.setup(8);

//    while (!stdio_usb_connected()) {
//        sleep_ms(1);
//    }
//    cout << "--- Connected ---" << endl;
    sleep_ms(500); // Don't read the initial frames

    deque<Range> ranges;

    while (true) {
        mlx90640.get_frame();
        graphics.set_pen(BLACK);
        graphics.clear();


        // Circular buffer to keep track of the temp range
        ranges.push_back(get_frame_temp_range(mlx90640.mlx90640To));
        while (ranges.size() > 64) {
            ranges.pop_front();
        }
        auto range = get_abs_range(ranges);

        for (auto y = 0u; y < 24; y++) {
            for (auto x = 0u; x < 32; x++) {
                // Usable temperature values as-is
                uint16_t offset = y * 32 + x;
                float temp = mlx90640.mlx90640To[offset];

                // Convert value to within the range of [0, 1]
                auto g = get_temp_gradient(temp, range);
                // Hacky way to flip the spectrum and then discard the part where it transitions from blue to red
                set_pen_hue(2 * (1 - g) / 3);
                // set_pen_grayscale(g);

                // Convert cam-pixel to display-pixel
                write_pixel(x, y);
            }
        }

        st7789.update(&graphics);
    }
}

