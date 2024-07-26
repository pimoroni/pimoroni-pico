import time
from tiny_fx import TinyFX
from picofx import MonoPlayer
from picofx.mono import PulseFX, RandomFX, StaticFX, BlinkFX
from machine import Pin

"""
Play effects for each space themed "postcard" when someone walks past.
A PIR sensor is used to activate the effect, which will turn off after a certain time.

Press "Boot" to exit the program.
"""

# Constants
PIR_TIMEOUT = 10      # The time to wait before turning off the lights after the PIR sensor stops detecting motion (in seconds)
PIR_TIMEOUT_MS = (PIR_TIMEOUT * 1000)

# Variables
tiny = TinyFX()                     # Create a new TinyFX object
player = MonoPlayer(tiny.outputs)   # Create a new effect player to control TinyFX's mono outputs

# Set up a pin for reading the PIR sensor connected to TinyFX
pir = Pin(tiny.SENSOR_PIN, Pin.IN, Pin.PULL_UP)

# Set up the effects to play
player.effects = [
    PulseFX(0.2),
    RandomFX(0.01, 0.5, 1.0),
    StaticFX(0.5),
    BlinkFX(0.5),
    None,
    None
]

# Record the start time of the program
last_motion = time.ticks_ms()

# Wrap the code in a try block, to catch any exceptions (including KeyboardInterrupt)
try:
    # Loop until the "Boot" button is pressed
    while not tiny.boot_pressed():

        # Has motion been detected?
        if pir.value() == 1:
            player.start()   # Start the effects running
            last_motion = time.ticks_ms()

        if player.is_running():
            elapsed_time = time.ticks_diff(time.ticks_ms(), last_motion)
            if elapsed_time > PIR_TIMEOUT_MS:
                player.stop(reset_fx=True)
                tiny.clear()

# Stop any running effects and turn off all the outputs
finally:    
    player.stop()
    tiny.clear()
