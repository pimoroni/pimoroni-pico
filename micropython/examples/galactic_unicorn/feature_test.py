from galactic import GalacticUnicorn

gu = GalacticUnicorn()


def gradient(r, g, b):
    for y in range(0, 11):
        for x in range(0, 53):
            # graphics.set_pen((r * x) / 52, (g * x) / 52, (b * x) / 52);
            gu.set_pixel(x, y, x)


def grid(ms, x, y):
    v = (x + y + (ms / 1000)) % 2
    if v == 0:
        gu.set_pixel(x, y, 255)
    else:
        gu.set_pixel(x, y, 0)
