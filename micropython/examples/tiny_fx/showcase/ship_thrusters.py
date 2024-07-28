from tiny_fx import TinyFX
from picofx import MonoPlayer, ColourPlayer
from picofx.mono import FlickerFX
from picofx.colour import HSVFX

"""
Play a set of flickering thruster effects on a model spaceship, with an RGB light used for planetshine underglow.

Press "Boot" to exit the program.
"""

# Constants
MIN_LOCKOUT = 0.2                       # The minimum amount of time the thrusters will be at full brightness for (in seconds)
CENTRAL_BRIGHTNESS = 1.0                # The brightness of the central thrusters (from 0.0 to 1.0)
SIDE_BRIGHTNESS = 0.3                   # The brightness of the side thrusters (from 0.0 to 1.0)
DIMNESS_PERCENT = 0.1                   # How much to dim the thuster brightness by (from 0.0 to 1.0)

PLANETSHINE_HUE = 0.5                   # The colour of the planetshine (from 0.0 to 1.0)
PLANETSHINE_SATURATION = 0.5            # The saturation/intensity of the planetshine (from 0.0 to 1.0)
PLANETSHINE_VALUE = 1.0                 # The value/brightness of the planetshine (from 0.0 to 1.0)


# Variables
tiny = TinyFX()                         # Create a new TinyFX object
player = MonoPlayer(tiny.outputs)       # Create a new effect player to control TinyFX's mono outputs
rgb_player = ColourPlayer(tiny.rgb)     # Create a new effect player to control TinyFX's RGB output

# Set up the effects for the thruster banks
left_centre = FlickerFX(lockout_min=MIN_LOCKOUT,
                        brightness=CENTRAL_BRIGHTNESS,
                        dimness=DIMNESS_PERCENT)

right_centre = FlickerFX(lockout_min=MIN_LOCKOUT,
                         brightness=CENTRAL_BRIGHTNESS,
                         dimness=DIMNESS_PERCENT)

left = FlickerFX(lockout_min=MIN_LOCKOUT,
                 brightness=SIDE_BRIGHTNESS,
                 dimness=DIMNESS_PERCENT)

right = FlickerFX(lockout_min=MIN_LOCKOUT,
                  brightness=SIDE_BRIGHTNESS,
                  dimness=DIMNESS_PERCENT)

# Set up the mono effects to play
# Both left and right LEDs are close together on the ship, so it looks better if they share the same flicker
player.effects = [
    left,
    left,
    left_centre,
    right_centre,    
    right,
    right
]

# Set up the colour effect to play
rgb_player.effects = HSVFX(hue=PLANETSHINE_HUE,
                           sat=PLANETSHINE_SATURATION,
                           val=PLANETSHINE_VALUE)

# Have the RGB player run in sync with the Mono player
player.pair(rgb_player)


# Wrap the code in a try block, to catch any exceptions (including KeyboardInterrupt)
try:
    player.start()   # Start the effects running

    # Loop until the effect stops or the "Boot" button is pressed
    while player.is_running() and not tiny.boot_pressed():
        pass

# Stop any running effects and turn off all the outputs
finally:    
    player.stop()
    tiny.clear()
