add_library(usermod_pngdec INTERFACE)

set(PNGDEC_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/pngdec)

target_sources(usermod_pngdec INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/pngdec.c
    ${CMAKE_CURRENT_LIST_DIR}/pngdec.cpp

    ${PNGDEC_DIR}/PNGdec.cpp
    ${PNGDEC_DIR}/adler32.c
    ${PNGDEC_DIR}/crc32.c
    ${PNGDEC_DIR}/infback.c
    ${PNGDEC_DIR}/inffast.c
    ${PNGDEC_DIR}/inflate.c
    ${PNGDEC_DIR}/inftrees.c
    ${PNGDEC_DIR}/zutil.c
)

target_include_directories(usermod_pngdec INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(usermod_pngdec INTERFACE
    MODULE_PNGDEC_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_pngdec)