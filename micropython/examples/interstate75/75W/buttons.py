'''
buttons.py
Push either switch A, switch B or the BOOT switch (in the case of the non-w version) to change the display
'''
import interstate75

i75 = interstate75.Interstate75(display=interstate75.DISPLAY_INTERSTATE75_32X32)
graphics = i75.display

width = i75.width
height = i75.height

A_COLOR = graphics.create_pen(0x31, 0x81, 0xCE)
A_TEXT = graphics.create_pen(0xCE, 0x7E, 0x31)

B_COLOR = graphics.create_pen(0xC3, 0x3C, 0xBD)
B_TEXT = graphics.create_pen(0x3C, 0xC3, 0x42)

BOOT_COLOR = graphics.create_pen(0xC3, 0x3C, 0xBD)
BOOT_TEXT = graphics.create_pen(0x3C, 0xC3, 0x42)

BG = graphics.create_pen(0xC1, 0x99, 0x3E)


def display_a():
    graphics.set_pen(A_COLOR)
    graphics.clear()
    graphics.set_pen(A_TEXT)
    graphics.text("A", 8, 6, False, 3)
    i75.update()


def display_b():
    graphics.set_pen(B_COLOR)
    graphics.clear()
    graphics.set_pen(B_TEXT)
    graphics.text("B", 8, 6, False, 3)
    i75.update()


graphics.set_pen(BG)
graphics.clear()
i75.update()

while 1:
    if i75.switch_pressed(interstate75.SWITCH_A):
        display_a()
    if i75.switch_pressed(interstate75.SWITCH_B):
        display_b()
