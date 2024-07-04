# Cody Tolene
# Apache License 2.0

import uasyncio


async def run(picoUnicorn, graphics):
    white = graphics.create_pen(255, 255, 255)

    while True:
        graphics.set_pen(white)
        graphics.clear()
        picoUnicorn.update(graphics)
        await uasyncio.sleep(0.1)


# This section of code is only for testing.
if __name__ == "__main__":
    from picounicorn import PicoUnicorn
    from picographics import PicoGraphics, DISPLAY_UNICORN_PACK

    picoUnicorn = PicoUnicorn()
    graphics = PicoGraphics(display=DISPLAY_UNICORN_PACK)
    uasyncio.run(run(picoUnicorn, graphics))
