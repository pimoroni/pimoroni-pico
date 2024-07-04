# Cody Tolene
# Apache License 2.0

import uasyncio
import random


async def run(picoUnicorn, graphics):
    width = picoUnicorn.get_width()
    height = picoUnicorn.get_height()

    # Choose a direction for all raindrops at the start
    direction = random.choice([-0.3, 0.3])

    class Raindrop:
        def __init__(self):
            self.x = random.randint(0, width - 1)
            self.y = random.uniform(0, height - 1)
            self.speed_y = random.uniform(0.3, 0.7)  # Vertical speed
            self.speed_x = direction  # Horizontal speed, determined at start
            self.color = graphics.create_pen(
                0, 0, random.randint(150, 255)
            )  # Realistic blue color with variants

        async def update(self):
            graphics.set_pen(self.color)
            graphics.pixel(int(self.x), int(self.y))
            self.x += self.speed_x
            self.y += self.speed_y
            if self.y >= height or self.x < 0 or self.x >= width:
                self.y = 0
                self.x = random.randint(0, width - 1)

    raindrops = [
        Raindrop() for _ in range(30)
    ]  # Increase number of raindrops for realism

    while True:
        graphics.set_pen(graphics.create_pen(0, 0, 0))
        graphics.clear()

        for raindrop in raindrops:
            await raindrop.update()

        picoUnicorn.update(graphics)
        await uasyncio.sleep(0.05)  # Faster update interval for smoother animation


# This section of code is only for testing.
if __name__ == "__main__":
    from picounicorn import PicoUnicorn
    from picographics import PicoGraphics, DISPLAY_UNICORN_PACK

    picoUnicorn = PicoUnicorn()
    graphics = PicoGraphics(display=DISPLAY_UNICORN_PACK)
    uasyncio.run(run(picoUnicorn, graphics))
