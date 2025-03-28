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

# Hershey has all but been replaced by PicoVector's "alright fonts."
# This definition ensures that PicoGraphics compiles in Hershey support.
target_compile_definitions(usermod_${MOD_NAME} INTERFACE
HERSHEY_FONTS=1
)

target_link_libraries(usermod INTERFACE usermod_${MOD_NAME})