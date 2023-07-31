cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

# Initialize the SDK
pico_sdk_init()

pico_find_compiler(PICO_COMPILER_LD ${PICO_GCC_TRIPLE}-ld)

function(add_resource target file)
  get_filename_component(NAME ${ARGV1} NAME_WE)
  set(FILENAME ${ARGV1})

  add_custom_command(
    OUTPUT ${NAME}.o

    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}
            ${CMAKE_CURRENT_BINARY_DIR}

    COMMAND ${PICO_COMPILER_LD} -r -b binary -o ${NAME}.o ${FILENAME}
    DEPENDS ${FILENAME}
  )

  target_sources(${ARGV0} PRIVATE ${NAME}.o)
endfunction(add_resource)

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

add_subdirectory(common)
add_subdirectory(drivers)
add_subdirectory(libraries)

# Example Pico Pack projects
add_subdirectory(examples)
