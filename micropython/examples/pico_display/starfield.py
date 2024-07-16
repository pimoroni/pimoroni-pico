# Travel through a Windows 3.1-esque starfield, with stars growing as they get 'closer'.
# If you have a Display Pack 2.0" or 2.8" use DISPLAY_PICO_DISPLAY_2 instead of DISPLAY_PICO_DISPLAY

from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY
import random

# Constants to play with
NUMBER_OF_STARS = 200
TRAVEL_SPEED = 1.2
STAR_GROWTH = 0.12

# Set up our display
graphics = PicoGraphics(display=DISPLAY_PICO_DISPLAY)

WIDTH, HEIGHT = graphics.get_bounds()

BLACK = graphics.create_pen(0, 0, 0)
WHITE = graphics.create_pen(255, 255, 255)

stars = []


def new_star():
    # Create a new star, with initial x, y, and size
    # Initial x will fall between -WIDTH / 2 and +WIDTH / 2 and y between -HEIGHT/2 and +HEIGHT/2
    # These are relative values for now, treating (0, 0) as the centre of the screen.
    star = [random.randint(0, WIDTH) - WIDTH // 2, random.randint(0, HEIGHT) - HEIGHT // 2, 0.5]
    return star


for i in range(0, NUMBER_OF_STARS):
    stars.append(new_star())

while True:
    graphics.set_pen(BLACK)
    graphics.clear()
    graphics.set_pen(WHITE)
    for i in range(0, NUMBER_OF_STARS):
        # Load a star from the stars list
        s = stars[i]

        # Update x
        s[0] = s[0] * TRAVEL_SPEED

        # Update y
        s[1] = s[1] * TRAVEL_SPEED

        if s[0] <= - WIDTH // 2 or s[0] >= WIDTH // 2 or s[1] <= - HEIGHT // 2 or s[1] >= HEIGHT // 2 or s[2] >= 5:
            # This star has fallen off the screen (or rolled dead centre and grown too big!)
            # Replace it with a new one
            s = new_star()

        # Grow the star as it travels outward
        s[2] += STAR_GROWTH

        # Save the updated star to the list
        stars[i] = s

        # Draw star, adding offsets to our relative coordinates to allow for (0, 0) being in the top left corner.
        graphics.circle(int(s[0]) + WIDTH // 2, int(s[1]) + HEIGHT // 2, int(s[2]))
    graphics.update()
