add_library(pngdec
    ${CMAKE_CURRENT_LIST_DIR}/PNGdec.cpp
    ${CMAKE_CURRENT_LIST_DIR}/adler32.c
    ${CMAKE_CURRENT_LIST_DIR}/crc32.c
    ${CMAKE_CURRENT_LIST_DIR}/infback.c
    ${CMAKE_CURRENT_LIST_DIR}/inffast.c
    ${CMAKE_CURRENT_LIST_DIR}/inflate.c
    ${CMAKE_CURRENT_LIST_DIR}/inftrees.c
    ${CMAKE_CURRENT_LIST_DIR}/zutil.c
)

set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/PNGDEC.cpp PROPERTIES COMPILE_FLAGS "-Wno-error=unused-function")

target_include_directories(pngdec INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(pngdec pico_stdlib)