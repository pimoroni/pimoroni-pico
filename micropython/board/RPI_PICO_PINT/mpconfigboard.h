// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Raspberry Pi Pico"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (1408 * 1024)

// Enable networking.
#define MICROPY_PY_NETWORK                      1
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "Pico"

extern const struct _mp_obj_type_t mod_network_nic_type_pint;
#define MICROPY_HW_NIC_PINT                      { MP_ROM_QSTR(MP_QSTR_PINT), MP_ROM_PTR(&mod_network_nic_type_pint) },

#define MICROPY_BOARD_NETWORK_INTERFACES \
    MICROPY_HW_NIC_PINT

#define MICROPY_PY_SOCKET_EXTENDED_STATE        1