from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS  # or PICO_EXPLORER_I2C_PINS or HEADER_I2C_PINS
from breakout_encoder_wheel import BreakoutEncoderWheel, NUM_BUTTONS

"""
How to read the buttons and rotary dial of the Encoder Wheel breakout, only when an interrupt occurs.

Press Ctrl+C to stop the program.
"""

# Constants
BUTTON_NAMES = ["Up", "Down", "Left", "Right", "Centre"]

# Create a new BreakoutEncoderWheel with a pin on the Pico specified as an interrupt
i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)
wheel = BreakoutEncoderWheel(i2c, interrupt=3)  # 3 for BG_BASE, 22 for EXPLORER_BASE, or 19 for some RP2040 boards
# If wiring the breakout via the qw/st connector, use the below line instead
# wheel = BreakoutEncoderWheel(i2c)

# Variables
last_pressed = [False] * NUM_BUTTONS
pressed = [False] * NUM_BUTTONS
position = 0
hue = 0.0

# Set the first LED
wheel.clear()
wheel.set_hsv(position, hue, 1.0, 1.0)
wheel.show()

# Clear any left over interrupt from previous code
wheel.clear_interrupt_flag()

# Loop forever
while True:

    # Check if the interrupt has fired
    if wheel.get_interrupt_flag():
        wheel.clear_interrupt_flag()

        # Read all of the encoder wheel's buttons
        for b in range(NUM_BUTTONS):
            pressed[b] = wheel.pressed(b)
            if pressed[b] != last_pressed[b]:
                print(BUTTON_NAMES[b], "Pressed" if pressed[b] else "Released")
            last_pressed[b] = pressed[b]

        # The interrupt may have come from several sources,
        # so check if it was a position change
        new_position = wheel.step()
        if new_position != position:
            # Record the new position (from 0 to 23)
            position = new_position
            print("Position = ", position)

            # Record a colour hue from 0.0 to 1.0
            hue = wheel.revolutions() % 1.0

            # Set the LED at the new position to the new hue
            wheel.clear()
            wheel.set_hsv(position, hue, 1.0, 1.0)
            wheel.show()
