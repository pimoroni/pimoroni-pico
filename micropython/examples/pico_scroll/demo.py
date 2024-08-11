import time
from picoscroll import PicoScroll, WIDTH, HEIGHT

scroll = PicoScroll()

i = 0
loop = 18
br_mult = 1
br_pressed = 32
tail = 12

while True:
    scroll.clear()
    for y in range(0, HEIGHT):
        for x in range(0, WIDTH):
            if x < 3 and y < 3 and scroll.is_pressed(scroll.BUTTON_A):
                scroll.set_pixel(x, y, br_pressed)
            elif x < 3 and y > 3 and scroll.is_pressed(scroll.BUTTON_B):
                scroll.set_pixel(x, y, br_pressed)
            elif x > WIDTH - 4 and y < 3 and scroll.is_pressed(scroll.BUTTON_X):
                scroll.set_pixel(x, y, br_pressed)
            elif x > WIDTH - 4 and y > 3 and scroll.is_pressed(scroll.BUTTON_Y):
                scroll.set_pixel(x, y, br_pressed)
            else:
                m = (x + (y * WIDTH)) % loop
                for b in range(0, loop):
                    if m == (i + (loop - b)) % loop and b < tail:
                        scroll.set_pixel(x, y, br_mult * (tail - b))

    scroll.show()
    i += 1
    if i >= loop:
        i = 0
    time.sleep(0.02)
