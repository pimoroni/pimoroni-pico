add_executable(
  explorer
  demo.cpp
)

add_resource(explorer fox.tga)

# Pull in pico libraries that we need
target_link_libraries(explorer pico_stdlib pico_explorer msa301)

# create map/bin/hex file etc.
pico_add_extra_outputs(explorer)

add_executable(
  text_demo
  text_demo.cpp
)

target_link_libraries(text_demo pico_stdlib pico_explorer msa301)

pico_add_extra_outputs(text_demo)