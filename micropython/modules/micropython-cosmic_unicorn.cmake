include_directories(${CMAKE_CURRENT_LIST_DIR}/../../)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Disable hershey fonts to claw back some binary size
# PicoVector's Alright Fonts replace these
target_compile_definitions(usermod INTERFACE
HERSHEY_FONTS=0
)

# Essential
include(pimoroni_i2c/micropython)
include(pimoroni_bus/micropython)

# Pico Graphics Essential
include(bitmap_fonts/micropython)
include(picographics/micropython)

# Pico Graphics Extra
include(pngdec/micropython)
include(jpegdec/micropython)
include(qrcode/micropython/micropython)

# Sensors & Breakouts
include(micropython-common-breakouts)
include(pcf85063a/micropython)

# Utility
include(adcfft/micropython)

# LEDs & Matrices
include(cosmic_unicorn/micropython)

# ULAB
include(micropython-common-ulab)
enable_ulab()

include(modules_py/modules_py)

# C++ Magic Memory
include(cppmem/micropython)

# Disable build-busting C++ exceptions
include(micropython-disable-exceptions)
