from breakout_as7262 import BreakoutAS7262
import picoexplorer as display
import time

width = display.get_width()
height = display.get_height()

bar_width = width // 6
bar_height = height

display_buffer = bytearray(width * height * 2)  # 2-bytes per pixel (RGB565)
display.init(display_buffer)

as7 = BreakoutAS7262()

integration_time = 10  # integration time in milliseconds, max ~90ms

as7.set_gain(as7.X64)
as7.set_integration_time(integration_time)
as7.set_measurement_mode(as7.CONT_ROYGBR)
as7.set_leds(True, True)


def draw_bar(v, i):
    current_bar_top = int(bar_height - (bar_height * v))
    # Drawing outside of the display region will cause horrible, horrible crashes
    current_bar_top = max(0, current_bar_top)
    current_bar_height = bar_height - current_bar_top
    display.rectangle(i * bar_width, current_bar_top, bar_width, current_bar_height - 1)


while True:
    r, o, y, g, b, v = as7.read()
    m = max(r, o, y, g, b, v)

    display.set_pen(0, 0, 0)
    display.clear()

    # Red
    display.set_pen(255, 0, 0)
    draw_bar(r / m, 0)

    # Orange
    display.set_pen(255, 128, 0)
    draw_bar(o / m, 1)

    # Yellow
    display.set_pen(255, 255, 0)
    draw_bar(y / m, 2)

    # Green
    display.set_pen(0, 255, 0)
    draw_bar(g / m, 3)

    # Blue
    display.set_pen(0, 0, 255)
    draw_bar(b / m, 4)

    # Violet
    display.set_pen(255, 0, 255)
    draw_bar(v / m, 5)

    display.update()

    time.sleep(integration_time / 1000.0)
