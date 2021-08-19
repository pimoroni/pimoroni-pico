set(OUTPUT_NAME plasma2040_rotary)
add_executable(${OUTPUT_NAME} plasma2040_rotary.cpp)

target_link_libraries(${OUTPUT_NAME}
        pico_stdlib
        plasma2040
        breakout_encoder
        pimoroni_i2c
        rgbled
        button
        )

pico_add_extra_outputs(${OUTPUT_NAME})
