export TERM=${TERM:="xterm-256color"}

function log_success {
	echo -e "$(tput setaf 2)$1$(tput sgr0)"
}

function log_inform {
	echo -e "$(tput setaf 6)$1$(tput sgr0)"
}

function log_warning {
	echo -e "$(tput setaf 1)$1$(tput sgr0)"
}

function clone_micropython_lib {
    git clone https://github.com/micropython/micropython-lib --depth=1
}

function clone_micropython {
    log_inform "Using MicroPython $MICROPYTHON_VERSION"
    git clone https://github.com/micropython/micropython --depth=1 --branch=$MICROPYTHON_VERSION
    cd micropython
    git submodule update --init lib/pico-sdk
    git submodule update --init lib/cyw43-driver
    git submodule update --init lib/lwip
    git submodule update --init lib/mbedtls
    git submodule update --init lib/micropython-lib
    git submodule update --init lib/tinyusb
    git submodule update --init lib/btstack
    cd mpy-cross
    make
    cd ../../
}

function build_deps {
    sudo apt update && sudo apt install ccache
}

function clone_pimoroni_pico {
    git clone https://github.com/micropython/micropython --branch=$PIMORONI_PICO_SHA --depth=1
}

function micropython_version {
    echo "MICROPY_GIT_TAG=$MICROPYTHON_VERSION, $BOARD_NAME $TAG_OR_SHA" >> $GITHUB_ENV
    echo "MICROPY_GIT_HASH=$MICROPYTHON_VERSION-$TAG_OR_SHA" >> $GITHUB_ENV
}

function hack_patch_micropython_disable_exceptions {
    cd micropython
    git apply $PIMORONI_PICO_DIR/micropython/micropython_nano_specs.patch
    cd ../
}

function hack_patch_pico_sdk {
    # pico-sdk-patch.sh will apply the patch if it exists
    cd micropython
    $PIMORONI_PICO_DIR/micropython/board/pico-sdk-patch.sh $MICROPY_BOARD
    cd ../
}

function cmake_configure {
    cmake -S micropython/ports/rp2 -B build-$BOARD_NAME \
    -DPICO_BUILD_DOCS=0 \
    -DUSER_C_MODULES=$USER_C_MODULES \
    -DMICROPY_BOARD_DIR=$MICROPY_BOARD_DIR \
    -DMICROPY_BOARD=$MICROPY_BOARD \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
}

function cmake_build {
    ccache --zero-stats || true
    cmake --build build-$BOARD_NAME -j 2
    ccache --show-stats || true
    cd build-$BOARD_NAME
    cp firmware.uf2 $RELEASE_FILE.uf2
}