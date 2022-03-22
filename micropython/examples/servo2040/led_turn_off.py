from plasma import WS2812
from servo import servo2040

# Create the LED bar, using PIO 1 and State Machine 0
led_bar = WS2812(servo2040.NUM_LEDS, 1, 0, servo2040.LED_DAT)

# Start updating the LED bar
led_bar.start()
