if(NOT TARGET pico_graphics)
    include(${CMAKE_CURRENT_LIST_DIR}/../pico_graphics/pico_graphics.cmake)
endif()

add_library(pico_vector 
    ${CMAKE_CURRENT_LIST_DIR}/pico_vector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/af-file-io.c
    ${CMAKE_CURRENT_LIST_DIR}/af-memory.c
)

target_include_directories(pico_vector INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(pico_vector pico_graphics pico_stdlib hardware_interp)

set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/pico_vector.cpp
    PROPERTIES COMPILE_FLAGS
    "-Wno-narrowing"
)