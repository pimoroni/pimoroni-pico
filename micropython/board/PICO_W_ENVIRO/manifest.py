include("$(PORT_DIR)/boards/manifest.py")

# https://github.com/micropython/micropython-lib/blob/master/micropython/bundles/bundle-networking/manifest.py
require("bundle-networking")
require("umqtt.simple")

# Bluetooth
require("aioble")

freeze("../../modules_py", "pimoroni.py")
freeze("../../modules_py", "boot.py")
