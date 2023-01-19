import badger2040w
import gc
import badger_os

# **** Put the name of your text file here *****
text_file = "/books/289-0-wind-in-the-willows-abridged.txt"  # File must be on the MicroPython device

gc.collect()

# Global Constants
WIDTH = badger2040w.WIDTH
HEIGHT = badger2040w.HEIGHT

ARROW_THICKNESS = 3
ARROW_WIDTH = 18
ARROW_HEIGHT = 14
ARROW_PADDING = 2

TEXT_PADDING = 4
TEXT_WIDTH = WIDTH - TEXT_PADDING - TEXT_PADDING - ARROW_WIDTH

FONTS = ["sans", "gothic", "cursive", "serif"]
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
    display.set_pen(15)
    display.clear()
    display.set_pen(12)
    display.rectangle(WIDTH - ARROW_WIDTH, 0, ARROW_WIDTH, HEIGHT)
    display.set_pen(0)
    if state["current_page"] > 0:
        draw_up(WIDTH - ARROW_WIDTH, (HEIGHT // 4) - (ARROW_HEIGHT // 2),
                ARROW_WIDTH, ARROW_HEIGHT, ARROW_THICKNESS, ARROW_PADDING)
    draw_down(WIDTH - ARROW_WIDTH, ((HEIGHT * 3) // 4) - (ARROW_HEIGHT // 2),
              ARROW_WIDTH, ARROW_HEIGHT, ARROW_THICKNESS, ARROW_PADDING)


# ------------------------------
#        Program setup
# ------------------------------

# Global variables
state = {
    "last_offset": 0,
    "current_page": 0,
    "font_idx": 0,
    "text_size": 0.5,
    "offsets": []
}
badger_os.state_load("ebook", state)

text_spacing = int(34 * state["text_size"])


# Create a new Badger and set it to update FAST
display = badger2040w.Badger2040W()
display.led(128)
display.set_update_speed(badger2040w.UPDATE_FAST)


# ------------------------------
#         Render page
# ------------------------------

def render_page():
    row = 0
    line = ""
    pos = ebook.tell()
    next_pos = pos
    add_newline = False
    display.set_font(FONTS[state["font_idx"]])

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
        appended_length = display.measure_text(appended_line, state["text_size"])

        # Would this appended line be longer than the text display area, or was a blank line spotted?
        if appended_length >= TEXT_WIDTH or add_newline:

            # Yes, so write out the line prior to the append
            print(line)
            display.set_pen(0)
            display.text(line, TEXT_PADDING, (row * text_spacing) + (text_spacing // 2) + TEXT_PADDING, WIDTH, state["text_size"])

            # Clear the line and move on to the next row
            line = ""
            row += 1

            # Have we reached the end of the page?
            if (row * text_spacing) + text_spacing >= HEIGHT:
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
                if (row * text_spacing) + text_spacing >= HEIGHT:
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

launch = True
changed = False

# Open the book file
ebook = open(text_file, "r")
if len(state["offsets"]) > state["current_page"]:
    ebook.seek(state["offsets"][state["current_page"]])
else:
    state["current_page"] = 0
    state["offsets"] = []

while True:
    # Was the next page button pressed?
    if display.pressed(badger2040w.BUTTON_DOWN):
        state["current_page"] += 1

        changed = True

    # Was the previous page button pressed?
    if display.pressed(badger2040w.BUTTON_UP):
        if state["current_page"] > 0:
            state["current_page"] -= 1
            if state["current_page"] == 0:
                ebook.seek(0)
            else:
                ebook.seek(state["offsets"][state["current_page"] - 1])  # Retrieve the start position of the last page
            changed = True

    if display.pressed(badger2040w.BUTTON_A):
        state["text_size"] += 0.1
        if state["text_size"] > 0.8:
            state["text_size"] = 0.5
        text_spacing = int(34 * state["text_size"])
        state["offsets"] = []
        ebook.seek(0)
        state["current_page"] = 0
        changed = True

    if display.pressed(badger2040w.BUTTON_B):
        state["font_idx"] += 1
        if (state["font_idx"] >= len(FONTS)):
            state["font_idx"] = 0
        state["offsets"] = []
        ebook.seek(0)
        state["current_page"] = 0
        changed = True

    if launch and not changed:
        if state["current_page"] > 0 and len(state["offsets"]) > state["current_page"] - 1:
            ebook.seek(state["offsets"][state["current_page"] - 1])
        changed = True
        launch = False

    if changed:
        draw_frame()
        render_page()

        # Is the next page one we've not displayed before?
        if state["current_page"] >= len(state["offsets"]):
            state["offsets"].append(ebook.tell())  # Add its start position to the state["offsets"] list
        badger_os.state_save("ebook", state)

        changed = False

    display.halt()
