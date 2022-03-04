import badger2040
import machine
import time
import gc

try:
    open("witw.txt", "r")
except OSError:
    try:
        import witw
        open("witw.txt", "wb").write(witw.data())
        del witw
    except ImportError:
        pass


gc.collect()


# **** Put the name of your text file here *****
text_file = "witw.txt"  # File must be on the MicroPython device

# Global Constants
WIDTH = badger2040.WIDTH
HEIGHT = badger2040.HEIGHT

ARROW_THICKNESS = 3
ARROW_WIDTH = 18
ARROW_HEIGHT = 14
ARROW_PADDING = 2

TEXT_PADDING = 4
TEXT_SPACING = 20
TEXT_SIZE = 0.5
TEXT_WIDTH = WIDTH - TEXT_PADDING - TEXT_PADDING - ARROW_WIDTH


# ------------------------------
#      Drawing functions
# ------------------------------

# Draw a upward arrow
def draw_up(x, y, width, height, thickness, padding):
    border = (thickness // 4) + padding
    display.line(x + border, y + height - border,
                 x + (width // 2), y + border)
    display.line(x + (width // 2), y + border,
                 x + width - border, y + height - border)


# Draw a downward arrow
def draw_down(x, y, width, height, thickness, padding):
    border = (thickness // 2) + padding
    display.line(x + border, y + border,
                 x + (width // 2), y + height - border)
    display.line(x + (width // 2), y + height - border,
                 x + width - border, y + border)


# Draw the frame of the reader
def draw_frame():
    display.pen(15)
    display.clear()
    display.pen(12)
    display.rectangle(WIDTH - ARROW_WIDTH, 0, ARROW_WIDTH, HEIGHT)
    display.pen(0)
    display.thickness(ARROW_THICKNESS)
    if current_page > 1:
        draw_up(WIDTH - ARROW_WIDTH, (HEIGHT // 4) - (ARROW_HEIGHT // 2),
                ARROW_WIDTH, ARROW_HEIGHT, ARROW_THICKNESS, ARROW_PADDING)
    draw_down(WIDTH - ARROW_WIDTH, ((HEIGHT * 3) // 4) - (ARROW_HEIGHT // 2),
              ARROW_WIDTH, ARROW_HEIGHT, ARROW_THICKNESS, ARROW_PADDING)


# ------------------------------
#        Program setup
# ------------------------------

# Global variables
next_page = True
prev_page = False
last_offset = 0
current_page = 0

# Create a new Badger and set it to update FAST
display = badger2040.Badger2040()
display.update_speed(badger2040.UPDATE_FAST)

# Set up the buttons
button_down = machine.Pin(badger2040.BUTTON_DOWN, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_up = machine.Pin(badger2040.BUTTON_UP, machine.Pin.IN, machine.Pin.PULL_DOWN)

# Set up the activity LED
led = machine.Pin(badger2040.PIN_LED, machine.Pin.OUT)

offsets = []


# Button handling function
def button(pin):
    global next_page, prev_page

    if pin == button_down:
        next_page = True

    if pin == button_up:
        prev_page = True


# Register the button handling function with the buttons
button_down.irq(trigger=machine.Pin.IRQ_RISING, handler=button)
button_up.irq(trigger=machine.Pin.IRQ_RISING, handler=button)


# ------------------------------
#         Render page
# ------------------------------

def render_page():
    row = 0
    line = ""
    pos = ebook.tell()
    next_pos = pos
    add_newline = False

    while True:
        # Read a full line and split it into words
        words = ebook.readline().split(" ")

        # Take the length of the first word and advance our position
        next_word = words[0]
        if len(words) > 1:
            next_pos += len(next_word) + 1
        else:
            next_pos += len(next_word)  # This is the last word on the line

        # Advance our position further if the word contains special characters
        if '\u201c' in next_word:
            next_word = next_word.replace('\u201c', '\"')
            next_pos += 2
        if '\u201d' in next_word:
            next_word = next_word.replace('\u201d', '\"')
            next_pos += 2
        if '\u2019' in next_word:
            next_word = next_word.replace('\u2019', '\'')
            next_pos += 2

        # Rewind the file back from the line end to the start of the next word
        ebook.seek(next_pos)

        # Strip out any new line characters from the word
        next_word = next_word.strip()

        # If an empty word is encountered assume that means there was a blank line
        if len(next_word) == 0:
            add_newline = True

        # Append the word to the current line and measure its length
        appended_line = line
        if len(line) > 0 and len(next_word) > 0:
            appended_line += " "
        appended_line += next_word
        appended_length = display.measure_text(appended_line, TEXT_SIZE)

        # Would this appended line be longer than the text display area, or was a blank line spotted?
        if appended_length >= TEXT_WIDTH or add_newline:

            # Yes, so write out the line prior to the append
            print(line)
            display.pen(0)
            display.thickness(2)
            display.text(line, TEXT_PADDING, (row * TEXT_SPACING) + (TEXT_SPACING // 2) + TEXT_PADDING, TEXT_SIZE)

            # Clear the line and move on to the next row
            line = ""
            row += 1

            # Have we reached the end of the page?
            if (row * TEXT_SPACING) + (TEXT_SPACING // 2) > HEIGHT:
                print("+++++")
                display.update()

                # Reset the position to the start of the word that made this line too long
                ebook.seek(pos)
                return
            else:
                # Set the line to the word and advance the current position
                line = next_word
                pos = next_pos

            # A new line was spotted, so advance a row
            if add_newline:
                print("")
                row += 1
                if (row * TEXT_SPACING) + (TEXT_SPACING // 2) > HEIGHT:
                    print("+++++")
                    display.update()
                    return
                add_newline = False
        else:
            # The appended line was not too long, so set it as the line and advance the current position
            line = appended_line
            pos = next_pos


# ------------------------------
#       Main program loop
# ------------------------------

# Open the book file
ebook = open(text_file, "r")

while True:
    # Was the next page button pressed?
    if next_page:
        current_page += 1

        # Is the next page one we've not displayed before?
        if current_page > len(offsets):
            offsets.append(ebook.tell())  # Add its start position to the offsets list
        draw_frame()
        render_page()
        next_page = False  # Clear the next page button flag

    # Was the previous page button pressed?
    if prev_page:
        if current_page > 1:
            current_page -= 1
            ebook.seek(offsets[current_page - 1])  # Retrieve the start position of the last page
            draw_frame()
            render_page()
        prev_page = False  # Clear the prev page button flag

    time.sleep(0.1)
