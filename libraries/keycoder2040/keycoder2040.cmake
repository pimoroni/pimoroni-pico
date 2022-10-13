add_library(keycoder2040 INTERFACE)

target_include_directories(keycoder2040 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(keycoder2040 INTERFACE pico_stdlib hardware_i2c encoder)

