import time
import gc

from picographics import PicoGraphics, DISPLAY_TUFTY_2040, PEN_RGB565 as PEN
from picovector import PicoVector, Transform, Polygon, ANTIALIAS_X4


display = PicoGraphics(DISPLAY_TUFTY_2040, pen_type=PEN)

vector = PicoVector(display)
vector.set_antialiasing(ANTIALIAS_X4)

RED = display.create_pen(200, 0, 0)
BLACK = display.create_pen(0, 0, 0)
GREY = display.create_pen(200, 200, 200)
WHITE = display.create_pen(255, 255, 255)

"""
# Redefine colours for a Blue clock
RED = display.create_pen(200, 0, 0)
BLACK = display.create_pen(135, 159, 169)
GREY = display.create_pen(10, 40, 50)
WHITE = display.create_pen(14, 60, 76)
"""

WIDTH, HEIGHT = display.get_bounds()

t = Transform()

hub = Polygon()
hub.circle(int(WIDTH / 2), int(HEIGHT / 2), 5)

face = Polygon()
face.circle(int(WIDTH / 2), int(HEIGHT / 2), int(HEIGHT / 2))

tick_mark = Polygon()
tick_mark.rectangle(int(WIDTH / 2) - 3, 10, 6, int(HEIGHT / 48))

hour_mark = Polygon()
hour_mark.rectangle(int(WIDTH / 2) - 5, 10, 10, int(HEIGHT / 10))

second_hand_length = int(HEIGHT / 2) - int(HEIGHT / 8)
second_hand = Polygon()
second_hand.path((-2, -second_hand_length), (-2, int(HEIGHT / 8)), (2, int(HEIGHT / 8)), (2, -second_hand_length))

minute_hand_length = int(HEIGHT / 2) - int(HEIGHT / 24)
minute_hand = Polygon()
minute_hand.path((-5, -minute_hand_length), (-10, int(HEIGHT / 16)), (10, int(HEIGHT / 16)), (5, -minute_hand_length))

hour_hand_length = int(HEIGHT / 2) - int(HEIGHT / 8)
hour_hand = Polygon()
hour_hand.path((-5, -hour_hand_length), (-10, int(HEIGHT / 16)), (10, int(HEIGHT / 16)), (5, -hour_hand_length))

print(time.localtime())

last_second = None

vector.set_transform(None)
while True:
    t.reset()
    t_start = time.ticks_ms()
    year, month, day, hour, minute, second, _, _ = time.localtime()

    if last_second == second:
        continue

    last_second = second

    display.set_pen(0)
    display.clear()

    display.set_pen(BLACK)
    display.circle(int(WIDTH / 2), int(HEIGHT / 2), int(HEIGHT / 2))
    display.set_pen(WHITE)
    # display.circle(int(WIDTH / 2), int(HEIGHT / 2), int(HEIGHT / 2) - 4)

    vector.draw(face)

    display.set_pen(GREY)

    vector.set_transform(t)

    t.translate(0, 2)
    for a in range(60):
        t.rotate(360 / 60.0, (WIDTH / 2, HEIGHT / 2))
        if a % 5 == 0:
            vector.draw(hour_mark)
        else:
            vector.draw(tick_mark)

    t.reset()
    angle_minute = minute * 6
    angle_minute += second / 10.0
    t.translate(WIDTH / 2, HEIGHT / 2 + 5)
    t.rotate(angle_minute, (0, 0))
    vector.draw(minute_hand)

    t.reset()
    angle_hour = (hour % 12) * 30
    angle_hour += minute / 2
    t.translate(WIDTH / 2, HEIGHT / 2 + 5)
    t.rotate(angle_hour, (0, 0))
    vector.draw(hour_hand)

    t.reset()
    t.translate(WIDTH / 2, HEIGHT / 2 + 5)
    t.rotate(second * 6, (0, 0))
    vector.draw(second_hand)

    display.set_pen(BLACK)

    t.reset()
    for a in range(60):
        t.rotate(360 / 60.0, (WIDTH / 2, HEIGHT / 2))
        if a % 5 == 0:
            vector.draw(hour_mark)
        else:
            vector.draw(tick_mark)

    t.reset()
    angle_minute = minute * 6
    angle_minute += second / 10.0
    t.translate(WIDTH / 2, HEIGHT / 2)
    t.rotate(angle_minute, (0, 0))
    vector.draw(minute_hand)

    t.reset()
    angle_hour = (hour % 12) * 30
    angle_hour += minute / 2
    t.translate(WIDTH / 2, HEIGHT / 2)
    t.rotate(angle_hour, (0, 0))
    vector.draw(hour_hand)

    display.set_pen(RED)

    t.reset()
    t.translate(WIDTH / 2, HEIGHT / 2)
    t.rotate(second * 6, (0, 0))
    vector.draw(second_hand)

    vector.set_transform(None)
    vector.draw(hub)

    display.update()
    mem = gc.mem_free()
    gc.collect()
    used = gc.mem_free() - mem

    t_end = time.ticks_ms()
    print(f"Took {t_end - t_start}ms, mem free: {gc.mem_free()} {used}")
