try:
    open("main.py", "r")
except OSError:
    with open("main.py", "w") as f:
        f.write("import _launcher")
        f.flush()
