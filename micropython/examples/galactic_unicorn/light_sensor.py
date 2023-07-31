import time
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN as DISPLAY
import math

'''
Auto brightness feature for the galactic unicorn
Uses the onboard light sensor to detect the light
The brightness percentage is displayed with brightness auto adjusted
'''
# set up unicorn and drawing variables
gu = GalacticUnicorn()
graphics = PicoGraphics(DISPLAY)

WIDTH, HEIGHT = graphics.get_bounds()
BLACK = graphics.create_pen(0, 0, 0)
WHITE = graphics.create_pen(255, 255, 255)
GREY = graphics.create_pen(20, 20, 20)
HUE_START = 0
HUE_END = 100
graphics.set_font("bitmap8")

# Text display sleep time in ms
TEXT_SLEEP = 500


# the onboard light sensor has a wide range from 0 t0 4095
# It is therefore needed to set a lower max and a higher minimum
MIN_LS_VALUE = 10
MAX_LS_VALUE = 295 # 4095 to use the full range
MIN_RANGE = 0.1
MAX_RANGE = 1


# perform linear interpolation to map a range of values to discrete
def map_range(x, min_input = MIN_LS_VALUE,
                  max_input = MAX_LS_VALUE,
                  min_output = MIN_RANGE,
                  max_output = MAX_RANGE):
    return (x - min_input) * (max_output - min_output) / (max_input - min_input) + min_output
    

# gets the light sensor value from onboard sensor and interpolates it
# clamps the brightness values
def calculate_brightness(current_lsv):
    brightness_value = map_range(current_lsv)
    if brightness_value > 1:
        brightness_value = 1
    elif brightness_value < 0.1:
        brightness_value = 0.1
        
    return brightness_value


# sets up a handy function we can call to clear the screen
def clear():
    graphics.set_pen(BLACK)
    graphics.clear()    

mode = "auto"
last = time.ticks_ms()
while True:
    current = time.ticks_ms()
    
    
    
    # get light sensor value from the sensor
    ls_value = gu.light()
    brightness_value = calculate_brightness(ls_value)
    gu.set_brightness(brightness_value)
    # calculate brightness percentage
    bp = (brightness_value / MAX_RANGE) * 100
    
    # deactivate auto brightness by pressing A
    if gu.is_pressed(GalacticUnicorn.SWITCH_A):
        print("Auto brightness off")
        mode = "off"
        
    # reactivate auto brightness by pressing A
    if gu.is_pressed(GalacticUnicorn.SWITCH_B):
        print("Auto brightness on")
        mode = "auto"
        
    # set brightness to default value if off
    if mode == "off":
        gu.set_brightness(0.5)
    
    # set text update rate after a certain time to reduce flickering 
    if current - last >= TEXT_SLEEP:
        clear()
        
        # calculate colour from the brightness value
        hue = max(0, HUE_START + ((bp - 0) * (HUE_END - HUE_START) / (100 - 0)))
        
        # create pens with this colour (and with the high / low colours)
        CURRENT_COLOUR = graphics.create_pen_hsv(hue / 360, 1.0, 0.8)
        HIGH_COLOUR = graphics.create_pen_hsv(HUE_END / 360, 1.0, 0.8)
        LOW_COLOUR = graphics.create_pen_hsv(HUE_START / 360, 1.0, 0.8)

        # draw the text
        graphics.set_pen(CURRENT_COLOUR)
        graphics.text("BRT: ", 0, 1, scale=1)
        # measure the rest of the text before drawing so that we can right align it
        text_width = graphics.measure_text(f"{bp:.0f}/°", scale=1)
        graphics.text(f"{bp:.0f}%", WIDTH - text_width, 1, scale=1)
        
        # draw a bar for the background
        graphics.set_pen(GREY)
        graphics.rectangle(0, 9, WIDTH, 10)

        # draw a bar for the current brightness percentage
        graphics.set_pen(CURRENT_COLOUR)
        graphics.rectangle(0, 9, int((bp / 100) * WIDTH), 10)
        last = current

    # time to update the display
    gu.update(graphics)
    # time.sleep(1)


