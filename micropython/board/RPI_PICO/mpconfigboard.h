// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Raspberry Pi Pico"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (1408 * 1024)

#define MICROPY_HW_USB_MSC (1)
#define MICROPY_HW_USB_VID (0x1FFB)
#define MICROPY_HW_USB_PID (0x2043)
#define MICROPY_HW_USB_DESC_STR_MAX (40)
#define MICROPY_HW_USB_MANUFACTURER_STRING "Pimoroni"
#define MICROPY_HW_USB_PRODUCT_FS_STRING MICROPY_HW_BOARD_NAME " MicroPython"