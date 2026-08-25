# cmake file for Pimoroni Pico LiPo 4MB

# Board specific version of the frozen manifest
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)

set(MICROPY_C_HEAP_SIZE 4096)

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 3145728)  # 3 * 1024 * 1024
endif()
