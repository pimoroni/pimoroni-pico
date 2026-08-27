// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Pimoroni Tiny 2350 (PPP)"

// Set up networking.
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "PPP2"

// Enable Networking & PPP
#define MICROPY_PY_NETWORK                      (1)
#define MICROPY_PY_NETWORK_PPP_LWIP             (1)

// I2C0 (non-default)
#define MICROPY_HW_I2C0_SCL  (4)
#define MICROPY_HW_I2C0_SDA  (5)