// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Raspberry Pi Pico 2 W"

#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "Pico2W"

// Enable PPP
#define MICROPY_PY_NETWORK_PPP_LWIP             (1)

#include "enable_cyw43.h"