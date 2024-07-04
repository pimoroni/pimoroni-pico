# Cody Tolene
# Apache License 2.0
# Part of the Unicorn-Pi project:
# https://github.com/CodyTolene/Unicorn-Pi

import random
import uasyncio


async def run(picoUnicorn, graphics):
    width = picoUnicorn.get_width()
    height = picoUnicorn.get_height()

    BRIGHT_GREEN = (0, 255, 0)
    MEDIUM_GREEN = (0, 128, 0)
    DARK_GREEN = (0, 64, 0)
    GREEN_VARIATIONS = [BRIGHT_GREEN, MEDIUM_GREEN, DARK_GREEN]

    def clear():
        for x in range(width):
            for y in range(height):
                picoUnicorn.set_pixel(x, y, 0, 0, 0)

    def create_random_dot():
        x = random.randint(0, width - 1)
        color = random.choice(GREEN_VARIATIONS)
        return (x, 0, color, False)

    def create_trailing_dots(dot):
        x, y, color, stopping = dot
        trailing_dots = []
        dimming_factors = [0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1]
        for i, factor in enumerate(dimming_factors):
            trailing_y = y - (i + 1)
            if trailing_y >= 0:
                trailing_color = dim_color(color, factor)
                trailing_dots.append((x, trailing_y, trailing_color, False))
        return trailing_dots

    def dim_color(color, factor):
        return tuple(max(0, int(c * factor)) for c in color)

    def move_dots(dots):
        new_dots = []
        for x, y, color, stopping in dots:
            if stopping or (y < height - 1 and random.random() < 0.1):
                stopping = True
                color = dim_color(color, 0.9)
            else:
                y += 1
            if 0 <= x < width and 0 <= y < height:
                new_dots.append((x, y, color, stopping))
        return new_dots

    def draw_dots(dots):
        for x, y, color, stopping in dots:
            if 0 <= x < width and 0 <= y < height:
                picoUnicorn.set_pixel(x, y, *color)

    def fade_dots(dots):
        faded_dots = []
        for x, y, color, stopping in dots:
            faded_color = dim_color(color, 0.95)
            if sum(faded_color) > 0:
                faded_dots.append((x, y, faded_color, stopping))
        return faded_dots

    dots = []
    while True:
        clear()
        if random.random() < 0.8:
            new_dot = create_random_dot()
            dots.append(new_dot)
            dots.extend(create_trailing_dots(new_dot))
        dots = move_dots(dots)
        draw_dots(dots)
        dots = fade_dots(dots)
        await uasyncio.sleep(0.1)


# This section of code is only for testing.
if __name__ == "__main__":
    from picounicorn import PicoUnicorn
    from picographics import PicoGraphics, DISPLAY_UNICORN_PACK

    picoUnicorn = PicoUnicorn()
    graphics = PicoGraphics(display=DISPLAY_UNICORN_PACK)
    uasyncio.run(run(picoUnicorn, graphics))
