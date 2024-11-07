# cmake file for Raspberry Pi Pico W
set(PICO_BOARD "pico_w")

# Allow Pico SDK to locate "pico_w.h" in this directory.
list(APPEND PICO_BOARD_HEADER_DIRS "${CMAKE_CURRENT_LIST_DIR}")

# The C malloc is needed by cyw43-driver Bluetooth and Pimoroni Pico modules
set(MICROPY_C_HEAP_SIZE 4096)

set(MICROPY_PY_LWIP ON)
set(MICROPY_PY_NETWORK_CYW43 ON)

# Bluetooth
set(MICROPY_PY_BLUETOOTH ON)
set(MICROPY_BLUETOOTH_BTSTACK ON)
set(MICROPY_PY_BLUETOOTH_CYW43 ON)

# Board specific version of the frozen manifest
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)