import time
from pimoroni import Button
from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY
display = PicoGraphics(display=DISPLAY_PICO_DISPLAY, rotate=0)
display.set_backlight(1.0)

button_a = Button(12)
button_b = Button(13)
button_x = Button(14)
button_y = Button(15)

WIDTH, HEIGHT = display.get_bounds()
WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)


def clear():
    display.set_pen(BLACK)
    display.clear()
    display.update()


clear()


lastAdjustment = None
currentTimeHours = 12
currentTimeMinutes = 0


def incrementHours():
    global currentTimeHours
    currentTimeHours += 1
    if currentTimeHours > 12:
        currentTimeHours = 1


def decrementHours():
    global currentTimeHours
    currentTimeHours -= 1
    if currentTimeHours < 1:
        currentTimeHours = 12


def incrementMinutes():
    global currentTimeMinutes
    currentTimeMinutes += 1
    if currentTimeMinutes > 59:
        currentTimeMinutes = 0


def decrementMinutes():
    global currentTimeMinutes
    currentTimeMinutes -= 1
    if currentTimeMinutes < 1:
        currentTimeMinutes = 59


def showTime():
    clear()
    display.set_pen(WHITE)
    display.text(
        "{:02}:{:02}".format(
            currentTimeHours,
            currentTimeMinutes), int(WIDTH/3.5), int(HEIGHT/3), 240, 4)
    display.update()


while True:
    showTime()
    if button_a.read():  # Handle hours adjustment
        incrementHours()
    elif button_b.read():
        decrementHours()
    elif button_x.read():  # Handle minutes adjustment
        incrementMinutes()
    elif button_y.read():
        decrementMinutes()
    if lastAdjustment is None or time.time()-lastAdjustment >= 59:
        if currentTimeMinutes != 59:
            incrementMinutes()
        else:
            incrementMinutes()
            incrementHours()
        lastAdjustment = time.time()
