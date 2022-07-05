function (copy_module TARGET SRC DST)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/../modules/${DST}.py

        COMMAND
            cp ${SRC} ${CMAKE_CURRENT_BINARY_DIR}/../modules/${DST}.py

        DEPENDS ${src}
    )

    target_sources(${TARGET} INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/../modules/${DST}.py)
endfunction()

# Create a dummy usermod to hang our .py copies from
add_library(usermod_modules_py INTERFACE)
target_link_libraries(usermod INTERFACE usermod_modules_py)

# .py files to copy from modules_py to ports/rp2/modules
#copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/picosystem.py picosystem)
copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/pimoroni.py pimoroni)

if(PICO_BOARD STREQUAL "pico_w")
    copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/automation.py automation)
    copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/inventor.py inventor)
    copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/enviro/__init__.py enviro/__init__)
    copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/enviro/base.py enviro/base)
    copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/enviro/camera.py enviro/camera)
    copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/enviro/grow.py enviro/grow)
    copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/enviro/indoor.py enviro/indoor)
    copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/enviro/urban.py enviro/urban)
    copy_module(usermod_modules_py ${CMAKE_CURRENT_LIST_DIR}/enviro/weather.py enviro/weather)
endif()