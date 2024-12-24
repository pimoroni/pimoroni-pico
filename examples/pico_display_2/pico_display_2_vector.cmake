function(static_asset NAME PATH)
  get_filename_component(PATH ${PATH} ABSOLUTE)
  get_filename_component(ASSET ${PATH} NAME)
  get_filename_component(PATH ${PATH} DIRECTORY)
  set(OBJNAME ${ASSET}.o)
  add_custom_command(OUTPUT ${OBJNAME}
    DEPENDS ${PATH}/${ASSET}
    COMMENT "Building ${OBJNAME}"
    WORKING_DIRECTORY "${PATH}"
    COMMAND ${CMAKE_LINKER} -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/${OBJNAME} ${ASSET}
    COMMAND ${CMAKE_OBJDUMP} -t ${CMAKE_CURRENT_BINARY_DIR}/${OBJNAME}
  )
  # TODO figure out how to make static resources work
  ## COMMAND ${CMAKE_OBJCOPY} --rename-section .data=.rodata,alloc,load,readonly,data,contents ${CMAKE_CURRENT_BINARY_DIR}/${OBJNAME} ${CMAKE_CURRENT_BINARY_DIR}/${OBJNAME})
  target_sources(${NAME} PRIVATE ${OBJNAME})
endfunction()

add_executable(
    pico_display_2_vector
    pico_display_2_vector.cpp
)

# Pull in pico libraries that we need
target_link_libraries(pico_display_2_vector 
    pico_stdlib
    hardware_spi
    hardware_pwm
    hardware_dma
    pico_display_2
    st7789
    pico_graphics
    pico_vector
    )

static_asset(pico_display_2_vector ${CMAKE_CURRENT_LIST_DIR}/vector/DynaPuff-Medium.af)

pico_enable_stdio_usb(pico_display_2_vector 0)
pico_enable_stdio_uart(pico_display_2_vector 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(pico_display_2_vector)