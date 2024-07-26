from tiny_fx import TinyFX
from picofx import MonoPlayer, ColourPlayer
from picofx.mono import PelicanLightFX
from picofx.colour import RainbowFX

"""
Play a traffic light sequence on TinyFX's outputs.

Press "Boot" to exit the program.
"""

# Variables
tiny = TinyFX()                        # Create a new TinyFX object to interact with the board
player = MonoPlayer(tiny.outputs)      # Create a new effect player to control TinyFX's mono outputs
rgb_player = ColourPlayer(tiny.rgb)    # Create a new effect player to control TinyFX's RGB output
    
# Effects
traffic = PelicanLightFX(1)            # Create a PelicanLight effect


# Set up the effects to play
player.effects = [
    traffic('red'),
    traffic('amber'),
    traffic('green')
]
rgb_player.effects = RainbowFX(0.1)

# Pair the RGB player with the Mono player so they run in sync
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
