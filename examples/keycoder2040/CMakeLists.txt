set(NAME keycoder_test)

# Add your source files
add_executable(${NAME}
    keycoder_demo.cpp 
)


target_include_directories(${NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        )


# Don't forget to link the libraries you need!
target_link_libraries(${NAME}
    pimoroni_i2c
    pico_stdlib              # for core functionality
    button
    keycoder2040
    )

# enable usb output
pico_enable_stdio_usb(${NAME} 1)


# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})


