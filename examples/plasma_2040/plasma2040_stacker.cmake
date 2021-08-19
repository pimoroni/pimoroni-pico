add_executable(plasma2040_stacker plasma2040_stacker.cpp)

target_link_libraries(plasma2040_stacker
        pico_stdlib
        plasma2040
        rgbled
        button
        )

pico_add_extra_outputs(plasma2040_stacker)
