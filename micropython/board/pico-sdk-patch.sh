BOARD=$1
FIXUP_DIR="$(dirname -- "$(readlink -f "${BASH_SOURCE}")")"

MPY_DIR=`pwd`

if [[ -f "$FIXUP_DIR/$BOARD/pico_sdk.patch" ]]; then
    echo "Applying pico_sdk.patch"
    cd $MPY_DIR/lib/pico-sdk
    git apply "$FIXUP_DIR/$BOARD/pico_sdk.patch"
fi