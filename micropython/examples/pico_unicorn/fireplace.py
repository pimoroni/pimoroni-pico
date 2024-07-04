# Cody Tolene
# Apache License 2.0
# Part of the Unicorn-Pi project:
# https://github.com/CodyTolene/Unicorn-Pi

import random
import micropython
import uasyncio


async def run(picoUnicorn, graphics):
    fire_colours = [
        graphics.create_pen(0, 0, 0),
        graphics.create_pen(20, 20, 20),
        graphics.create_pen(180, 30, 0),
        graphics.create_pen(220, 160, 0),
        graphics.create_pen(255, 255, 180),
    ]

    width = picoUnicorn.get_width()
    height = picoUnicorn.get_height() + 2

    heat = [[0.0 for y in range(height)] for x in range(width)]
    damping_factor = 0.80  # Increased damping factor to reduce flame height

    @micropython.native  # noqa: F821
    def update():
        # Take local references as it's quicker than accessing the global
        # and we access it a lot in this method
        _heat = heat

        # Clear the bottom rows and then add new fire seeds to them
        for x in range(width):
            _heat[x][height - 1] = random.uniform(0.3, 0.6)  # Lower initial intensity
            _heat[x][height - 2] = random.uniform(0.3, 0.6)

        factor = damping_factor / 5.0
        for y in range(height - 3, -1, -1):  # Ensure y doesn't go out of bounds
            for x in range(1, width - 1):
                sum_heat_y = _heat[x][y]
                sum_heat_y1 = _heat[x][y + 1]
                sum_heat_y2 = _heat[x][y + 2]
                sum_heat_x1y1 = _heat[x - 1][y + 1]
                sum_heat_x2y1 = _heat[x + 1][y + 1]

                sum_heat_y = sum_heat_y + sum_heat_y1 + sum_heat_y2
                sum_heat_x = sum_heat_x1y1 + sum_heat_x2y1

                sum_heat = sum_heat_y + sum_heat_x

                _heat[x][y] = sum_heat * factor

    @micropython.native  # noqa: F821
    def draw():
        # Take local references as it's quicker than accessing the global
        # and we access it a lot in this method
        _graphics = graphics
        _heat = heat
        _set_pen = graphics.set_pen
        _pixel = graphics.pixel
        _fire_colours = fire_colours

        for y in range(picoUnicorn.get_height()):
            for x in range(picoUnicorn.get_width()):
                value = _heat[x][y + 2]  # Adjust indexing to stay within bounds
                if value < 0.15:
                    _set_pen(_fire_colours[0])
                elif value < 0.3:
                    _set_pen(_fire_colours[1])
                elif value < 0.45:
                    _set_pen(_fire_colours[2])
                elif value < 0.6:
                    _set_pen(_fire_colours[3])
                else:
                    _set_pen(_fire_colours[4])
                _pixel(x, y)

        picoUnicorn.update(_graphics)

    while True:
        update()
        draw()
        await uasyncio.sleep(
            1.0 / 30
        )  # Increased sleep time to slow down the animation

        # Introduce a small delay in the update loop to reduce flickering
        await uasyncio.sleep(1.0 / 10)


# This section of code is only for testing.
if __name__ == "__main__":
    from picounicorn import PicoUnicorn
    from picographics import PicoGraphics, DISPLAY_UNICORN_PACK

    picoUnicorn = PicoUnicorn()
    graphics = PicoGraphics(display=DISPLAY_UNICORN_PACK)
    uasyncio.run(run(picoUnicorn, graphics))
