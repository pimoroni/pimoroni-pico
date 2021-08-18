#include <stdio.h>
#include <math.h>
#include <cstdint>

#include "pico/stdlib.h"

#include "plasma2040.hpp"

#include "common/pimoroni_common.hpp"
#include "breakout_encoder.hpp"
#include "rgbled.hpp"
#include "button.hpp"

using namespace pimoroni;

// Set how many LEDs you have
const uint N_LEDS = 30;

// How many times the LEDs will be updated per second
const uint UPDATES = 60;

// Pick *one* LED type by uncommenting the relevant line below:

// APA102-style LEDs with Data/Clock lines. AKA DotStar
//plasma::APA102 led_strip(N_LEDS, pio0, 0, plasma::PIN_DAT, plasma::PIN_CLK);

// WS28X-style LEDs with a single signal line. AKA NeoPixel
plasma::WS2812 led_strip(N_LEDS, pio0, 0, plasma::PIN_DAT);



Button button_a(plasma::BUTTON_A);
Button button_b(plasma::BUTTON_B);

RGBLED led(plasma::LED_R, plasma::LED_G, plasma::LED_B);

I2C i2c(BOARD::PICO_EXPLORER);
BreakoutEncoder enc(&i2c);

enum ENCODER_MODE {
    COLOUR,
    ANGLE,
    BRIGHTNESS,
    TIME
};


void colour_cycle(float hue, float t, float angle) {
    t /= 200.0f;

    for (auto i = 0u; i < led_strip.num_leds; ++i) {
        float offset = (M_PI * i) / led_strip.num_leds;
        offset = sinf(offset + t) * angle;
        led_strip.set_hsv(i, (hue + offset) / 360.0f, 1.0f, 1.0f);
    }
}

void gauge(uint v, uint vmax = 100) {
    uint light_pixels = led_strip.num_leds * v / vmax;

    for (auto i = 0u; i < led_strip.num_leds; ++i) {
        if(i < light_pixels) {
            led_strip.set_rgb(i, 0, 255, 0);
        } else {
            led_strip.set_rgb(i, 255, 0, 0);
        }
    }
}

int main() {
    stdio_init_all();

    led_strip.start(UPDATES);

    bool encoder_detected = enc.init();
    enc.clear_interrupt_flag();

    int speed = 50;
    float hue = 0;
    int angle = 120;
    int8_t brightness = 16;
    bool cycle = true;
    ENCODER_MODE mode = ENCODER_MODE::COLOUR;
    while (true) {
        uint32_t t = millis();
        if(encoder_detected) {
            if(enc.get_interrupt_flag()) {
                int count = enc.read();
                enc.clear_interrupt_flag();
                enc.clear();

                cycle = false;
                switch(mode) {
                    case ENCODER_MODE::COLOUR:
                        hue += count;
                        brightness = std::min((int8_t)359, brightness);
                        brightness = std::max((int8_t)0, brightness);
                        colour_cycle(hue, 0, (float)angle);
                        break;
                    case ENCODER_MODE::ANGLE:
                        angle += count;
                        angle = std::min((int)359, angle);
                        angle = std::max((int)0, angle);
                        colour_cycle(hue, 0, (float)angle);
                        break;
                    case ENCODER_MODE::BRIGHTNESS:
                        brightness += count;
                        brightness = std::min((int8_t)31, brightness);
                        brightness = std::max((int8_t)0, brightness);
                        led_strip.set_brightness(brightness);
                        gauge(brightness, 31);
                        break;
                    case ENCODER_MODE::TIME:
                        speed += count;
                        speed = std::min((int)100, speed);
                        speed = std::max((int)0, speed);
                        gauge(speed, 100);
                        break;
                }
            }
        }
        bool a_pressed = button_a.read();
        bool b_pressed = button_b.read();

        if(b_pressed) cycle = true;

        switch(mode) {
            case ENCODER_MODE::COLOUR:
                led.set_rgb(255, 0, 0);
                if(a_pressed) mode = ENCODER_MODE::ANGLE;
                break;
            case ENCODER_MODE::ANGLE:
                led.set_rgb(255, 255, 0);
                if(a_pressed) mode = ENCODER_MODE::BRIGHTNESS;
                break;
            case ENCODER_MODE::BRIGHTNESS:
                led.set_rgb(0, 255, 0);
                if(a_pressed) mode = ENCODER_MODE::TIME;
                break;
            case ENCODER_MODE::TIME:
                led.set_rgb(0, 0, 255);
                if(a_pressed) mode = ENCODER_MODE::COLOUR;
                break;
        }

        if(cycle) colour_cycle(hue, t * speed / 100, (float)angle);

        auto first_led = led_strip.get(0);
        enc.set_led(first_led.r, first_led.g, first_led.b);

        // Sleep time controls the rate at which the LED buffer is updated
        // but *not* the actual framerate at which the buffer is sent to the LEDs
        sleep_ms(1000 / UPDATES);
    }
}
