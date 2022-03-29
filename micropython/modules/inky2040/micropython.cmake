set(MOD_NAME inky2040)
string(TOUPPER ${MOD_NAME} MOD_NAME_UPPER)
add_library(usermod_${MOD_NAME} INTERFACE)

target_sources(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.c
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/inky2040/inky2040.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/uc8159/uc8159.cpp
)

target_include_directories(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/inky2040/
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/uc8159/
)

target_compile_definitions(usermod_${MOD_NAME} INTERFACE
    MODULE_INKY2040_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})