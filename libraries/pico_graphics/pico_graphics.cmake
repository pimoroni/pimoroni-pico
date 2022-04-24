include(${CMAKE_CURRENT_LIST_DIR}/../bitmap_fonts/bitmap_fonts.cmake)

add_library(pico_graphics 
    ${CMAKE_CURRENT_LIST_DIR}/types.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pico_graphics.cpp
)

target_include_directories(pico_graphics INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(pico_graphics bitmap_fonts)
