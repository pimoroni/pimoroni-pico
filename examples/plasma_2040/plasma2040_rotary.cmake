add_executable(plasma2040_rotary plasma2040_rotary.cpp)

target_link_libraries(plasma2040_rotary
        pico_stdlib
        plasma2040
        breakout_encoder
        pimoroni_i2c
        rgbled
        button
        )

pico_add_extra_outputs(plasma2040_rotary)
