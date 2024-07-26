from tiny_fx import TinyFX
from picofx import MonoPlayer
from picofx.mono import WaveSequenceFX

"""
Play a wave sequence on TinyFX's outputs.

Press "Boot" to exit the program.
"""

# Variables
tiny = TinyFX()                        # Create a new TinyFX object to interact with the board
player = MonoPlayer(tiny.outputs)      # Create a new effect player to control TinyFX's mono outputs
    
# Effects
wave = WaveSequenceFX(1, 6)            # Create a WaveSequenceFX effect


# Set up the effects to play
player.effects = [
    wave(0),
    wave(1),
    wave(2),
    wave(3),
    wave(4),
    wave(5)
]


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
