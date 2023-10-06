include("$(PORT_DIR)/boards/manifest.py")

# https://github.com/micropython/micropython-lib/blob/master/micropython/bundles/bundle-networking/manifest.py
require("bundle-networking")
require("urllib.urequest")
require("umqtt.simple")

# SD Card
require("sdcard")

# Bluetooth
require("aioble")

freeze("../../modules_py", "pimoroni.py")
freeze("../../modules_py", "boot.py")