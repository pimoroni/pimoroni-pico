# Cody Tolene
# Apache License 2.0
# Part of the Unicorn-Pi project:
# https://github.com/CodyTolene/Unicorn-Pi

import uasyncio


async def run(picoUnicorn, graphics):
    white = graphics.create_pen(255, 255, 255)
    black = graphics.create_pen(0, 0, 0)

    async def draw_sos():
        # S: "· · ·"
        for _ in range(3):
            graphics.set_pen(white)
            graphics.clear()
            picoUnicorn.update(graphics)
            await uasyncio.sleep(0.3)  # Dot duration
            graphics.set_pen(black)
            graphics.clear()
            picoUnicorn.update(graphics)
            await uasyncio.sleep(0.3)  # Space between parts of the same letter

        await uasyncio.sleep(0.3)  # Space between letters

        # O: "− − −"
        for _ in range(3):
            graphics.set_pen(white)
            graphics.clear()
            picoUnicorn.update(graphics)
            await uasyncio.sleep(0.9)  # Dash duration
            graphics.set_pen(black)
            graphics.clear()
            picoUnicorn.update(graphics)
            await uasyncio.sleep(0.3)  # Space between parts of the same letter

        await uasyncio.sleep(0.3)  # Space between letters

        # S: "· · ·"
        for _ in range(3):
            graphics.set_pen(white)
            graphics.clear()
            picoUnicorn.update(graphics)
            await uasyncio.sleep(0.3)  # Dot duration
            graphics.set_pen(black)
            graphics.clear()
            picoUnicorn.update(graphics)
            await uasyncio.sleep(0.3)  # Space between parts of the same letter

    while True:
        await draw_sos()
        await uasyncio.sleep(1.5)  # Space between repetitions of SOS signal


# This section of code is only for testing.
if __name__ == "__main__":
    from picounicorn import PicoUnicorn
    from picographics import PicoGraphics, DISPLAY_UNICORN_PACK

    picoUnicorn = PicoUnicorn()
    graphics = PicoGraphics(display=DISPLAY_UNICORN_PACK)
    uasyncio.run(run(picoUnicorn, graphics))
