# cmake file for Pimoroni Tufty 2040
set(PICO_BOARD "pimoroni_tufty2040")

# Allow Pico SDK to locate "pimoroni_tufty2040.h" in this directory.
list(APPEND PICO_BOARD_HEADER_DIRS "${CMAKE_CURRENT_LIST_DIR}")

# Board specific version of the frozen manifest
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)

set(MICROPY_C_HEAP_SIZE 4096)