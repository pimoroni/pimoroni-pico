# This example takes the temperature from the Pico's onboard temperature sensor, and displays it on Pico Explorer, along with a little pixelly graph.
# It's based on the thermometer example in the "Getting Started with MicroPython on the Raspberry Pi Pico" book, which is a great read if you're a beginner!

import machine 
import utime 

# Pico Explorer boilerplate
import picoexplorer as display
width = display.get_width()
height = display.get_height()
display_buffer = bytearray(width * height * 2)
display.init(display_buffer)

# reads from Pico's temp sensor and converts it into a more manageable number
sensor_temp = machine.ADC(4) 
conversion_factor = 3.3 / (65535) 

i = 0

while True:
    # the following two lines do some maths to convert the number from the temp sensor into celsius
    reading = sensor_temp.read_u16() * conversion_factor
    temperature = round(27 - (reading - 0.706) / 0.001721)    
    
    # this if statement clears the display once the graph reaches the right hand side of the display
    if i >= (width + 1):
        i = 0
        display.set_pen(0, 0, 0)
        display.clear()

    # chooses a pen colour based on the temperature
    display.set_pen(0, 255, 0)
    if temperature > 20:
        display.set_pen(255, 0, 0)
    if temperature < 13:
        display.set_pen(0, 0, 255)
           
    # draws the reading as a tall, thin rectangle
    display.rectangle(i, height - (temperature * 6), 6, height)
    
    # draws a white background for the text
    display.set_pen(255, 255, 255)
    display.rectangle(1, 1, 65, 33)
    
    # writes the reading as text in the white rectangle
    display.set_pen(0, 0, 0)
    display.text("{:.0f}".format(temperature) + "c", 3, 3, 0, 4)  
    
    # time to update the display
    display.update()
    
    # waits for 5 seconds
    utime.sleep(5) 
    
    # the next tall thin rectangle needs to be drawn 6 pixels to the right of the last one
    i += 6 
