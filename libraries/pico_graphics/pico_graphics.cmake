if(NOT TARGET bitmap_fonts)
    include(${CMAKE_CURRENT_LIST_DIR}/../bitmap_fonts/bitmap_fonts.cmake)
endif()

if(NOT TARGET hershey_fonts)
    include(${CMAKE_CURRENT_LIST_DIR}/../hershey_fonts/hershey_fonts.cmake)
endif()

add_library(pico_graphics 
    ${CMAKE_CURRENT_LIST_DIR}/types.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pico_graphics.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pico_graphics_pen_1bit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pico_graphics_pen_1bitY.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pico_graphics_pen_3bit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pico_graphics_pen_p4.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pico_graphics_pen_p8.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pico_graphics_pen_rgb332.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pico_graphics_pen_rgb565.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pico_graphics_pen_rgb888.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pico_graphics_pen_inky7.cpp
)

target_include_directories(pico_graphics INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(pico_graphics bitmap_fonts hershey_fonts pico_stdlib)