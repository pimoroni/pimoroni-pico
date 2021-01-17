add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/fox.o
  COMMAND arm-none-eabi-ld -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/fox.o ${CMAKE_CURRENT_SOURCE_DIR}/fox.tga
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/fox.tga
)

add_executable(
  explorer
  demo.cpp
  fox.o
)

# Pull in pico libraries that we need
target_link_libraries(explorer pico_stdlib pico_explorer msa301)

# create map/bin/hex file etc.
pico_add_extra_outputs(explorer)
