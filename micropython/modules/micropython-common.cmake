# The C heap ends at __StackLimit, which is not page aligned, while newlib's malloc grows
# its arena in page-aligned steps. With an unlucky __bss_end__ the first C++ static
# allocations overshoot the limit, are refused whole, and panic before main runs, so
# whether a build boots depends on its RAM layout. Clamping sbrk to the limit instead of
# refusing removes that dependence.
add_compile_definitions(PICO_USE_OPTIMISTIC_SBRK=1)

# Essential
include(pimoroni_i2c/micropython)
include(pimoroni_bus/micropython)

# Pico Graphics Essential
include(bitmap_fonts/micropython)
include(picographics/micropython)

# Pico Graphics Extra
include(pngdec/micropython)
include(jpegdec/micropython)
include(picovector/micropython)
include(qrcode/micropython/micropython)

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