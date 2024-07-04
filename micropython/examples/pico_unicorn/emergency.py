# Cody Tolene
# Apache License 2.0

import uasyncio


async def run(picoUnicorn, graphics):
    red = graphics.create_pen(255, 0, 0)
    blue = graphics.create_pen(0, 0, 255)

    while True:
        # Flash red
        graphics.set_pen(red)
        graphics.clear()
        picoUnicorn.update(graphics)
        await uasyncio.sleep(0.5)

        # Flash blue
        graphics.set_pen(blue)
        graphics.clear()
        picoUnicorn.update(graphics)
        await uasyncio.sleep(0.5)


# This section of code is only for testing.
if __name__ == "__main__":
    from picounicorn import PicoUnicorn
    from picographics import PicoGraphics, DISPLAY_UNICORN_PACK

    picoUnicorn = PicoUnicorn()
    graphics = PicoGraphics(display=DISPLAY_UNICORN_PACK)
    uasyncio.run(run(picoUnicorn, graphics))
