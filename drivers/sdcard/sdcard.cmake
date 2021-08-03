if (NOT TARGET sdcard)
    add_library(sdcard INTERFACE)

    pico_generate_pio_header(sdcard ${CMAKE_CURRENT_LIST_DIR}/spi.pio)

    target_sources(sdcard INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/sdcard.c
            ${CMAKE_CURRENT_LIST_DIR}/pio_spi.c
    )

    target_link_libraries(sdcard INTERFACE fatfs pico_stdlib hardware_clocks hardware_spi hardware_pio)
    target_include_directories(sdcard INTERFACE ${CMAKE_CURRENT_LIST_DIR})
endif()
