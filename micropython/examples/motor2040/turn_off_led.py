from plasma import WS2812
from motor import motor2040

"""
A simple program that turns off the onboard LED.

NOTE: Plasma WS2812 uses the RP2040's PIO system, and as
such may have problems when running code multiple times.
If you encounter issues, try resetting your board.
"""

# Create the LED, using PIO 1 and State Machine 0
led = WS2812(motor2040.NUM_LEDS, 1, 0, motor2040.LED_DATA)

# Start updating the LED
led.start()
