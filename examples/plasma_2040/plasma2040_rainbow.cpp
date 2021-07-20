#include <stdio.h>
#include <math.h>
#include <cstdint>

#include "pico/stdlib.h"

#include "plasma2040.hpp"

#include "common/pimoroni_common.hpp"
#include "rgbled.hpp"
#include "button.hpp"

/*
Press "B" to speed up the LED cycling effect.
Press "A" to slow it down again.
*/

using namespace pimoroni;

// Set how many LEDs you have
const uint N_LEDS = 30;

// Pick *one* LED type by uncommenting the relevant line below:

// APA102-style LEDs with Data/Clock lines. AKA DotStar
//plasma::APA102 led_strip(N_LEDS, pio0, 0, plasma::PIN_DAT, plasma::PIN_CLK);

// WS28X-style LEDs with a single signal line. AKA NeoPixel
plasma::WS2812 led_strip(N_LEDS, pio0, 0, plasma::PIN_DAT);


Button button_a(plasma::BUTTON_A, Polarity::ACTIVE_LOW, 50);
Button button_b(plasma::BUTTON_B, Polarity::ACTIVE_LOW, 50);
RGBLED led(plasma::LED_R, plasma::LED_G, plasma::LED_B);


int main() {
    stdio_init_all();

    led_strip.start(60);

    int speed = 10;
    float offset = 0.0f;

    while (true) {
        bool a = button_a.read();
        bool b = button_b.read();
    
        if(a) speed--;
        if(b) speed++;
        speed = std::min((int)255, std::max((int)1, speed));

        offset += float(speed) / 2000.0f;

        for(auto i = 0u; i < led_strip.num_leds; ++i) {
            float hue = float(i) / led_strip.num_leds;
            led_strip.set_hsv(i, hue + offset, 1.0f, 1.0f);
        }

        led.set_rgb(speed, 0, 255 - speed);

        // Sleep time controls the rate at which the LED buffer is updated
        // but *not* the actual framerate at which the buffer is sent to the LEDs
        sleep_ms(1000 / 60);
    }
}
