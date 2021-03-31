if (TARGET pico_audio_i2s)
    # You must supply the Pico Extras path to build this example
    # -DPICO_SDK_POST_LIST_DIRS=/path/to/pico-extras
    # You can use "cmake.configureSettings" in VSCode settings.json
    # Grab the extra libraries here: https://github.com/raspberrypi/pico-extras
    add_executable(
    audio
    demo.cpp
    )

    # Pull in pico libraries that we need
    target_link_libraries(audio pico_stdlib pico_audio_i2s)

    target_compile_definitions(audio PRIVATE
    # compile time configuration of I2S
    PICO_AUDIO_I2S_MONO_INPUT=1
    #define for our example code
    USE_AUDIO_I2S=1
    )

    # create map/bin/hex file etc.
    pico_add_extra_outputs(audio)
endif()