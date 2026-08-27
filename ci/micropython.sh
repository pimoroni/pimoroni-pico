export TERM=${TERM:="xterm-256color"}

# Pinned here rather than in the workflow so a local build matches CI without
# having to read the YAML first. Override in the environment to try another.
# RP2350 boards need the cyw43 dynamic pin support, which is v1.29.0 plus five
# commits on our fork and not yet upstream.
MICROPYTHON_FLAVOUR="${MICROPYTHON_FLAVOUR:-pimoroni}"
MICROPYTHON_VERSION="${MICROPYTHON_VERSION:-cyw43-dynamic-pins}"

PY_DECL_VERSION="${PY_DECL_VERSION:-v0.0.4}"


function log_success {
	echo -e "$(tput setaf 2)$1$(tput sgr0)"
}

function log_inform {
	echo -e "$(tput setaf 6)$1$(tput sgr0)"
}

function log_warning {
	echo -e "$(tput setaf 1)$1$(tput sgr0)"
}

function ci_debug {
    log_inform "Project root: $CI_PROJECT_ROOT"
    log_inform "Build root:   $CI_BUILD_ROOT"
}

# Resolves the paths for a board and checks they exist, so a typo fails here
# rather than somewhere deep inside cmake. BOARD_NAME picks the usermodules
# cmake file (eg: pico -> micropython-pico.cmake) and MICROPY_BOARD picks the
# board directory (eg: RPI_PICO).
function ci_board_paths {
    BOARD_NAME="${1:-$BOARD_NAME}"
    MICROPY_BOARD="${2:-$MICROPY_BOARD}"

    if [ -z "$BOARD_NAME" ] || [ -z "$MICROPY_BOARD" ]; then
        log_warning "Expected a board name and a MicroPython board, eg: pico RPI_PICO"
        return 1
    fi

    MICROPY_BOARD_DIR="$CI_PROJECT_ROOT/micropython/board/$MICROPY_BOARD"
    USER_C_MODULES="$CI_PROJECT_ROOT/micropython/modules/micropython-$BOARD_NAME.cmake"
    BUILD_DIR="$CI_BUILD_ROOT/build-$BOARD_NAME"
    RELEASE_FILE="${CI_RELEASE_FILENAME:-$BOARD_NAME}"

    if [ ! -f "$MICROPY_BOARD_DIR/mpconfigboard.h" ]; then
        log_warning "No mpconfigboard.h in $MICROPY_BOARD_DIR"
        return 1
    fi

    if [ ! -f "$USER_C_MODULES" ]; then
        log_warning "No usermodules cmake at $USER_C_MODULES"
        return 1
    fi
}

function ci_micropython_clone {
    log_inform "Using MicroPython $MICROPYTHON_FLAVOUR/$MICROPYTHON_VERSION"
    git clone https://github.com/$MICROPYTHON_FLAVOUR/micropython -b $MICROPYTHON_VERSION --depth=1 "$CI_BUILD_ROOT/micropython" || return $?
    for module in lib/pico-sdk lib/cyw43-driver lib/lwip lib/mbedtls lib/micropython-lib lib/tinyusb lib/btstack; do
        git -C "$CI_BUILD_ROOT/micropython" submodule update --init --depth=1 -- $module || return $?
    done
}

function ci_tools_clone {
    mkdir -p "$CI_BUILD_ROOT/tools"
    git clone https://github.com/gadgetoid/py_decl -b "$PY_DECL_VERSION" --depth=1 "$CI_BUILD_ROOT/tools/py_decl" || return $?
}

function ci_micropython_build_mpy_cross {
    ccache --zero-stats || true
    CROSS_COMPILE="ccache " USER_C_MODULES= make -C "$CI_BUILD_ROOT/micropython/mpy-cross" || return $?
    ccache --show-stats || true
}

function ci_apt_install_build_deps {
    sudo apt update && sudo apt install ccache
}

function ci_prepare_all {
    ci_micropython_clone || return $?
    ci_tools_clone || return $?
    ci_micropython_build_mpy_cross || return $?
}

# py/makeversionhdr.py picks these up from the environment.
function ci_micropython_version {
    export MICROPY_GIT_TAG="$MICROPYTHON_VERSION, $BOARD_NAME ${TAG_OR_SHA:-local}"
    export MICROPY_GIT_HASH="$MICROPYTHON_VERSION-${TAG_OR_SHA:-local}"
}

# pico-sdk-patch.sh applies board/$MICROPY_BOARD/pico_sdk.patch if there is one,
# and takes the MicroPython checkout as its working directory. The subshell is so
# that requirement doesn't leak into the caller's cwd.
function ci_hack_patch_pico_sdk {
    ci_board_paths "$1" "$2" || return $?
    (
        cd "$CI_BUILD_ROOT/micropython" || exit $?
        "$CI_PROJECT_ROOT/micropython/board/pico-sdk-patch.sh" "$MICROPY_BOARD"
    )
}

# Any arguments after the board are passed through to cmake.
function ci_cmake_configure {
    ci_board_paths "$1" "$2" || return $?
    if [ $# -gt 0 ]; then shift; fi
    if [ $# -gt 0 ]; then shift; fi
    ci_micropython_version

    cmake -S "$CI_BUILD_ROOT/micropython/ports/rp2" -B "$BUILD_DIR" \
    -DPICO_BUILD_DOCS=0 \
    -DUSER_C_MODULES="$USER_C_MODULES" \
    -DMICROPY_BOARD_DIR="$MICROPY_BOARD_DIR" \
    -DMICROPY_BOARD="$MICROPY_BOARD" \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    "$@"
}

function ci_cmake_build {
    ci_board_paths "$1" "$2" || return $?
    ci_micropython_version

    ccache --zero-stats || true
    cmake --build "$BUILD_DIR" -j "${CI_BUILD_JOBS:-2}" || return $?
    ccache --show-stats || true

    log_inform "Copying firmware.uf2 to $CI_BUILD_ROOT/$RELEASE_FILE.uf2"
    cp "$BUILD_DIR/firmware.uf2" "$CI_BUILD_ROOT/$RELEASE_FILE.uf2" || return $?
}

function ci_py_decl_verify {
    ci_board_paths "$1" "$2" || return $?
    python3 "$CI_BUILD_ROOT/tools/py_decl/py_decl.py" --to-json --verify "$CI_BUILD_ROOT/$RELEASE_FILE.uf2"
}

# Build one board start to finish, for local use.
function ci_build_board {
    ci_cmake_configure "$1" "$2" || return $?
    ci_cmake_build "$1" "$2" || return $?
}

if [ -z ${CI_USE_ENV+x} ] || [ -z ${CI_PROJECT_ROOT+x} ] || [ -z ${CI_BUILD_ROOT+x} ]; then
    SCRIPT_PATH=${BASH_SOURCE-$0}
    SCRIPT_PATH=$(dirname "$SCRIPT_PATH")
    CI_PROJECT_ROOT=$(realpath "$SCRIPT_PATH/..")
    CI_BUILD_ROOT=$(pwd)
fi

ci_debug
