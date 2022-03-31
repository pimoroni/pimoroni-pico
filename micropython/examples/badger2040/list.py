import binascii

import badger2040
import badger_os

# **** Put your list title here *****
list_title = "Checklist"
list_file = "checklist.txt"


# Global Constants
WIDTH = badger2040.WIDTH
HEIGHT = badger2040.HEIGHT

ARROW_THICKNESS = 3
ARROW_WIDTH = 18
ARROW_HEIGHT = 14
ARROW_PADDING = 2

MAX_ITEM_CHARS = 26
TITLE_TEXT_SIZE = 0.7
ITEM_TEXT_SIZE = 0.6
ITEM_SPACING = 20

LIST_START = 40
LIST_PADDING = 2
LIST_WIDTH = WIDTH - LIST_PADDING - LIST_PADDING - ARROW_WIDTH
LIST_HEIGHT = HEIGHT - LIST_START - LIST_PADDING - ARROW_HEIGHT


# Default list items - change the list items by editing checklist.txt
list_items = ["Badger", "Badger", "Badger", "Badger", "Badger", "Mushroom", "Mushroom", "Snake"]
save_checklist = False

try:
    with open("checklist.txt", "r") as f:
        raw_list_items = f.read()

    if raw_list_items.find(" X\n") != -1:
        # Have old style checklist, preserve state and note we should resave the list to remove the Xs
        list_items = []
        state = {
            "current_item": 0,
            "checked": []
        }
        for item in raw_list_items.strip().split("\n"):
            if item.endswith(" X"):
                state["checked"].append(True)
                item = item[:-2]
            else:
                state["checked"].append(False)
            list_items.append(item)
        state["items_hash"] = binascii.crc32("\n".join(list_items))

        badger_os.state_save("list", state)
        save_checklist = True
    else:
        list_items = [item.strip() for item in raw_list_items.strip().split("\n")]

except OSError:
    save_checklist = True

if save_checklist:
    with open("checklist.txt", "w") as f:
        for item in list_items:
            f.write(item + "\n")


# ------------------------------
#      Drawing functions
# ------------------------------

# Draw the list of items
def draw_list(items, item_states, start_item, highlighted_item, x, y, width, height, item_height, columns):
    item_x = 0
    item_y = 0
    current_col = 0
    for i in range(start_item, len(items)):
        if i == highlighted_item:
            display.pen(12)
            display.rectangle(item_x, item_y + y - (item_height // 2), width // columns, item_height)
        display.pen(0)
        display.text(items[i], item_x + x + item_height, item_y + y, ITEM_TEXT_SIZE)
        draw_checkbox(item_x, item_y + y - (item_height // 2), item_height, 15, 0, 2, item_states[i], 2)
        item_y += item_height
        if item_y >= height - (item_height // 2):
            item_x += width // columns
            item_y = 0
            current_col += 1
            if current_col >= columns:
                return


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


# Draw a left arrow
def draw_left(x, y, width, height, thickness, padding):
    border = (thickness // 2) + padding
    display.line(x + width - border, y + border,
                 x + border, y + (height // 2))
    display.line(x + border, y + (height // 2),
                 x + width - border, y + height - border)


# Draw a right arrow
def draw_right(x, y, width, height, thickness, padding):
    border = (thickness // 2) + padding
    display.line(x + border, y + border,
                 x + width - border, y + (height // 2))
    display.line(x + width - border, y + (height // 2),
                 x + border, y + height - border)


# Draw a tick
def draw_tick(x, y, width, height, thickness, padding):
    border = (thickness // 2) + padding
    display.line(x + border, y + ((height * 2) // 3),
                 x + (width // 2), y + height - border)
    display.line(x + (width // 2), y + height - border,
                 x + width - border, y + border)


# Draw a cross
def draw_cross(x, y, width, height, thickness, padding):
    border = (thickness // 2) + padding
    display.line(x + border, y + border, x + width - border, y + height - border)
    display.line(x + width - border, y + border, x + border, y + height - border)


# Draw a checkbox with or without a tick
def draw_checkbox(x, y, size, background, foreground, thickness, tick, padding):
    border = (thickness // 2) + padding
    display.pen(background)
    display.rectangle(x + border, y + border, size - (border * 2), size - (border * 2))
    display.pen(foreground)
    display.thickness(thickness)
    display.line(x + border, y + border, x + size - border, y + border)
    display.line(x + border, y + border, x + border, y + size - border)
    display.line(x + size - border, y + border, x + size - border, y + size - border)
    display.line(x + border, y + size - border, x + size - border, y + size - border)
    if tick:
        draw_tick(x, y, size, size, thickness, 2 + border)


# ------------------------------
#        Program setup
# ------------------------------

changed = not badger2040.woken_by_button()
state = {
    "current_item": 0,
}
badger_os.state_load("list", state)
items_hash = binascii.crc32("\n".join(list_items))
if "items_hash" not in state or state["items_hash"] != items_hash:
    # Item list changed, or not yet written reset the list
    state["current_item"] = 0
    state["items_hash"] = items_hash
    state["checked"] = [False] * len(list_items)
    changed = True

# Global variables
items_per_page = 0

# Create a new Badger and set it to update FAST
display = badger2040.Badger2040()
display.led(128)
if changed:
    display.update_speed(badger2040.UPDATE_FAST)
else:
    display.update_speed(badger2040.UPDATE_TURBO)

# Find out what the longest item is
longest_item = 0
for i in range(len(list_items)):
    while True:
        item = list_items[i]
        item_length = display.measure_text(item, ITEM_TEXT_SIZE)
        if item_length > 0 and item_length > LIST_WIDTH - ITEM_SPACING:
            list_items[i] = item[:-1]
        else:
            break
    longest_item = max(longest_item, display.measure_text(list_items[i], ITEM_TEXT_SIZE))


# And use that to calculate the number of columns we can fit onscreen and how many items that would give
list_columns = 1
while longest_item + ITEM_SPACING < (LIST_WIDTH // (list_columns + 1)):
    list_columns += 1

items_per_page = ((LIST_HEIGHT // ITEM_SPACING) + 1) * list_columns


# ------------------------------
#       Main program loop
# ------------------------------

while True:
    if len(list_items) > 0:
        if display.pressed(badger2040.BUTTON_A):
            if state["current_item"] > 0:
                page = state["current_item"] // items_per_page
                state["current_item"] = max(state["current_item"] - (items_per_page) // list_columns, 0)
                if page != state["current_item"] // items_per_page:
                    display.update_speed(badger2040.UPDATE_FAST)
                changed = True
        if display.pressed(badger2040.BUTTON_B):
            state["checked"][state["current_item"]] = not state["checked"][state["current_item"]]
            changed = True
        if display.pressed(badger2040.BUTTON_C):
            if state["current_item"] < len(list_items) - 1:
                page = state["current_item"] // items_per_page
                state["current_item"] = min(state["current_item"] + (items_per_page) // list_columns, len(list_items) - 1)
                if page != state["current_item"] // items_per_page:
                    display.update_speed(badger2040.UPDATE_FAST)
                changed = True
        if display.pressed(badger2040.BUTTON_UP):
            if state["current_item"] > 0:
                state["current_item"] -= 1
                changed = True
        if display.pressed(badger2040.BUTTON_DOWN):
            if state["current_item"] < len(list_items) - 1:
                state["current_item"] += 1
                changed = True

    if changed:
        badger_os.state_save("list", state)

        display.pen(15)
        display.clear()

        display.pen(12)
        display.rectangle(WIDTH - ARROW_WIDTH, 0, ARROW_WIDTH, HEIGHT)
        display.rectangle(0, HEIGHT - ARROW_HEIGHT, WIDTH, ARROW_HEIGHT)

        y = LIST_PADDING + 12
        display.pen(0)
        display.thickness(3)
        display.text(list_title, LIST_PADDING, y, TITLE_TEXT_SIZE)

        y += 12
        display.pen(0)
        display.thickness(2)
        display.line(LIST_PADDING, y, WIDTH - LIST_PADDING - ARROW_WIDTH, y)

        if len(list_items) > 0:
            page_item = 0
            if items_per_page > 0:
                page_item = (state["current_item"] // items_per_page) * items_per_page

            # Draw the list
            display.pen(0)
            display.thickness(2)
            draw_list(list_items, state["checked"], page_item, state["current_item"], LIST_PADDING, LIST_START,
                      LIST_WIDTH, LIST_HEIGHT, ITEM_SPACING, list_columns)

            # Draw the interaction button icons
            display.pen(0)
            display.thickness(ARROW_THICKNESS)

            # Previous item
            if state["current_item"] > 0:
                draw_up(WIDTH - ARROW_WIDTH, (HEIGHT // 4) - (ARROW_HEIGHT // 2),
                        ARROW_WIDTH, ARROW_HEIGHT, ARROW_THICKNESS, ARROW_PADDING)

            # Next item
            if state["current_item"] < (len(list_items) - 1):
                draw_down(WIDTH - ARROW_WIDTH, ((HEIGHT * 3) // 4) - (ARROW_HEIGHT // 2),
                          ARROW_WIDTH, ARROW_HEIGHT, ARROW_THICKNESS, ARROW_PADDING)

            # Previous column
            if state["current_item"] > 0:
                draw_left((WIDTH // 7) - (ARROW_WIDTH // 2), HEIGHT - ARROW_HEIGHT,
                          ARROW_WIDTH, ARROW_HEIGHT, ARROW_THICKNESS, ARROW_PADDING)

            # Next column
            if state["current_item"] < (len(list_items) - 1):
                draw_right(((WIDTH * 6) // 7) - (ARROW_WIDTH // 2), HEIGHT - ARROW_HEIGHT,
                           ARROW_WIDTH, ARROW_HEIGHT, ARROW_THICKNESS, ARROW_PADDING)

            if state["checked"][state["current_item"]]:
                # Tick off item
                draw_cross((WIDTH // 2) - (ARROW_WIDTH // 2), HEIGHT - ARROW_HEIGHT,
                           ARROW_HEIGHT, ARROW_HEIGHT, ARROW_THICKNESS, ARROW_PADDING)
            else:
                # Untick item
                draw_tick((WIDTH // 2) - (ARROW_WIDTH // 2), HEIGHT - ARROW_HEIGHT,
                          ARROW_HEIGHT, ARROW_HEIGHT, ARROW_THICKNESS, ARROW_PADDING)
        else:
            # Say that the list is empty
            empty_text = "Nothing Here"
            text_length = display.measure_text(empty_text, ITEM_TEXT_SIZE)
            display.text(empty_text, ((LIST_PADDING + LIST_WIDTH) - text_length) // 2, (LIST_HEIGHT // 2) + LIST_START - (ITEM_SPACING // 4), ITEM_TEXT_SIZE)

        display.update()
        display.update_speed(badger2040.UPDATE_TURBO)
        changed = False

    display.halt()
