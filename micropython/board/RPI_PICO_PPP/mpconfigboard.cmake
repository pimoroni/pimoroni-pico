# cmake file for Raspberry Pi Pico
set(PICO_BOARD "pico")

set(MICROPY_PY_LWIP ON)

# Board specific version of the frozen manifest
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)

set(MICROPY_C_HEAP_SIZE 4096)

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 1048576)  # 1024 * 1024
endif()
