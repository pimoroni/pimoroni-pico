# cmake file for Pimoroni Badger 2040W
set(MICROPY_BOARD PICO_W)

set(MICROPY_PY_LWIP ON)
set(MICROPY_PY_NETWORK_CYW43 ON)

# Board specific version of the frozen manifest
set(MICROPY_FROZEN_MANIFEST ${CMAKE_SOURCE_DIR}/boards/PIMORONI_BADGER2040W/manifest.py)