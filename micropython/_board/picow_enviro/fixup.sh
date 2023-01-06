SRC_DIR=$1
DST_DIR=$2

echo "Applying wakeup_gpio.patch"
cd "$DST_DIR/../../lib/pico-sdk"
git apply "$SRC_DIR/wakeup_gpio.patch"
echo "Applying wakeup_vsys.patch"
cd "../../"
ls
git apply "$SRC_DIR/wakeup_vsys.patch"