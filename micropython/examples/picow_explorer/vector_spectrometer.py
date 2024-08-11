import math
import time
from pimoroni_i2c import PimoroniI2C
from breakout_as7262 import BreakoutAS7262
from picographics import PicoGraphics, DISPLAY_PICO_W_EXPLORER, PEN_RGB332
from picovector import PicoVector, Polygon, RegularPolygon, ANTIALIAS_X4

PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}
i2c = PimoroniI2C(**PINS_PICO_EXPLORER)

# Set up the AS7262 Spectrometer
as7262 = BreakoutAS7262(i2c)
as7262.set_gain(BreakoutAS7262.X16)
as7262.set_measurement_mode(BreakoutAS7262.CONT_ROYGBR)
as7262.set_illumination_current(BreakoutAS7262.MA12)
as7262.set_indicator_current(BreakoutAS7262.MA4)
as7262.set_leds(True, True)

# Set up the display
display = PicoGraphics(DISPLAY_PICO_W_EXPLORER, pen_type=PEN_RGB332)
display.set_backlight(0.8)

# Set up PicoVector
vector = PicoVector(display)
vector.set_antialiasing(ANTIALIAS_X4)

# Load an Alright Font, find this in common/AdvRe.af
result = vector.set_font("/AdvRe.af", 30)

WIDTH, HEIGHT = display.get_bounds()

CENTER_X = int(WIDTH / 2)
CENTER_Y = int(HEIGHT / 2)

RADIUS = 90
DEBUG = False

RED = display.create_pen(255, 0, 0)
ORANGE = display.create_pen(255, 128, 0)
YELLOW = display.create_pen(255, 255, 0)
GREEN = display.create_pen(0, 255, 0)
BLUE = display.create_pen(0, 0, 255)
VIOLET = display.create_pen(255, 0, 255)

BLACK = display.create_pen(0, 0, 0)
GREY = display.create_pen(128, 128, 128)
WHITE = display.create_pen(255, 255, 255)

LABELS = ["R", "O", "Y", "G", "B", "V"]
COLS = [RED, ORANGE, YELLOW, GREEN, BLUE, VIOLET]


# Custom regular_polygon function to give each point its own "radius"
def regular_polygon(o_x, o_y, radius, rotation):
    sides = 6
    angle = math.radians(360 / sides)
    rotation = math.radians(rotation)

    points = []

    for side in range(sides):
        current_angle = side * angle + rotation
        x = math.cos(current_angle) * radius[side]
        y = math.sin(current_angle) * radius[side]
        points.append((int(x) + o_x, int(y) + o_y))

    return points


lines = RegularPolygon(CENTER_X, CENTER_Y, 6, RADIUS)
label_points = list(RegularPolygon(CENTER_X, CENTER_Y, 6, RADIUS * 0.7, -(360 / 12)))


while True:
    # Clear to black
    display.set_pen(BLACK)
    display.clear()

    # Add the title
    display.set_pen(WHITE)
    vector.text("Spectrograph", 5, -5)

    # Get the spectrometer readings
    reading = list(as7262.read())

    # Print out the readings
    if DEBUG:
        for i in range(6):
            print(f"{LABELS[i]}: {reading[i]:0.2f}", end=" ")
        print("")

    # Draw the lines separating each section
    display.set_pen(GREY)
    for (x, y) in lines:
        display.line(CENTER_X, CENTER_Y, int(x), int(y))

    # Scale readings for display
    for i in range(6):
        reading[i] = int(reading[i] / 3.0)
        reading[i] = min(reading[i], RADIUS)

    # Create a 6 point polygon with each points distance from the center
    # scaled by the corresponding reading.
    points = regular_polygon(CENTER_X, CENTER_Y, reading, 0)

    # Split the polygon into six triangles, one for each channel
    # draw each one, along with its corresponding label
    point_a = points[-1]
    for i in range(6):
        point_b = points[i]
        label_x, label_y = label_points[i]
        display.set_pen(COLS[i])
        vector.text(LABELS[i], int(label_x) - 5, int(label_y) - 20)
        vector.draw(Polygon(point_a, point_b, (CENTER_X, CENTER_Y)))
        point_a = point_b

    display.update()
    time.sleep(1.0 / 60)
