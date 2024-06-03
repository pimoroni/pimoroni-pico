// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Raspberry Pi Pico"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (1408 * 1024)

// Enable networking.
#define MICROPY_PY_NETWORK                      1
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "Pico"

#define MICROPY_PY_NETWORK_PPP_LWIP             1

#define MICROPY_HW_NIC_PPP                      { MP_ROM_QSTR(MP_QSTR_PINT), MP_ROM_PTR(&mp_network_ppp_lwip_type) },

#define MICROPY_BOARD_NETWORK_INTERFACES \
    MICROPY_HW_NIC_PPP

#define MICROPY_PY_SOCKET_EXTENDED_STATE        1