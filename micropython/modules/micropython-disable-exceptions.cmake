# Do not include stack unwinding & exception handling for C++ user modules
target_compile_definitions(usermod INTERFACE PICO_CXX_ENABLE_EXCEPTIONS=0)
target_compile_options(usermod INTERFACE $<$<COMPILE_LANGUAGE:CXX>:
    -fno-exceptions
    -fno-unwind-tables
    -fno-rtti
    -fno-use-cxa-atexit
    -Wl,--no-ld-generated-unwind-info
>)
