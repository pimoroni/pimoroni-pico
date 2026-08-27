# Deprecated, retained for out-of-tree boards. Use cxx_terminate/micropython.
#
# The four C++ flags below are already pico-sdk defaults: pico_cxx_options sets
# all of them unless PICO_CXX_ENABLE_EXCEPTIONS/RTTI/CXA_ATEXIT are set, and
# pico_runtime links it, so they were never doing anything here.
#
# -specs=nano.specs was doing the work, but by swapping newlib and libstdc++ for
# the nano variants wholesale. Most of the saving is libstdc++_nano's terse
# std::terminate, which cxx_terminate now gets directly. The rest is newlib-nano,
# which is a size/behaviour decision unrelated to exceptions - among other things
# it drops float support from printf unless something links -u _printf_float.
#
# Both are kept here so out-of-tree boards including this do not change size or
# behaviour. In-tree boards use cxx_terminate/micropython instead.
message(WARNING
    "micropython-disable-exceptions is deprecated. "
    "Use include(cxx_terminate/micropython) instead, and add -specs=nano.specs "
    "yourself if you want newlib-nano.")

include(cxx_terminate/micropython)

target_compile_definitions(usermod INTERFACE PICO_CXX_ENABLE_EXCEPTIONS=0)
target_compile_options(usermod INTERFACE $<$<COMPILE_LANGUAGE:CXX>:
    -fno-exceptions
    -fno-unwind-tables
    -fno-rtti
    -fno-use-cxa-atexit
>)
target_link_options(usermod INTERFACE -specs=nano.specs)
