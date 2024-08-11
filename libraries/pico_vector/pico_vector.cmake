add_library(pico_vector 
    ${CMAKE_CURRENT_LIST_DIR}/pico_vector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pretty_poly.cpp
    ${CMAKE_CURRENT_LIST_DIR}/alright_fonts.cpp
)

target_include_directories(pico_vector INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(pico_vector pico_stdlib hardware_interp)