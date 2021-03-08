# This example takes the temperature from the Pico's onboard temperature sensor, and displays it on Pico Display Pack, along with a little pixelly graph.
# It's based on the thermometer example in the "Getting Started with MicroPython on the Raspberry Pi Pico" book, which is a great read if you're a beginner!

import machine 
import utime 

# Pico Display boilerplate
import picodisplay as display
width = display.get_width()
height = display.get_height()
display_buffer = bytearray(width * height * 2)
display.init(display_buffer)

# reads from Pico's temp sensor and converts it into a more manageable number
sensor_temp = machine.ADC(4) 
conversion_factor = 3.3 / (65535) 

# Set the display backlight to 50%
display.set_backlight(0.5)

temperatures = []

while True:
    # fills the screen with black
    display.set_pen(0, 0, 0)
    display.clear()

    # the following two lines do some maths to convert the number from the temp sensor into celsius
    reading = sensor_temp.read_u16() * conversion_factor
    temperature = round(27 - (reading - 0.706) / 0.001721)    
    
    # shifts the temperatures history to the left by one sample
    if len(temperatures) > 48:  # 240/5
        temperatures = temperatures[1::]
    else:
        temperatures.append(temperature)

    i = 0
    for t in temperatures:
        # chooses a pen colour based on the temperature
        display.set_pen(0, 255, 0)
        if t > 20:
            display.set_pen(255, 0, 0)
        elif t < 13:
            display.set_pen(0, 0, 255)
    
        # draws the reading as a tall, thin rectangle
        display.rectangle(i, height - (t * 4), 5, height)

        # the next tall thin rectangle needs to be drawn 5 pixels to the right of the last one
        i += 5
    
    # heck lets also set the LED to match
    display.set_led(0, 255, 0)
    if temperature > 20:
        display.set_led(255, 0, 0)
    elif temperature < 13:
        display.set_led(0, 0, 255)

    # draws a white background for the text
    display.set_pen(255, 255, 255)
    display.rectangle(1, 1, 50, 25)
    
    # writes the reading as text in the white rectangle
    display.set_pen(0, 0, 0)
    display.text("{:.0f}".format(temperature) + "c", 3, 3, 0, 3)  
    
    # time to update the display
    display.update()
    
    # waits for 5 seconds
    utime.sleep(5)
