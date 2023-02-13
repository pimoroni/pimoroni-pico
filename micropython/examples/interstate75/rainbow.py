import time
import interstate75

i75 = interstate75.Interstate75(display=interstate75.DISPLAY_INTERSTATE75_64X64, stb_invert=True)
graphics = i75.display

width = i75.width
height = i75.height

devs = 1.0 / height


@micropython.native  # noqa: F821
def draw(offset):
    for x in range(width):
        graphics.set_pen(graphics.create_pen_hsv(devs * x + offset, 1.0, 0.5))
        for y in range(height):

            graphics.pixel(x, y)

    i75.update(graphics)


animate = True
stripe_width = 3.0
speed = 5.0
offset = 0.0

phase = 0
while True:

    if animate:
        phase += speed

    start = time.ticks_ms()
    offset += 0.05
    draw(offset)

    print("total took: {} ms".format(time.ticks_ms() - start))
