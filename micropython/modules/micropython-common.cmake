# Essential
include(pimoroni_i2c/micropython)
include(pimoroni_bus/micropython)

# Pico Graphics Essential
include(bitmap_fonts/micropython)
include(picographics/micropython)

# Pico Graphics Extra
include(pngdec/micropython)
include(jpegdec/micropython)

# RP2350 boards take PicoVector v3 from picovector-micropython, rasterising on core1,
# and their QR codes with it, since it carries its own qrcodegen. RP2040 boards keep
# the in-tree v2 and qrcode, since v3 assumes the RP2350's FIFO interrupt and hardware
# float. This MicroPython has no no-scan allocator, so the plain one stands in.
if(PICO_RP2350)
    set(PV_DUAL_CORE ON)
    find_package(PICOVECTOR_MICROPYTHON CONFIG REQUIRED)
    target_compile_definitions(usermod_picovector INTERFACE m_malloc_no_scan=m_malloc)
else()
    include(picovector/micropython)
    include(qrcode/micropython/micropython)
endif()

# Sensors & Breakouts
include(micropython-common-breakouts)

# Packs & Bases
include(pico_unicorn/micropython)
include(pico_scroll/micropython)
include(pico_rgb_keypad/micropython)
include(pico_explorer/micropython)

# LEDs & Matrices
include(plasma/micropython)
include(hub75/micropython)

# Servos & Motors
include(pwm/micropython)
include(servo/micropython)
include(encoder/micropython)
include(motor/micropython)

# Utility
include(adcfft/micropython)

# RTC (Badger 2040W, Enviro)
if(PICO_BOARD STREQUAL "pico_w")
    include(pcf85063a/micropython)
endif()

include(modules_py/modules_py)

# Most board specific ports wont need all of these
# copy_module(gfx_pack.py)
# copy_module(interstate75.py)
# if(PICO_BOARD STREQUAL "pico_w")
#     copy_module(automation.py)
#     copy_module(inventor.py)
# endif()

# Must call `enable_ulab()` to enable
include(micropython-common-ulab)