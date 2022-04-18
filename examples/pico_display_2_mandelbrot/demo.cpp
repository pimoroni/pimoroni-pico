#define MULTICORE

#include <math.h>
#include "pico/stdlib.h"
#if defined(MULTICORE)
#include "pico/multicore.h"
#endif
#include "libraries/pico_display_2/pico_display_2.hpp"


using namespace pimoroni;

typedef int32_t fixed_t;

class complex_fixed_t {
public:
    complex_fixed_t() {}
    complex_fixed_t(fixed_t _r, fixed_t _i) : r(_r), i(_i) {}
public:
    fixed_t r;
    fixed_t i;
};
inline bool operator==(const complex_fixed_t& lhs, const complex_fixed_t& rhs){ return lhs.r == rhs.r && lhs.i == rhs.i; }

#define FXD_FRACTIONAL_BITS 25

#define FXD_FROM_INT(x) ((x) << FXD_FRACTIONAL_BITS)
static inline fixed_t float_to_fixed(float x) { return static_cast<fixed_t>(x * static_cast<float>(1u << FXD_FRACTIONAL_BITS)); }
static inline float fixed_to_float(fixed_t x) { return static_cast<float>(x) / static_cast<float>(1u << FXD_FRACTIONAL_BITS); }
static inline fixed_t fixed_multiply(fixed_t a, fixed_t b) {
    const int64_t r = static_cast<int64_t>(a) * static_cast<int64_t>(b);
    return static_cast<int32_t>(r >> FXD_FRACTIONAL_BITS);
}
#define FXD_MUL(x, y) fixed_multiply(x, y)

// PicoDisplay2 is 320 by 240
#define DISPLAY_WIDTH PicoDisplay2::WIDTH
#define DISPLAY_HEIGHT PicoDisplay2::HEIGHT

uint16_t g_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];
PicoDisplay2 pico_display(g_buffer);

class MandelbrotView {
public:
    void init(int aSizeX, int aScreenSizeY, uint16_t *aBuffer, int aPalettSize, int aBlockSizeX, int aInterationLimit, int aCore);
    void init(const MandelbrotView& aView, int aBlockSizeX, int aInterationLimit, int aCore);
    void setRange(fixed_t aFxdRangeR, const complex_fixed_t& aFxdCenter);
    void setRange(const MandelbrotView& aView);
    void render(void);
    void createPalettes(int aPaletteSize);
    void nextPalette(void);
    inline void start(void) { running = true; }
    inline void stop(void) { running = false; }
    inline bool isRunning(void) const { return running; }
private:
    bool running;
    int core;
    int screenSizeX;
    int screenSizeY;
    fixed_t fxdRangeR;
    complex_fixed_t fxdCenter;
    complex_fixed_t fxdMin;
    complex_fixed_t fxdMax;
    complex_fixed_t fxdPixel;
    int iterationLimit;
    int blockSizeX;
    uint16_t *pFrameBuffer;
    static const uint8_t PALETTE_COUNT = 6;
    int paletteIndex;
    uint16_t *pPalette;
    uint16_t *pPalettes[PALETTE_COUNT];
};

void MandelbrotView::init(int aScreenSizeX, int aScreenSizeY, uint16_t *aBuffer, int aPaletteSize, int aBlockSizeX, int aInterationLimit, int aCore) {
    screenSizeX = aScreenSizeX;
    screenSizeY = aScreenSizeY;
    createPalettes(aPaletteSize);
    blockSizeX = aBlockSizeX;
    pFrameBuffer = aBuffer;
    iterationLimit = aInterationLimit;
    core = aCore;
    running = true;
}

void MandelbrotView::init(const MandelbrotView& aView, int aBlockSizeX, int aInterationLimit, int aCore) {
    *this = aView;
    for (int ii = 0; ii < PALETTE_COUNT; ++ii) {
        pPalettes[ii] = aView.pPalettes[ii];
    }
    blockSizeX = aBlockSizeX;
    iterationLimit = aInterationLimit;
    core = aCore;
    running = false;
}

void MandelbrotView::setRange(fixed_t aFxdRangeR, const complex_fixed_t& aFxdCenter) {
    fxdRangeR = aFxdRangeR;
    fxdCenter = aFxdCenter;
    fxdMin.r = aFxdCenter.r - fxdRangeR / 2;
    fxdMax.r = aFxdCenter.r + fxdRangeR / 2;
    const float screenRatio2 = static_cast<float>(screenSizeY) / static_cast<float>(screenSizeX * 2);
    const fixed_t fxdRangeI2 = float_to_fixed(fixed_to_float(fxdRangeR) * screenRatio2);
    fxdMin.i = fxdCenter.i - fxdRangeI2;
    fxdMax.i = fxdCenter.i + fxdRangeI2;
    fxdPixel.r = float_to_fixed(fixed_to_float(fxdRangeR) / screenSizeX);
    fxdPixel.i = float_to_fixed(2*fixed_to_float(fxdRangeI2) / screenSizeY);
 }

void MandelbrotView::setRange(const MandelbrotView& aView) {
    fxdRangeR = aView.fxdRangeR;
    fxdCenter= aView.fxdCenter;
    fxdMin = aView.fxdMin;
    fxdMax = aView.fxdMax;
    fxdPixel = aView.fxdPixel;
}

void MandelbrotView::render(void) {
    const fixed_t fxdMaxZ2 = FXD_FROM_INT(4);
    uint16_t *pFrame = pFrameBuffer;

    Pen pen = 0;

    complex_fixed_t fxdC = fxdMin;
    for (int screenY = 0; screenY < screenSizeY; ++screenY) {
        fxdC.r = fxdMin.r;
        for (int screenX = 0; screenX < screenSizeX; screenX += blockSizeX) {
            if (!running) {
                return;
            }
            pen = 0;
            complex_fixed_t fxdZ = fxdC;
            complex_fixed_t fxdZ0 = fxdZ;
            int period0 = 0;

            for (int ii = 0; ii < iterationLimit; ++ii) {
                const fixed_t fxdZR2 = FXD_MUL(fxdZ.r, fxdZ.r);
                const fixed_t fxdZI2 = FXD_MUL(fxdZ.i, fxdZ.i);
                if (fxdZR2 + fxdZI2 >= fxdMaxZ2) {
                    // we are outside the set so set color and break
                    pen = pPalette[ii];
                    break;
                }
                fxdZ.i = 2 * FXD_MUL(fxdZ.r, fxdZ.i) + fxdC.i;
                fxdZ.r = fxdZR2 - fxdZI2 + fxdC.r;
                if (fxdZ == fxdZ0) {
                    // we have a repeating cycle, so we are inside the set
                    break;
                }
                if (++period0 > 20) {
                    period0 = 0;
                    fxdZ0 = fxdZ;
                }
            }

            for (int ii = 0; ii < blockSizeX; ++ii) {
                *(pFrame + screenX + ii) = pen;
                fxdC.r += fxdPixel.r;
            }
        }
        fxdC.i += fxdPixel.i;
        pFrame += screenSizeX;
        if (core == 1 && (screenY & 0xF) == 0) {
            // update core1 view every 16 lines
            pico_display.update();
        }
    }
    pico_display.update();
}

// HSV Conversion expects float inputs in the range of 0.0-360.0 for the h channel and 0.0-1.0 for the s and v channels
uint16_t penFromHSV(float h, float s, float v) {
    h = fmod(h, 360.0) / 360.0;
    const float i = floor(h * 6.0f);
    const float f = h * 6.0f - i;
    v *= 255.0f;
    const uint8_t p = v * (1.0f - s);
    const uint8_t q = v * (1.0f - f * s);
    const uint8_t t = v * (1.0f - (1.0f - f) * s);

    uint8_t r = 0, g = 0, b = 0;
    switch (int(i) % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return pico_display.create_pen(r, g, b);
}

void MandelbrotView::createPalettes(int aPaletteSize) {
    paletteIndex = 2;
    for (int ii = 0; ii < PALETTE_COUNT; ++ii) {
        pPalettes[ii] = static_cast<uint16_t*>(malloc(sizeof(uint16_t) * aPaletteSize));
    }
    for (int ii = 0; ii < aPaletteSize; ++ii) {
        pPalettes[0][ii] = pico_display.create_pen(255 - 255 * ii / aPaletteSize, 255 - 255 * ii / aPaletteSize, 255 - 255 * ii / aPaletteSize);
        pPalettes[1][ii] = pico_display.create_pen(255, 255, 255);
        pPalettes[2][ii] = penFromHSV(160.0 + 360.0 * static_cast<float>(ii) / aPaletteSize, 0.9, 1.0);
        pPalettes[3][ii] = penFromHSV(60.0 + 360.0 * static_cast<float>(ii) / aPaletteSize, 0.9, 1.0);
        pPalettes[4][ii] = penFromHSV(360.0 * static_cast<float>(ii) / aPaletteSize, 0.9, 1.0);
        pPalettes[5][ii] = pico_display.create_pen(ii % 4 * 64, ii % 8 * 32, ii % 16 * 16);
    }
    pPalette = pPalettes[paletteIndex];
}

void MandelbrotView::nextPalette(void) {
    ++paletteIndex;
    if (paletteIndex >= PALETTE_COUNT) {
        paletteIndex = 0;
    }
    pPalette = pPalettes[paletteIndex];
}

MandelbrotView g_view0;

#if defined(MULTICORE)
MandelbrotView g_view1;

// Note no mutual exclusion required for start and stop
static inline void core1_start(void) {
    g_view1.start();
}

static inline void core1_stop(void) {
    g_view1.stop();
}

void core1_main(void) {
    while (true) {
        //uint32_t command = multicore_fifo_pop_blocking();
        while (!g_view1.isRunning()) {
            //tight_loop_contents();
            sleep_ms(1);
        }
        g_view1.render();
    }
}
#endif // MULTICORE

int main() {
    pico_display.init();
    pico_display.set_backlight(100);
    pico_display.set_pen(0, 0, 0);
    pico_display.clear();
    pico_display.update();

    const int iterationLimitV0 = 40;
    const int iterationLimitV1 = 128;
    g_view0.init(DISPLAY_WIDTH, DISPLAY_HEIGHT, g_buffer, iterationLimitV1, 4, iterationLimitV0, 0);

#if defined(MULTICORE)
    g_view1.init(g_view0, 1, iterationLimitV1, 1);
    // Launch core1, it won't start rendering until core1_start() is called
    multicore_launch_core1(core1_main);
#endif

    const fixed_t fxdInitialRangeR = float_to_fixed(3.2);
    const complex_fixed_t fxdInitialCenter(float_to_fixed(-0.75), float_to_fixed(0.0));
    fixed_t fxdRangeR = fxdInitialRangeR;
    complex_fixed_t fxdCenter = fxdInitialCenter;

    while (true) {
        g_view0.setRange(fxdRangeR, fxdCenter);
#if defined(MULTICORE)
        g_view1.setRange(fxdRangeR, fxdCenter);
#endif
        pico_display.set_led(64, 0, 0);

        g_view0.render();

        pico_display.set_led(0, 0, 64);
#if defined(MULTICORE)
        core1_start();
#endif

        pico_display.set_led(0, 0, 0);

        // Loop, waiting for key presses
        bool keyPressed = false;
        while (keyPressed == false) {
            sleep_ms(1);

            if (pico_display.is_pressed(pico_display.A)) {
                // Hold A to move left/right
                if (pico_display.is_pressed(pico_display.X) && fxdCenter.r > FXD_FROM_INT(-3)) {
                    fxdCenter.r -= fxdRangeR / 8;
                    keyPressed = true;
                } else if (pico_display.is_pressed(pico_display.Y) && fxdCenter.r < FXD_FROM_INT(3)) {
                    fxdCenter.r += fxdRangeR / 8;
                    keyPressed = true;
                } else if (pico_display.is_pressed(pico_display.B)) {
                    // Press A and B together to switch palette
                    g_view0.nextPalette();
#if defined(MULTICORE)
                    g_view1.nextPalette();
#endif
                    keyPressed = true;
                }
            } else if (pico_display.is_pressed(pico_display.B)) {
                // Hold B to move up/down
                if (pico_display.is_pressed(pico_display.X) && fxdCenter.i > FXD_FROM_INT(-2)) {
                    fxdCenter.i -= fxdRangeR / 8;
                    keyPressed = true;
                } else if (pico_display.is_pressed(pico_display.Y) && fxdCenter.i < FXD_FROM_INT(2)) {
                    fxdCenter.i += fxdRangeR / 8;
                    keyPressed = true;
                }
            } else {
                // Otherwise zoom in/out
                if (pico_display.is_pressed(pico_display.X)) {
                    if (pico_display.is_pressed(pico_display.Y)) {
                        // Press X and Y together to reset to initial position
                        fxdRangeR = fxdInitialRangeR;
                        fxdCenter = fxdInitialCenter;
                    } else {
                        fxdRangeR /= 4;
                        fxdRangeR *= 3;
                    }
                    keyPressed = true;
                } else if (pico_display.is_pressed(pico_display.Y) && fxdRangeR < FXD_FROM_INT(3)) {
                    fxdRangeR *= 4;
                    fxdRangeR /= 3;
                    keyPressed = true;
                }
            }
        }
#if defined(MULTICORE)
        // key pressed, so stop core1
        core1_stop();
#endif
    }
    return 0;
}
