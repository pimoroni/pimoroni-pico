# Do not include stack unwinding & exception handling for C++ user modules
target_compile_definitions(usermod INTERFACE PICO_CXX_ENABLE_EXCEPTIONS=0)
target_compile_options(usermod INTERFACE $<$<COMPILE_LANGUAGE:CXX>:
    -fno-exceptions
    -fno-unwind-tables
    -fno-rtti
    -fno-use-cxa-atexit
    -ffunction-sections
    -fdata-sections
    -Wl,--gc-sections
    -Wl,--no-ld-generated-unwind-info
>)
#target_link_options(usermod INTERFACE -specs=nano.specs)
