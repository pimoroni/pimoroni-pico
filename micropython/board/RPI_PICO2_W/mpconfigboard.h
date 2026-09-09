// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Raspberry Pi Pico 2 W"

#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "Pico2W"

// Enable PPP
#define MICROPY_PY_NETWORK_PPP_LWIP             (1)

#include "enable_cyw43.h"

// core1 is picovector's worker, which the spidisplay module's frame conversion shares.
// With threads on, every soft reset resets core1 under a worker that believes it is
// still running, and the next job waits for it forever.
#define MICROPY_PY_THREAD                       (0)