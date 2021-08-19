#include <stdio.h>
#include <math.h>
#include <cstdint>

#include "pico/stdlib.h"

#include "plasma2040.hpp"

#include "common/pimoroni_common.hpp"
#include "rgbled.hpp"
#include "button.hpp"
#include "analog.hpp"

/*
Press "B" to speed up the LED cycling effect.
Press "A" to slow it down again.
Press "Boot" to reset the speed back to default.
*/

using namespace pimoroni;
using namespace plasma;

// Set how many LEDs you have
const uint N_LEDS = 30;

// The speed that the LEDs will start cycling at
const uint DEFAULT_SPEED = 10;

// How many times the LEDs will be updated per second
const uint UPDATES = 60;


// Pick *one* LED type by uncommenting the relevant line below:

// APA102-style LEDs with Data/Clock lines. AKA DotStar
//APA102 led_strip(N_LEDS, pio0, 0, plasma2040::DAT, plasma2040::CLK);

// WS28X-style LEDs with a single signal line. AKA NeoPixel
// by default the WS2812 LED strip will be 400KHz, RGB with no white element
WS2812 led_strip(N_LEDS, pio0, 0, plasma2040::DAT);

Button user_sw(plasma2040::USER_SW, Polarity::ACTIVE_LOW, 0);
Button button_a(plasma2040::BUTTON_A, Polarity::ACTIVE_LOW, 50);
Button button_b(plasma2040::BUTTON_B, Polarity::ACTIVE_LOW, 50);
RGBLED led(plasma2040::LED_R, plasma2040::LED_G, plasma2040::LED_B);
Analog sense(plasma2040::CURRENT_SENSE, plasma2040::ADC_GAIN, plasma2040::SHUNT_RESISTOR);


int main() {
    stdio_init_all();

    led_strip.start(UPDATES);

    int speed = DEFAULT_SPEED;
    float offset = 0.0f;

    uint count = 0;
    while (true) {
        bool sw = user_sw.read();
        bool a = button_a.read();
        bool b = button_b.read();

        if(sw) {
            speed = DEFAULT_SPEED;
        }
        else {
            if(a) speed--;
            if(b) speed++;
        }
        speed = std::min((int)255, std::max((int)1, speed));

        offset += float(speed) / 2000.0f;

        for(auto i = 0u; i < led_strip.num_leds; ++i) {
            float hue = float(i) / led_strip.num_leds;
            led_strip.set_hsv(i, hue + offset, 1.0f, 1.0f);
        }

        led.set_rgb(speed, 0, 255 - speed);

        count += 1;
        if(count >= UPDATES) {
            // Display the current value once every second
            printf("Current = %f A\n", sense.read_current());
            count = 0;
        }

        // Sleep time controls the rate at which the LED buffer is updated
        // but *not* the actual framerate at which the buffer is sent to the LEDs
        sleep_ms(1000 / UPDATES);
    }
}
