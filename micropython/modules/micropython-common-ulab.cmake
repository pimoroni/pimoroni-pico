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
        NDARRAY_HAS_BINARY_OPS=1

        # Firmware size can be reduced at the expense of speed by using function
        # pointers in iterations. For each operator, the function pointer saves around
        # 2 kB in the two-dimensional case, and around 4 kB in the four-dimensional case.

        NDARRAY_BINARY_USES_FUN_POINTER=0

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