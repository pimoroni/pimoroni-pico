set(MOD_NAME breakout_bme68x)
string(TOUPPER ${MOD_NAME} MOD_NAME_UPPER)
add_library(usermod_${MOD_NAME} INTERFACE)

target_sources(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.c
    ${CMAKE_CURRENT_LIST_DIR}/${MOD_NAME}.cpp
)

include(drivers/bme68x/bme68x)
target_link_libraries(usermod_${MOD_NAME} INTERFACE bme68x)

target_include_directories(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(usermod_${MOD_NAME} INTERFACE
    MODULE_${MOD_NAME_UPPER}_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})