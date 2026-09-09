include_directories(${CMAKE_CURRENT_LIST_DIR}/../../)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# RGBA4444 canvases, half the bytes of RGBA8888, so a full 2.8" canvas fits beside the
# heap on a board with SRAM alone. picovector and the screen driver both read this, and
# it has to be set before either is found.
set(PV_PIXEL_FORMAT 2)

include(micropython-common)
enable_ulab()

# C++ Magic Memory
include(cppmem/micropython)

# Drop the C++ demangler, which nothing can reach
include(cxx_terminate/micropython)

# SP/CE screens, on the Pico Display Pack 2.8" and what chains from its output. The GC
# heap owns this board's SRAM, so the displays' region is a block of it sized for two.
set(SPIDISPLAY_HEAP_RESERVE_BYTES 16384)
find_package(SPIDISPLAY CONFIG REQUIRED)
