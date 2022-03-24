import random
import time
import machine
import badger2040
from badger2040 import WIDTH, HEIGHT
import eliminationsquares

squares = bytearray(eliminationsquares.data())

d = badger2040.Badger2040()

# Easier names for reading button states.
btna = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
btnb = machine.Pin(badger2040.BUTTON_B, machine.Pin.IN, machine.Pin.PULL_DOWN)
btnc = machine.Pin(badger2040.BUTTON_C, machine.Pin.IN, machine.Pin.PULL_DOWN)
btnu = machine.Pin(badger2040.BUTTON_UP, machine.Pin.IN, machine.Pin.PULL_DOWN)
btnd = machine.Pin(badger2040.BUTTON_DOWN, machine.Pin.IN, machine.Pin.PULL_DOWN)
# Inverted. For reasons.
btnr = machine.Pin(badger2040.BUTTON_USER, machine.Pin.IN, machine.Pin.PULL_UP)

roundWinMsgs = ("Nice!",
                "Good job!",
                "You did the thing!",
                "Little faster now!",
                "Excellent!")
roundLoseMsgs = ("Wrong shape, buddy!",
                 "That wasn't it!",
                 "Not that one!",
                 "Oops! Try again!",
                 "Nope! >:C")

heartData = bytearray((
    0b00000000, 0b00000000,
    0b00011100, 0b00111000,
    0b00111110, 0b01111100,
    0b01111111, 0b11111110,
    0b01111111, 0b11111110,
    0b01111111, 0b11111110,
    0b01111111, 0b11111110,
    0b00111111, 0b11111100,
    0b00111111, 0b11111100,
    0b00011111, 0b11111000,
    0b00011111, 0b11111000,
    0b00001111, 0b11110000,
    0b00000111, 0b11100000,
    0b00000011, 0b11000000,
    0b00000001, 0b10000000,
    0b00000000, 0b00000000))

# Left lines for columns, left to right
columns = (30, 136, 239, 268)

# Top lines for rows, top to bottom
rows = (19, 71, 100)

# Which squares are active. 0 is A, 1 B, 2 C, 3 DOWN, 4 UP
# Each spot has two squares. The first entry in the list is the
# square that is closest to the edge of the screen.
activesquares = ([0, 0], [0, 0], [0, 0], [0, 0], [0, 0])

# What level are we on? Completing three rounds brings you to the next level.
level = 0
# Todo: Add a user button combo to cheat to a higher level
# 0 is easiest, 3 blocks
# 1, 5 blocks
# 2, 7 blocks
# 3, 10 blocks

# What round of this level is it? You play each level for three rounds before moving forward.
# Each round gets slightly faster, from 1 second down to 0.1
gmround = 1
roundtimes = (0, 1.0, 0.4, 0.1)

# How many chances are left? You can only get four wrong!
lives = 4

# The current pattern, stored as a string of numbers 0 to 9 that represent each square
# 0 a1, 1 b1, 2 c1, 3 down1, 4 up1, 5 a2, 6 b2, 7 ctri2, 8 downtri2, 9 up2
pattern = ""

# Are we currently listening for user input, should we start the next round
active = False
startround = True

# QuickRefs
# .text(Text, x, y, scale=1, rotation=0)
# .icon(data, index, sheetwidth, iconsizewh, x, y)
# .measure_text(text, scale)
# .image(data, width, height, x, y)
# .partial_update(x, y8, w, h8)


def eliminate_square(sq):  # used ONLY to update a square to eliminated during gameplay
    # sq is the square to update
    d.led(200)
    d.update_speed(badger2040.UPDATE_SUPER_EXTRA_TURBO)
    global activesquares
    if sq == 0:  # a1
        activesquares[0][0] = 0
        d.icon(squares, 1, 144, 24, columns[0], rows[2])
        d.partial_update(columns[0], 96, 24, 32)
    elif sq == 1:  # b1
        activesquares[1][0] = 0
        d.icon(squares, 1, 144, 24, columns[1], rows[2])
        d.partial_update(columns[1], 96, 24, 32)
    elif sq == 2:  # c1
        activesquares[2][0] = 0
        d.icon(squares, 1, 144, 24, columns[2], rows[2])
        d.partial_update(columns[2], 96, 24, 32)
    elif sq == 3:  # down1
        activesquares[3][0] = 0
        d.icon(squares, 1, 144, 24, columns[3], rows[1])
        d.partial_update(columns[3], 72, 24, 32)
    elif sq == 4:  # up1
        activesquares[4][0] = 0
        d.icon(squares, 1, 144, 24, columns[3], rows[0])
        d.partial_update(columns[3], 16, 24, 32)
    elif sq == 5:  # a2
        activesquares[0][1] = 0
        d.icon(squares, 1, 144, 24, columns[0], rows[1])
        d.partial_update(columns[0], 72, 24, 24)
    elif sq == 6:  # b2
        activesquares[1][1] = 0
        d.icon(squares, 1, 144, 24, columns[1], rows[1])
        d.partial_update(columns[1], 72, 24, 24)
    elif sq == 9:  # up2
        activesquares[4][1] = 0
        d.icon(squares, 1, 144, 24, columns[2], rows[0])
        d.partial_update(columns[2], 16, 24, 32)
    elif sq == 7:  # ctri2
        activesquares[2][1] = 0
        if activesquares[3][1] == 1:
            d.icon(squares, 4, 144, 24, columns[2], rows[1])
        else:
            d.icon(squares, 5, 144, 24, columns[2], rows[1])
        d.partial_update(columns[2], 72, 24, 24)
    elif sq == 8:  # downtri2
        activesquares[3][1] = 0
        if activesquares[2][1] == 1:
            d.icon(squares, 3, 144, 24, columns[2], rows[1])
        else:
            d.icon(squares, 5, 144, 24, columns[2], rows[1])
        d.partial_update(columns[2], 72, 24, 24)
    d.led(0)
    pass


def activate_square(sq):  # used ONLY to show a new square during gameplay
    # sq is the square to update
    d.update_speed(badger2040.UPDATE_SUPER_EXTRA_TURBO)
    global activesquares
    if sq == 0:  # a1
        activesquares[0][0] = 1
        d.icon(squares, 0, 144, 24, columns[0], rows[2])
        d.partial_update(columns[0], 96, 24, 32)
    elif sq == 1:  # b1
        activesquares[1][0] = 1
        d.icon(squares, 0, 144, 24, columns[1], rows[2])
        d.partial_update(columns[1], 96, 24, 32)
    elif sq == 2:  # c1
        activesquares[2][0] = 1
        d.icon(squares, 0, 144, 24, columns[2], rows[2])
        d.partial_update(columns[2], 96, 24, 32)
    elif sq == 3:  # down1
        activesquares[3][0] = 1
        d.icon(squares, 0, 144, 24, columns[3], rows[1])
        d.partial_update(columns[3], 72, 24, 32)
    elif sq == 4:  # up1
        activesquares[4][0] = 1
        d.icon(squares, 0, 144, 24, columns[3], rows[0])
        d.partial_update(columns[3], 16, 24, 32)
    elif sq == 5:  # a2
        activesquares[0][1] = 1
        d.icon(squares, 0, 144, 24, columns[0], rows[1])
        d.partial_update(columns[0], 72, 24, 24)
    elif sq == 6:  # b2
        activesquares[1][1] = 1
        d.icon(squares, 0, 144, 24, columns[1], rows[1])
        d.partial_update(columns[1], 72, 24, 24)
    elif sq == 9:  # up2
        activesquares[4][1] = 1
        d.icon(squares, 0, 144, 24, columns[2], rows[0])
        d.partial_update(columns[2], 16, 24, 32)
    elif sq == 7:  # ctri2
        activesquares[2][1] = 1
        if activesquares[3][1] == 1:
            d.icon(squares, 2, 144, 24, columns[2], rows[1])
        else:
            d.icon(squares, 3, 144, 24, columns[2], rows[1])
        d.partial_update(columns[2], 72, 24, 24)
    elif sq == 8:  # downtri2
        activesquares[3][1] = 1
        if activesquares[2][1] == 1:
            d.icon(squares, 2, 144, 24, columns[2], rows[1])
        else:
            d.icon(squares, 4, 144, 24, columns[2], rows[1])
        d.partial_update(columns[2], 72, 24, 24)
    pass


def countdown():  # Counts down from 3 to WATCH, then triggers whatever function is next to show the pattern
    d.update_speed(badger2040.UPDATE_SUPER_EXTRA_TURBO)
    d.pen(0)
    d.thickness(3)
    d.font("sans")
    d.text("3..", 10, 55, 0.7)
    d.partial_update(10, 40, 30, 24)  # Update 3 to dark
    time.sleep(roundtimes[gmround])
    d.text("2..", 40, 55, 0.7)
    d.pen(8)
    d.text("3..", 10, 55, 0.7)
    d.partial_update(10, 40, 60, 24)  # Update 3 to light, 2 to dark
    time.sleep(roundtimes[gmround])
    d.text("2..", 40, 55, 0.7)
    d.pen(0)
    d.text("1..", 70, 55, 0.7)
    d.partial_update(40, 40, 60, 24)  # Update 2 to light, 1 to dark
    time.sleep(roundtimes[gmround])
    d.text("WATCH", 105, 55, 0.7)
    d.pen(8)
    d.text("1..", 70, 55, 0.7)
    d.partial_update(40, 40, 133, 24)  # Update 1 to light, WATCH to dark
    time.sleep(roundtimes[gmround])
    generate_pattern()  # Countdown complete. Time to watch the squares
    pass


def generate_pattern():  # Generates the pattern to be remembered, and activates those blocks on screen as it does so
    global pattern, activesquares, active
    activesquares = ([0, 0], [0, 0], [0, 0], [0, 0], [0, 0])
    pattern = ""
    if level == 0:
        total = 3
    elif level == 1:
        total = 5
    elif level == 2:
        total = 7
    elif level == 3:
        total = 10
    for i in range(0, total):
        while True:
            new = random.randint(0, 4)
            if activesquares[new][0] == 1 and level > 1:
                new = new + 5
            newstr = str(new)
            print("Candidate Number: " + newstr)
            if newstr not in pattern:
                print("Number accepted.")
                global pattern
                pattern = pattern + newstr
                activate_square(new)
                # print(activesquares)
                time.sleep(roundtimes[gmround])
                break
            else:
                print("Same number, trying again.")
                continue
    print("Pattern: " + pattern)
    time.sleep(roundtimes[gmround])
    d.pen(8)
    d.text("WATCH", 105, 55, 0.7)
    d.pen(0)
    d.text("GO!", 185, 55, 0.7)
    d.partial_update(105, 40, 115, 24)  # Update WATCH to light, GO! to dark
    active = True
    pass


def show_result(result):
    d.update_speed(badger2040.UPDATE_SUPER_EXTRA_TURBO)
    d.pen(15)  # White pen for the outline
    d.rectangle(0, 0, WIDTH, HEIGHT)
    d.thickness(2)
    d.pen(0)
    d.text(result, int(WIDTH / 2) - int(d.measure_text(result, 0.5) / 2), int(HEIGHT / 2), 0.5)
    d.update()
    pass


def drawstaticui():
    d.update_speed(badger2040.UPDATE_FAST)
    d.led(255)
    d.pen(15)
    d.rectangle(0, 0, WIDTH, HEIGHT)
    d.pen(0)  # Black pen
    d.thickness(2)  # 2px line
    d.font("sans")
    d.text("eliminate the shapes in", 10, 10, 0.6)
    d.text("the order they appear!", 10, 32, 0.6)
    d.thickness(2)  # 1px line for the diagonal bar for corner squares
    d.line(231, 63, WIDTH, HEIGHT)  # Said bar 231 63 to bottom right corner

    # Level Text
    d.font("serif")
    d.thickness(3)
    levelmid = int((columns[1] + columns[0]) / 2) + 12
    lvlmid = int(levelmid - d.measure_text("LVL", 0.8) / 2)
    lvltmid = int(levelmid - d.measure_text(str(level), 0.8) / 2)
    d.text("LVL", lvlmid, 88, 0.8)  # 66, 84
    d.text(str(level), lvltmid, 110, 0.8)

    # Draws the hearts/lives/chances
    d.led(220)
    d.thickness(2)
    d.font("serif")
    d.text("LIVES", 177, 80, 0.5)
    for i in range(1, lives + 1):
        d.image(heartData, 16, 16, 231 - i * 16, 86)
        pass

    # Round Text
    levelmid = int((columns[2] + columns[1]) / 2) + 12
    lvlmid = int(levelmid - d.measure_text("ROUND", 0.5) / 2)
    lvltmid = int(levelmid - d.measure_text(str(gmround), 1) / 2)
    d.text("ROUND", lvlmid, 110, 0.5)  # 66, 84
    d.text(str(gmround), lvltmid, 123, 0.5)

    # Initial Countdown Text
    d.font("sans")
    d.thickness(3)
    d.pen(8)
    d.text("3..", 10, 55, 0.7)
    d.text("2..", 40, 55, 0.7)
    d.text("1..", 70, 55, 0.7)
    d.text("WATCH", 105, 55, 0.7)
    d.text("GO!", 185, 55, 0.7)

    d.update()
    d.led(0)
    pass


def nextround(wl):  # Just a quick function to set us up for the next round. (or declare a total win)
    global level, gmround, lives, active, startround
    active = False
    if wl == "win":  # We won the round!
        if gmround == 3:  # If they are on the last round of the level
            if level == 3:  # and it was the last level
                show_result("You won remembering! Bye!")
                time.sleep(3)
                machine.reset()  # replace with another prompt?
            else:  # This is not the last round of level 3
                show_result("Level up!")
                time.sleep(3)
                level = level + 1
                gmround = 1
        else:  # This is not round 3
            show_result(random.choice(roundWinMsgs))
            time.sleep(3)
            gmround = gmround + 1
    else:  # We lost the round
        if lives == 1:  # This was our last life
            show_result("You died.")
            time.sleep(3)
            machine.reset()  # replace with another prompt?
        else:  # This was not our last life
            show_result(random.choice(roundLoseMsgs))
            lives = lives - 1
            time.sleep(3)
    startround = True
    pass


# Main loop
while True:
    if startround:
        startround = False
        drawstaticui()
        countdown()
    if active:
        if btna.value():
            if activesquares[0][1] == 1 and pattern[0] == "5":  # If the second A square is active and is the next in the pattern
                pattern = pattern.strip(pattern[0])
                eliminate_square(5)
                if pattern == "":  # That was the last block and we have no more
                    nextround("win")
            elif activesquares[0][0] == 1 and pattern[0] == "0":  # If the first A square is active and is the next in the pattern
                pattern = pattern.strip(pattern[0])
                eliminate_square(0)
                if pattern == "":  # That was the last block and we have no more
                    nextround("win")
            else:  # Neither of the above was true, so you lose the round
                nextround("lose")
        if btnb.value():
            if activesquares[1][1] == 1 and pattern[0] == "6":  # Second B
                pattern = pattern.strip(pattern[0])
                eliminate_square(6)
                if pattern == "":  # That was the last block and we have no more
                    nextround("win")
            elif activesquares[1][0] == 1 and pattern[0] == "1":  # First B
                pattern = pattern.strip(pattern[0])
                eliminate_square(1)
                if pattern == "":  # That was the last block and we have no more
                    nextround("win")
            else:  # Neither of the above was true, so you lose the round
                nextround("lose")
        if btnc.value():
            if activesquares[2][1] == 1 and pattern[0] == "7":  # Second C
                pattern = pattern.strip(pattern[0])
                eliminate_square(7)
                if pattern == "":  # That was the last block and we have no more
                    nextround("win")
            elif activesquares[2][0] == 1 and pattern[0] == "2":  # First C
                pattern = pattern.strip(pattern[0])
                eliminate_square(2)
                if pattern == "":  # That was the last block and we have no more
                    nextround("win")
            else:  # Neither of the above was true, so you lose the round
                nextround("lose")
        if btnd.value():
            if activesquares[3][1] == 1 and pattern[0] == "8":  # Second Down
                pattern = pattern.strip(pattern[0])
                eliminate_square(8)
                if pattern == "":  # That was the last block and we have no more
                    nextround("win")
            elif activesquares[3][0] == 1 and pattern[0] == "3":  # First Down, wait is this football?
                pattern = pattern.strip(pattern[0])
                eliminate_square(3)
                if pattern == "":  # That was the last block and we have no more
                    nextround("win")
            else:  # Neither of the above was true, so you lose the round
                nextround("lose")
        if btnu.value():
            if activesquares[4][1] == 1 and pattern[0] == "9":  # Second Up
                pattern = pattern.strip(pattern[0])
                eliminate_square(9)
                if pattern == "":  # That was the last block and we have no more
                    nextround("win")
            elif activesquares[4][0] == 1 and pattern[0] == "4":  # First Up
                pattern = pattern.strip(pattern[0])
                eliminate_square(4)
                if pattern == "":  # That was the last block and we have no more
                    nextround("win")
            else:  # Neither of the above was true, so you lose the round
                nextround("lose")
        pass
    time.sleep(0.01)
    pass
