set(MOD_NAME badger2040)
string(TOUPPER ${MOD_NAME} MOD_NAME_UPPER)
add_library(usermod_${MOD_NAME} INTERFACE)

target_sources(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.c
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.cpp
    ${CMAKE_CURRENT_LIST_DIR}/badgerinit.S
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/badger2040/badger2040.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/uc8151/uc8151.cpp
)

target_include_directories(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/badger2040/
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/uc8151/
)

target_compile_definitions(usermod_${MOD_NAME} INTERFACE
    MODULE_BADGER2040_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})
