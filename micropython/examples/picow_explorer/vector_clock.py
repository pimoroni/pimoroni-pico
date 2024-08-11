import time
import gc

from picographics import PicoGraphics, DISPLAY_PICO_W_EXPLORER, PEN_RGB332
from picovector import PicoVector, Polygon, RegularPolygon, Rectangle, ANTIALIAS_X4


display = PicoGraphics(DISPLAY_PICO_W_EXPLORER, pen_type=PEN_RGB332)

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

hub = RegularPolygon(int(WIDTH / 2), int(HEIGHT / 2), 24, 5)

face = RegularPolygon(int(WIDTH / 2), int(HEIGHT / 2), 48, int(HEIGHT / 2))

print(time.localtime())

last_second = None

while True:
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
    display.circle(int(WIDTH / 2), int(HEIGHT / 2), int(HEIGHT / 2) - 4)

    display.set_pen(GREY)

    for a in range(60):
        tick_mark = Rectangle(int(WIDTH / 2) - 3, 10, 6, int(HEIGHT / 48))
        vector.rotate(tick_mark, 360 / 60.0 * a, int(WIDTH / 2), int(HEIGHT / 2))
        vector.translate(tick_mark, 0, 2)
        vector.draw(tick_mark)

    for a in range(12):
        hour_mark = Rectangle(int(WIDTH / 2) - 5, 10, 10, int(HEIGHT / 10))
        vector.rotate(hour_mark, 360 / 12.0 * a, int(WIDTH / 2), int(HEIGHT / 2))
        vector.translate(hour_mark, 0, 2)
        vector.draw(hour_mark)

    angle_second = second * 6
    second_hand_length = int(HEIGHT / 2) - int(HEIGHT / 8)
    second_hand = Polygon((-2, -second_hand_length), (-2, int(HEIGHT / 8)), (2, int(HEIGHT / 8)), (2, -second_hand_length))
    vector.rotate(second_hand, angle_second, 0, 0)
    vector.translate(second_hand, int(WIDTH / 2), int(HEIGHT / 2) + 5)

    angle_minute = minute * 6
    angle_minute += second / 10.0
    minute_hand_length = int(HEIGHT / 2) - int(HEIGHT / 24)
    minute_hand = Polygon((-5, -minute_hand_length), (-10, int(HEIGHT / 16)), (10, int(HEIGHT / 16)), (5, -minute_hand_length))
    vector.rotate(minute_hand, angle_minute, 0, 0)
    vector.translate(minute_hand, int(WIDTH / 2), int(HEIGHT / 2) + 5)

    angle_hour = (hour % 12) * 30
    angle_hour += minute / 2
    hour_hand_length = int(HEIGHT / 2) - int(HEIGHT / 8)
    hour_hand = Polygon((-5, -hour_hand_length), (-10, int(HEIGHT / 16)), (10, int(HEIGHT / 16)), (5, -hour_hand_length))
    vector.rotate(hour_hand, angle_hour, 0, 0)
    vector.translate(hour_hand, int(WIDTH / 2), int(HEIGHT / 2) + 5)

    display.set_pen(GREY)

    vector.draw(minute_hand)
    vector.draw(hour_hand)
    vector.draw(second_hand)

    display.set_pen(BLACK)

    for a in range(60):
        tick_mark = Rectangle(int(WIDTH / 2) - 3, 10, 6, int(HEIGHT / 48))
        vector.rotate(tick_mark, 360 / 60.0 * a, int(WIDTH / 2), int(HEIGHT / 2))
        vector.draw(tick_mark)

    for a in range(12):
        hour_mark = Rectangle(int(WIDTH / 2) - 5, 10, 10, int(HEIGHT / 10))
        vector.rotate(hour_mark, 360 / 12.0 * a, int(WIDTH / 2), int(HEIGHT / 2))
        vector.draw(hour_mark)

    vector.translate(minute_hand, 0, -5)
    vector.translate(hour_hand, 0, -5)
    vector.draw(minute_hand)
    vector.draw(hour_hand)

    display.set_pen(RED)
    vector.translate(second_hand, 0, -5)
    vector.draw(second_hand)
    vector.draw(hub)

    display.update()
    gc.collect()

    t_end = time.ticks_ms()
    print(f"Took {t_end - t_start}ms")
