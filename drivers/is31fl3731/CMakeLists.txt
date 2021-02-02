add_library(is31fl3731 INTERFACE)

target_sources(is31fl3731 INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/is31fl3731.cpp
)

target_include_directories(is31fl3731 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(is31fl3731 INTERFACE pico_stdlib hardware_i2c)