
add_library(keycoder2040 INTERFACE)

target_sources(keycoder2040 INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/keycoder2040.cpp
)

target_include_directories(keycoder2040 INTERFACE ${CMAKE_CURRENT_LIST_DIR})


pico_generate_pio_header(keycoder2040 ${CMAKE_CURRENT_LIST_DIR}/keycoder.pio)

# Pull in pico libraries that we need
target_link_libraries(keycoder2040 INTERFACE 
                        pico_stdlib 
                        hardware_i2c
                        hardware_pio
                        pimoroni_i2c
                        is31fl3731
                        )

