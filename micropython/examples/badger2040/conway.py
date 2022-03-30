import math
import time
from random import random

import machine

import badger2040

# Overclock the RP2040 to run the sim faster
badger2040.system_speed(badger2040.SYSTEM_TURBO)

# ------------------------------
#        Program setup
# ------------------------------

# Global constants
CELL_SIZE = 6  # Size of cell in pixels
INITIAL_DENSITY = 0.3  # Density of cells at start

# Create a new Badger and set it to update TURBO
screen = badger2040.Badger2040()
screen.led(128)
screen.update_speed(badger2040.UPDATE_TURBO)

restart = False  # should sim be restarted

# ------------------------------
#     Button functions
# ------------------------------


# Button handling function
def button(pin):
    global restart
    # if 'a' button is pressed, restart the sim
    if pin == button_a:
        restart = True


# Set up button
button_a = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_a.irq(trigger=machine.Pin.IRQ_RISING, handler=button)

# ------------------------------
#      Screen functions
# ------------------------------


# Remove everything from the screen
def init_screen():
    screen.update_speed(badger2040.UPDATE_NORMAL)
    screen.pen(15)
    screen.clear()
    screen.update()
    screen.update_speed(badger2040.UPDATE_TURBO)


# ------------------------------
#       Classes
# ------------------------------

# Define a 'cell'
class Cell:
    def __init__(self):
        self._alive = False

    def make_alive(self):
        self._alive = True

    def make_dead(self):
        self._alive = False

    def is_alive(self):
        return self._alive


# Define the whole board
class Board:
    def __init__(self):
        self._rows = math.floor(badger2040.WIDTH / CELL_SIZE)
        self._columns = math.floor(badger2040.HEIGHT / CELL_SIZE)
        self._grid = [[Cell() for _ in range(self._columns)] for _ in range(self._rows)]

        self._initialise_board()

    # Draw the board to the screen
    def draw_board(self):
        row_idx = 0
        column_idx = 0

        for row in self._grid:
            column_idx = 0
            for cell in row:
                if cell.is_alive():
                    screen.pen(0)
                else:
                    screen.pen(15)
                screen.rectangle(
                    row_idx * CELL_SIZE, column_idx * CELL_SIZE, CELL_SIZE, CELL_SIZE
                )
                column_idx += 1
            row_idx += 1

        screen.update()

    # Generate the first iteration of the board
    def _initialise_board(self):
        for row in self._grid:
            for cell in row:
                if random() <= INITIAL_DENSITY:
                    cell.make_alive()

    # Get the neighbour cells for a given cell
    def get_neighbours(self, current_row, current_column):
        # Cells either side of current cell
        neighbour_min = -1
        neighbour_max = 2
        neighbours = []

        for row in range(neighbour_min, neighbour_max):
            for column in range(neighbour_min, neighbour_max):
                neighbour_row = current_row + row
                neighbour_column = current_column + column
                # Don't count the current cell
                if not (
                    neighbour_row == current_row and neighbour_column == current_column
                ):
                    # It's a toroidal world so go all the way round if necessary
                    if (neighbour_row) < 0:
                        neighbour_row = self._rows - 1
                    elif (neighbour_row) >= self._rows:
                        neighbour_row = 0

                    if (neighbour_column) < 0:
                        neighbour_column = self._columns - 1
                    elif (neighbour_column) >= self._columns:
                        neighbour_column = 0

                    neighbours.append(self._grid[neighbour_row][neighbour_column])
        return neighbours

    # Calculate the next generation
    def create_next_generation(self):
        to_alive = []
        to_dead = []
        changed = False

        for row in range(len(self._grid)):
            for column in range(len(self._grid[row])):
                # Get all the neighours that are alive
                alive_neighbours = []
                for neighbour_cell in self.get_neighbours(row, column):
                    if neighbour_cell.is_alive():
                        alive_neighbours.append(neighbour_cell)

                current_cell = self._grid[row][column]
                # Apply the Conway GoL rules (B3/S23)
                if current_cell.is_alive():
                    if len(alive_neighbours) < 2 or len(alive_neighbours) > 3:
                        to_dead.append(current_cell)
                    if len(alive_neighbours) == 3 or len(alive_neighbours) == 2:
                        to_alive.append(current_cell)
                else:
                    if len(alive_neighbours) == 3:
                        to_alive.append(current_cell)

        for cell in to_alive:
            if not cell.is_alive():
                # The board has changed since the previous generation
                changed = True
            cell.make_alive()

        for cell in to_dead:
            if cell.is_alive():
                # The board has changed since the previous generation
                changed = True
            cell.make_dead()

        return changed


# ------------------------------
#       Main program loop
# ------------------------------


def main():
    global restart

    init_screen()
    board = Board()
    board.draw_board()
    time.sleep(0.5)

    while True:
        # The 'a' button has been pressed so restart sim
        if restart:
            init_screen()
            restart = False
            board = Board()
            board.draw_board()
            time.sleep(0.5)
        # The board didn't update since the previous generation
        if not board.create_next_generation():
            screen.update_speed(badger2040.UPDATE_NORMAL)
            board.draw_board()
            screen.update_speed(badger2040.UPDATE_TURBO)
            time.sleep(5)
            restart = True
        # Draw the next generation
        else:
            board.draw_board()


main()
