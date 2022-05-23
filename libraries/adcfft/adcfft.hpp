#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cstring>

#include "pico/stdlib.h"

#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/adc.h"
#include "hardware/irq.h"

typedef signed int fix15;

// Helpers for 16.15 fixed-point arithmetic
constexpr __always_inline fix15 multiply_fix15(fix15 a, fix15 b) {return (fix15)(((signed long long)(a) * (signed long long)(b)) >> 15);}
constexpr __always_inline fix15 float_to_fix15(float a) {return (fix15)(a * 32768.0f);}
constexpr __always_inline float fix15_to_float(fix15 a) {return (float)(a) / 32768.0f;}
constexpr __always_inline fix15 int_to_fix15(int a) {return (fix15)(a << 15);}
constexpr __always_inline int fix15_to_int(fix15 a) {return (int)(a >> 15);}

constexpr unsigned int SAMPLE_COUNT = 512u;

class ADCFFT {
    private:
    
        unsigned int adc_channel;
        unsigned int adc_pin;
        float sample_rate;

        unsigned int log2_samples;
        unsigned int shift_amount;

        int dma_channel;

        // Here's where we'll have the DMA channel put ADC samples
        uint8_t sample_array[SAMPLE_COUNT];

        // Lookup tables
        fix15 sine_table[SAMPLE_COUNT];    // a table of sines for the FFT
        fix15 filter_window[SAMPLE_COUNT]; // a table of window values for the FFT

        // And here's where we'll copy those samples for FFT calculation
        fix15 fr[SAMPLE_COUNT];
        fix15 fi[SAMPLE_COUNT];

        int max_freq_dex = 0;
        
        void FFT();
        void init();
    public:
        ADCFFT() : ADCFFT(0, 26, 10000.0f) {};
        ADCFFT(unsigned int adc_channel, unsigned int adc_pin) : ADCFFT(adc_channel, adc_pin, 10000.0f) {}
        ADCFFT(unsigned int adc_channel, unsigned int adc_pin, float sample_rate) :
            adc_channel(adc_channel), adc_pin(adc_pin), sample_rate(sample_rate) {
                log2_samples = log2(SAMPLE_COUNT);
                shift_amount = 16u - log2_samples;

                dma_channel = dma_claim_unused_channel(true);

                memset(sample_array, 0, SAMPLE_COUNT);

                memset(fr, 0, SAMPLE_COUNT * sizeof(fix15));
                memset(fi, 0, SAMPLE_COUNT * sizeof(fix15));

                init();
        };
        ~ADCFFT();

        void update();
        float max_frequency();
        int get_scaled(unsigned int i, unsigned int scale);
};