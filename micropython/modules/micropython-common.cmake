include(pimoroni_i2c/micropython)

include(breakout_dotmatrix/micropython)
include(breakout_encoder/micropython)
include(breakout_ioexpander/micropython)
include(breakout_ltr559/micropython)
include(breakout_colourlcd160x80/micropython)
include(breakout_as7262/micropython)
include(breakout_rgbmatrix5x5/micropython)
include(breakout_matrix11x7/micropython)
include(breakout_msa301/micropython)
include(breakout_pmw3901/micropython)
include(breakout_mics6814/micropython)
include(breakout_potentiometer/micropython)
include(breakout_rtc/micropython)
include(breakout_trackball/micropython)
include(breakout_sgp30/micropython)
include(breakout_bh1745/micropython)
include(breakout_bme68x/micropython)
include(breakout_bme280/micropython)
include(breakout_bmp280/micropython)
include(breakout_icp10125/micropython)
include(breakout_scd41/micropython)
include(breakout_vl53l5cx/micropython)

include(pico_scroll/micropython)
include(pico_rgb_keypad/micropython)
include(pico_unicorn/micropython)
include(pico_explorer/micropython)
include(pico_wireless/micropython)

include(bitmap_fonts/micropython)

include(plasma/micropython)
include(hub75/micropython)
include(pwm/micropython)
include(servo/micropython)
include(encoder/micropython)
include(motor/micropython)
include(qrcode/micropython/micropython)
include(adcfft/micropython)

include(st7789/micropython)
include(pcf85063a/micropython)

include(modules_py/modules_py)

function(enable_ulab)
    include(ulab/code/micropython)

    target_compile_definitions(usermod_ulab INTERFACE
        # Support for complex ndarrays
        ULAB_SUPPORTS_COMPLEX=0

        # Determines, whether scipy is defined in ulab. The sub-modules and functions
        # of scipy have to be defined separately
        ULAB_HAS_SCIPY=0

        # The maximum number of dimensions the firmware should be able to support
        # Possible values lie between 1, and 4, inclusive
        ULAB_MAX_DIMS=2

        # By setting this constant to 1, iteration over array dimensions will be implemented
        # as a function (ndarray_rewind_array), instead of writing out the loops in macros
        # This reduces firmware size at the expense of speed
        ULAB_HAS_FUNCTION_ITERATOR=1

        # If NDARRAY_IS_ITERABLE is 1, the ndarray object defines its own iterator function
        # This option saves approx. 250 bytes of flash space
        NDARRAY_IS_ITERABLE=1

        # Slicing can be switched off by setting this variable to 0
        NDARRAY_IS_SLICEABLE=1

        # The default threshold for pretty printing. These variables can be overwritten
        # at run-time via the set_printoptions() function
        ULAB_HAS_PRINTOPTIONS=1
        NDARRAY_PRINT_THRESHOLD=10
        NDARRAY_PRINT_EDGEITEMS=3

        # determines, whether the dtype is an object, or simply a character
        # the object implementation is numpythonic, but requires more space
        ULAB_HAS_DTYPE_OBJECT=0

        # the ndarray binary operators
        NDARRAY_HAS_BINARY_OPS=0

        # Firmware size can be reduced at the expense of speed by using function
        # pointers in iterations. For each operator, the function pointer saves around
        # 2 kB in the two-dimensional case, and around 4 kB in the four-dimensional case.

        NDARRAY_BINARY_USES_FUN_POINTER=1

        NDARRAY_HAS_BINARY_OP_ADD=1
        NDARRAY_HAS_BINARY_OP_EQUAL=1
        NDARRAY_HAS_BINARY_OP_LESS=1
        NDARRAY_HAS_BINARY_OP_LESS_EQUAL=1
        NDARRAY_HAS_BINARY_OP_MORE=1
        NDARRAY_HAS_BINARY_OP_MORE_EQUAL=1
        NDARRAY_HAS_BINARY_OP_MULTIPLY=1
        NDARRAY_HAS_BINARY_OP_NOT_EQUAL=1
        NDARRAY_HAS_BINARY_OP_POWER=1
        NDARRAY_HAS_BINARY_OP_SUBTRACT=1
        NDARRAY_HAS_BINARY_OP_TRUE_DIVIDE=1
    )
endfunction()
