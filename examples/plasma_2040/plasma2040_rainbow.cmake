add_executable(plasma2040_rainbow plasma2040_rainbow.cpp)

target_link_libraries(plasma2040_rainbow
        pico_stdlib
        plasma2040
        rgbled
        button
        )

pico_add_extra_outputs(plasma2040_rainbow)
