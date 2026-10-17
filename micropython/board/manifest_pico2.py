# SD Card
require("sdcard")

freeze("../modules_py", "gfx_pack.py")

freeze("../modules_py", "pimoroni.py")
freeze("../modules_py", "boot.py")

# The LTE modem driver, from its own repository as a submodule
include("../modules_py/lte-micropython")

freeze("../modules_py", "spce.py")
