#import gc
#import machine
#
#a = machine.Pin(12, machine.Pin.IN, machine.Pin.PULL_DOWN)
#b = machine.Pin(13, machine.Pin.IN, machine.Pin.PULL_DOWN)
#c = machine.Pin(14, machine.Pin.IN, machine.Pin.PULL_DOWN)
#
#if a.value() or b.value() or c.value():
#    for k in locals().keys():
#        if k not in ("gc"):
#            del locals()[k]
#    gc.collect()
#    __import__("launcher.py")
try:
    open("main.py", "r")
except OSError:
    with open("main.py", "w") as f:
        f.write("import launcher")
        f.flush()
