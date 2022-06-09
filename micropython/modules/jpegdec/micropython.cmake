add_library(usermod_jpegdec INTERFACE)

target_sources(usermod_jpegdec INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/jpegdec.c
    ${CMAKE_CURRENT_LIST_DIR}/jpegdec.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/jpegdec/JPEGDEC.cpp
)

target_include_directories(usermod_jpegdec INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(usermod_jpegdec INTERFACE
    MODULE_JPEGDEC_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_jpegdec)

set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/../../../libraries/jpegdec/JPEGDEC.cpp PROPERTIES COMPILE_FLAGS "-Wno-error=unused-function")
