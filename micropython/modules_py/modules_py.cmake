function (copy_module TARGET SRC DST)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/../modules/${DST}.py

        COMMAND
            cp ${SRC} ${CMAKE_CURRENT_BINARY_DIR}/../modules/${DST}.py

        DEPENDS ${src}
    )

    target_sources(${TARGET} INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/../modules/${DST}.py)
endfunction()

function (genversion TARGET DST)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/../modules/${DST}.py

        COMMAND
            bash ${CMAKE_CURRENT_LIST_DIR}/genversion.sh ${CMAKE_CURRENT_BINARY_DIR}/../modules/${DST}.py

        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/genversion.sh
    )

    target_sources(${TARGET} INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/../modules/${DST}.py)
endfunction()

# Create a dummy usermod to hang our .py copies from
add_library(usermod_modules_py INTERFACE)
target_link_libraries(usermod INTERFACE usermod_modules_py)

genversion(usermod_modules_py version)

# .py files to copy from modules_py to ports/rp2/modules
#copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/picosystem.py picosystem)
copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/pimoroni.py pimoroni)
copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/gfx_pack.py gfx_pack)
copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/interstate75.py interstate75)
if(PICO_BOARD STREQUAL "pico_w")
    copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/automation.py automation)
    copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/inventor.py inventor)
    copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/inky_frame.py inky_frame)
endif()