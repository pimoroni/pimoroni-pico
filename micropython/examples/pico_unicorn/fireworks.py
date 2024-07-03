# Part of the Unicorn-Pi project:
# https://github.com/CodyTolene/Unicorn-Pi
#
# Cody Tolene
# Apache License 2.0

import random
import uasyncio

class Firework:
    def __init__(self, width, height, graphics):
        self.width = width
        self.height = height
        self.graphics = graphics
        # Randomly determine the launch x position, with 80% chance near the middle
        self.launch_x = random.randint(width // 4, 3 * width // 4) if random.random() < 0.8 else random.randint(1, width - 2)
        self.explosion_x = self.launch_x
        self.explosion_y = height // 2  # Center the explosion
        self.brightness = random.uniform(0.5, 1.0)  # Random brightness for distance effect
        # Create a color with the given brightness
        self.color = graphics.create_pen(
            int(self.brightness * random.randint(100, 255)),
            int(self.brightness * random.randint(100, 255)),
            int(self.brightness * random.randint(100, 255))
        )
        self.particles = []
        self.stage = "launch"  # Initial stage is launch
        self.y = height - 1  # Start from the bottom
        self.create_explosion_particles()  # Initialize explosion particles

    def create_explosion_particles(self):
        # Create random points for the explosion particles
        explosion_points = [(random.uniform(-1, 1), random.uniform(-1, 1)) for _ in range(10)]
        for dx, dy in explosion_points:
            self.particles.append({
                'x': self.explosion_x + dx,
                'y': self.explosion_y + dy,
                'dx': dx,
                'dy': dy,
                'color': self.color,
                'life': 1.0  # Particle life starts at 1.0
            })

    async def update(self):
        if self.stage == "launch":
            if self.y > self.height // 2:
                # Draw the firework as it launches
                self.graphics.set_pen(self.color)
                self.graphics.pixel(self.launch_x, self.y)
                self.y -= 1  # Move up
            else:
                self.stage = "explode"  # Transition to explode stage
        elif self.stage == "explode":
            if any(p['life'] > 0 for p in self.particles):
                for p in self.particles:
                    if p['life'] > 0:
                        p['x'] += p['dx']  # Move particle horizontally
                        p['y'] += p['dy']  # Move particle vertically
                        p['dy'] += 0.05  # Apply gravity effect
                        p['life'] -= 0.05  # Decrease life

                        # Draw particle if within bounds
                        if 0 <= int(p['x']) < self.width and 0 <= int(p['y']) < self.height:
                            brightness = int(255 * p['life'])
                            self.graphics.set_pen(self.graphics.create_pen(
                                brightness * (p['color'] >> 16 & 0xFF) // 255,
                                brightness * (p['color'] >> 8 & 0xFF) // 255,
                                brightness * (p['color'] & 0xFF) // 255))
                            self.graphics.pixel(int(p['x']), int(p['y']))

async def run(picoUnicorn, graphics):
    width = picoUnicorn.get_width()
    height = picoUnicorn.get_height()
    fireworks = []

    while True:
        # Clear the screen
        graphics.set_pen(graphics.create_pen(0, 0, 0))
        graphics.clear()

        # Add new fireworks at random intervals
        if random.random() < 0.1:
            fireworks.append(Firework(width, height, graphics))

        # Update all fireworks
        for firework in fireworks:
            await firework.update()

        # Remove fireworks that have completed their explosion
        fireworks = [
            firework for firework in fireworks 
                if any(p['life'] > 0 for p in firework.particles) 
                    or firework.stage == "launch"
        ]

        # Update the display
        picoUnicorn.update(graphics)

        await uasyncio.sleep(0.1)  # Sleep to control animation speed

# This section of code is only for testing.
if __name__ == "__main__":
    from picounicorn import PicoUnicorn
    from picographics import PicoGraphics, DISPLAY_UNICORN_PACK
    picoUnicorn = PicoUnicorn()
    graphics = PicoGraphics(display=DISPLAY_UNICORN_PACK)
    uasyncio.run(run(picoUnicorn, graphics))
