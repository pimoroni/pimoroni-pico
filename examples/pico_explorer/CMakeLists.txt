add_executable(
  pico_explorer_demo
  pico_explorer_demo.cpp
)

add_resource(pico_explorer_demo fox.tga)

# Pull in pico libraries that we need
target_link_libraries(pico_explorer_demo pico_stdlib pico_explorer msa301 picographics_st7789 button motor analog)

# create map/bin/hex file etc.
pico_add_extra_outputs(pico_explorer_demo)

add_executable(
  text_demo
  text_demo.cpp
)

target_link_libraries(text_demo pico_stdlib pico_explorer msa301 picographics_st7789)

pico_add_extra_outputs(text_demo)