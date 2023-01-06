SRC_DIR=$1
DST_DIR=$2

echo "Applying wakeup_gpio.patch"
cd "$DST_DIR/../../"
echo "$DST_DIR/../../"
echo "`pwd`"
ls
echo "$SRC_DIR"
ls "$SRC_DIR"
normalDir="`cd "$SRC_DIR";pwd`"
echo "${normalDir}"
git apply "$SRC_DIR/wakeup_gpio.patch"