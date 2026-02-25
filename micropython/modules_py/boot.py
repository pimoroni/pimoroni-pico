import builtins
import cppmem

# Switch C++ memory allocations to use MicroPython's heap
cppmem.set_mode(cppmem.MICROPYTHON)

# Since USB isn't enabled yet, catch any errors and save them
try:
    __import__("/boot.py")
    builtins.BOOT_ERROR = None
except Exception as e:  # noqa: BLE001
    builtins.BOOT_ERROR = e
