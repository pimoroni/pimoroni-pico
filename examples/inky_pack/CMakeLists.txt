include(inky_pack_drawing.cmake)
include(inky_pack_fonts.cmake)
include(inky_pack_image.cmake)
include(inky_pack_demo.cmake)
include(inky_pack_clock.cmake)

if (PICO_CYW43_SUPPORTED) # set by BOARD=pico-w
    if (NOT TARGET pico_cyw43_arch)
        message("Inky Pack: Skipping Pico W examples (Unsupported)")
    else()
        add_subdirectory(picow_examples)
    endif()
endif()