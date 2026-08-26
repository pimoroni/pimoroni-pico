add_library(bitmap_fonts
    ${CMAKE_CURRENT_LIST_DIR}/bitmap_fonts.cpp
    ${CMAKE_CURRENT_LIST_DIR}/font6_data.cpp
    ${CMAKE_CURRENT_LIST_DIR}/font8_data.cpp
    ${CMAKE_CURRENT_LIST_DIR}/font14_outline_data.cpp
)

target_include_directories(bitmap_fonts INTERFACE ${CMAKE_CURRENT_LIST_DIR})

