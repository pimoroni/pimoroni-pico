# cmake file for Raspberry Pi Pico2
set(PICO_BOARD "pico2")
set(PICO_PLATFORM "rp2350")

# To change the gpio count for QFN-80
# set(PICO_NUM_GPIOS 48)

# Links micropy_lib_lwip and sets MICROPY_PY_LWIP = 1
# Picked up and expanded upon in mpconfigboard.h
set(MICROPY_PY_LWIP ON)

if (PICO_CYW43_SUPPORTED)
    include(enable_cyw43.cmake)
    set(PICO_PINS_CSV_NAME pins_cyw43.csv)
endif()

set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)

# The C malloc is needed by cyw43-driver Bluetooth and Pimoroni Pico modules
set(MICROPY_C_HEAP_SIZE 4096)

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 2097152)  # 4MB - 2MB
endif()
