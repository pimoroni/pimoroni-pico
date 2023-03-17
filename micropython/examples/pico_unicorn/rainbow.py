from picounicorn import PicoUnicorn
import time

picounicorn = PicoUnicorn()


# From CPython Lib/colorsys.py
def hsv_to_rgb(h, s, v):
    if s == 0.0:
        return v, v, v
    i = int(h * 6.0)
    f = (h * 6.0) - i
    p = v * (1.0 - s)
    q = v * (1.0 - s * f)
    t = v * (1.0 - s * (1.0 - f))
    i = i % 6
    if i == 0:
        return v, t, p
    if i == 1:
        return q, v, p
    if i == 2:
        return p, v, t
    if i == 3:
        return p, q, v
    if i == 4:
        return t, p, v
    if i == 5:
        return v, p, q


w = picounicorn.get_width()
h = picounicorn.get_height()

while True:
    t = time.ticks_ms() / 3600
    for x in range(w):
        for y in range(h):
            r, g, b = [int(c * 255) for c in hsv_to_rgb(t + ((x + y) / w / 4), 1.0, 1.0)]
            picounicorn.set_pixel(x, y, r, g, b)
    time.sleep(1.0 / 60)
