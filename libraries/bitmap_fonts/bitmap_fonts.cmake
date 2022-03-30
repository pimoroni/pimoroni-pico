add_library(bitmap_fonts 
    ${CMAKE_CURRENT_LIST_DIR}/bitmap_fonts.cpp
)

target_include_directories(bitmap_fonts INTERFACE ${CMAKE_CURRENT_LIST_DIR})

