set(LIB_NAME hershey_fonts)
add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/${LIB_NAME}.cpp
  ${CMAKE_CURRENT_LIST_DIR}/${LIB_NAME}_data.cpp
)

if(NOT HERSHEY_FONTS)
    # TODO: Swap this to disabled by default when we're ready to deprecate
    # Hershey has all but been replaced by PicoVector's "alright fonts."
    set(HERSHEY_FONTS 1)
endif()

target_compile_definitions(${LIB_NAME} INTERFACE
    HERSHEY_FONTS=${HERSHEY_FONTS}
)

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})
