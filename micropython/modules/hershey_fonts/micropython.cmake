set(MOD_NAME hershey_fonts)
string(TOUPPER ${MOD_NAME} MOD_NAME_UPPER)
add_library(usermod_${MOD_NAME} INTERFACE)

target_sources(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/hershey_fonts/hershey_fonts.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/hershey_fonts/hershey_fonts_data.cpp
)

target_include_directories(usermod_${MOD_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../../libraries/hershey_fonts
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})