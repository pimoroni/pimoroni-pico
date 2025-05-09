add_library(usermod_picovector INTERFACE)

target_sources(usermod_picovector INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_vector/pico_vector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/picovector.c
    ${CMAKE_CURRENT_LIST_DIR}/picovector.cpp
)

target_include_directories(usermod_picovector INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_vector/
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_graphics/
)

target_compile_definitions(usermod_picovector INTERFACE
    MODULE_PICOVECTOR_ENABLED=1
)

target_link_libraries(usermod_picovector INTERFACE hardware_interp)

target_link_libraries(usermod INTERFACE usermod_picovector)

set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pico_vector/pico_vector.cpp
    PROPERTIES COMPILE_FLAGS
    "-Wno-narrowing -O2 -fgcse-after-reload -floop-interchange -fpeel-loops -fpredictive-commoning -fsplit-paths -ftree-loop-distribute-patterns -ftree-loop-distribution -ftree-vectorize -ftree-partial-pre -funswitch-loops"
)
