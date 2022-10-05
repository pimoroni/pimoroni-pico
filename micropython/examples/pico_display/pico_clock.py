import time
from pimoroni import Button
from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY, PEN_P4

# We're only using a few colours so we can use a 4 bit/16 colour palette and save RAM!
display = PicoGraphics(display=DISPLAY_PICO_DISPLAY, rotate=0)
display.set_backlight(1.0)


button_a = Button(12)
button_b = Button(13)
button_x = Button(14)
button_y = Button(15)

WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)

def clear():
    display.set_pen(BLACK)
    display.clear()
    display.update()

clear()

WIDTH, HEIGHT = display.get_bounds()

currentTimeHours = "12"
currentTimeMinutes = "00"

lastAdjustment = None

def showTime():
    clear()
    display.set_pen(WHITE)
    display.text(str(currentTimeHours)+":"+str(currentTimeMinutes), int(WIDTH/3.5), int(HEIGHT/3), 240, 4)
    display.update()

while True:
    showTime()
    
    if button_a.read(): # Handle hours adjustment
        currentTimeHours = str(int(currentTimeHours)+1)
        if int(currentTimeHours) < 10:
            currentTimeHours = "0"+currentTimeHours
        elif int(currentTimeHours) > 12:
            currentTimeHours = "01"
    elif button_b.read():
        currentTimeHours = str(int(currentTimeHours)-1)
        if int(currentTimeHours) < 1:
            currentTimeHours = "12"
        if int(currentTimeHours) < 10:
            currentTimeHours = "0"+currentTimeHours
    elif button_x.read(): # Handle minutes adjustment
        currentTimeMinutes = str(int(currentTimeMinutes)+1)
        if int(currentTimeMinutes) < 10:
            currentTimeMinutes = "0"+currentTimeMinutes
        elif int(currentTimeMinutes) > 59:
            currentTimeMinutes = "01"
    elif button_y.read():
        currentTimeMinutes = str(int(currentTimeMinutes)-1)
        if int(currentTimeMinutes) < 1:
            currentTimeMinutes = "59"
        if int(currentTimeMinutes) < 10:
            currentTimeMinutes = "0"+currentTimeMinutes
    
    if lastAdjustment is None or time.time()-lastAdjustment >= 59:
        if currentTimeMinutes != "59":
            currentTimeMinutes = str(int(currentTimeMinutes)+1)
            if int(currentTimeMinutes) < 10:
                currentTimeMinutes = "0"+currentTimeMinutes
        else:
            currentTimeMinutes = "00"
            if currentTimeHours == "12":
                currentTimeHours = "01"
            else:
                currentTimeHours = str(int(currentTimeHours)+1)
                if int(currentTimeHours) < 10:
                    currentTimeHours = "0"+currentTimeHours
        lastAdjustment = time.time()