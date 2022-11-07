import time
import random
from gfx_pack import GfxPack, SWITCH_A, SWITCH_B, SWITCH_C, SWITCH_D, SWITCH_E

"""
Basic Snake demo for GFX Pack
Feel free to add your own improvements :)

A = up
B = down
C = reset
D = left
E = right
"""

MOVE_UP = 0
MOVE_DOWN = 1
MOVE_LEFT = 2
MOVE_RIGHT = 3

next_move = MOVE_RIGHT
score = 0
head_position = (30, 30)
segments = [head_position]
ate_apple = False
apple_position = None

gp = GfxPack()
gp.set_backlight(0, 0, 0, 255)
display = gp.display

WIDTH, HEIGHT = display.get_bounds()


def set_new_apple():
    global apple_position
    apple_position = random.randint(0, WIDTH), random.randint(30, HEIGHT)


def game_over():
    global score, segments, head_position, ate_apple
    score = 0
    head_position = (30, 30)
    segments = [head_position]
    ate_apple = False
    set_new_apple()
    pass


def check_button():
    global next_move, ate_apple
    if gp.switch_pressed(SWITCH_A):
        if next_move != MOVE_DOWN:
            next_move = MOVE_UP
    elif gp.switch_pressed(SWITCH_B):
        if next_move != MOVE_UP:
            next_move = MOVE_DOWN
    elif gp.switch_pressed(SWITCH_C):
        if next_move != MOVE_RIGHT:
            next_move = MOVE_LEFT
    elif gp.switch_pressed(SWITCH_D):
        if next_move != MOVE_LEFT:
            next_move = MOVE_RIGHT
    elif gp.switch_pressed(SWITCH_E):
        game_over()


def check_eaten():
    global ate_apple, head_position, apple_position, score
    if head_position == apple_position:
        ate_apple = True
        score += 1
        set_new_apple()


def check_collision():
    for index in range(len(segments) - 1):
        if head_position == segments[index]:
            game_over()
            return
    if head_position[0] >= WIDTH:
        game_over()
    if head_position[0] <= 0:
        game_over()
    if head_position[1] >= HEIGHT:
        game_over()
    if head_position[1] <= 20:
        game_over()


def move():
    global head_position, segments, ate_apple

    head_x, head_y = head_position

    if next_move == MOVE_UP:
        head_y -= 1
    elif next_move == MOVE_DOWN:
        head_y += 1
    elif next_move == MOVE_LEFT:
        head_x -= 1
    elif next_move == MOVE_RIGHT:
        head_x += 1

    head_position = (head_x, head_y)
    segments.append(head_position)

    if ate_apple:
        ate_apple = False
    else:
        segments.pop(0)


def draw():
    display.set_pen(0)
    display.clear()
    display.set_pen(15)
    display.text("score: {0}".format(score), 0, 0)
    display.line(0, 20, 127, 20)
    display.line(0, 63, 127, 63)
    display.line(0, 63, 0, 20)
    display.line(128, 63, 127, 20)
    # Draw apple
    display.pixel(apple_position[0], apple_position[1])

    # Drawing snake
    for segment in segments:
        display.pixel(segment[0], segment[1])

    display.update()


game_over()

while True:
    check_button()
    check_eaten()
    move()
    check_collision()
    draw()

    time.sleep(0.2)
