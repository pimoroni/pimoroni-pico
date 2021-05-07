if (NOT TARGET sdcard)
    add_library(sdcard INTERFACE)

    target_sources(sdcard INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/sdcard.c
    )

    target_link_libraries(sdcard INTERFACE fatfs pico_stdlib hardware_clocks hardware_spi)
    target_include_directories(sdcard INTERFACE ${CMAKE_CURRENT_LIST_DIR})
endif()
