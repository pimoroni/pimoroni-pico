# cmake file for Raspberry Pi Pico W
set(MICROPY_BOARD PICO_W)

set(MICROPY_PY_LWIP ON)
set(MICROPY_PY_NETWORK_CYW43 ON)

# Board specific version of the frozen manifest
set(MICROPY_FROZEN_MANIFEST ${CMAKE_SOURCE_DIR}/boards/PICO_W_INKY/manifest.py)
