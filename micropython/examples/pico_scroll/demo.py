import time
import picoscroll as scroll

scroll.init()
i = 0
loop = 18
br_mult = 1
br_pressed = 32
tail = 12

width = scroll.get_width()
height = scroll.get_height()

while True:
    scroll.clear()
    for y in range(0, height):
        for x in range(0, width):
            if x < 3 and y < 3 and scroll.is_pressed(scroll.BUTTON_A):
                scroll.set_pixel(x, y, br_pressed)
            elif x < 3 and y > 3 and scroll.is_pressed(scroll.BUTTON_B):
                scroll.set_pixel(x, y, br_pressed)
            elif x > width - 4 and y < 3 and scroll.is_pressed(scroll.BUTTON_X):
                scroll.set_pixel(x, y, br_pressed)
            elif x > width - 4 and y > 3 and scroll.is_pressed(scroll.BUTTON_Y):
                scroll.set_pixel(x, y, br_pressed)
            else:
                m = (x + (y * width)) % loop
                for b in range(0, loop):
                    if m == (i + (loop - b)) % loop and b < tail:
                        scroll.set_pixel(x, y, br_mult * (tail - b))

    scroll.update()
    i += 1
    if i >= loop:
        i = 0
    time.sleep(0.02)
