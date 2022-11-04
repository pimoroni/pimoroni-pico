"""
Basic Snake demo for GFX Pack
Feel free to add your own improvements :)
A = up
B = down
C = reset
D = left
E = right
"""
from picographics import PicoGraphics, DISPLAY_GFX_PACK
from gfx_pack import GfxPack
import time
import random

MOVE_UP = 0
MOVE_DOWN = 1
MOVE_LEFT = 2
MOVE_RIGHT = 3
next_move = MOVE_RIGHT
score = 0
head_possition = (30, 30)
segments = [head_possition]
ate_apple = False
apple_possition = None

display = PicoGraphics(display=DISPLAY_GFX_PACK)
display.set_backlight(1.0)

gp = GfxPack()

WIDTH, HEIGHT = display.get_bounds()


def set_new_apple():
    global apple_possition
    apple_possition = (random.randint(0, WIDTH), random.randint(30, HEIGHT))


def game_over():
    global score, segments, head_possition, ate_apple
    score = 0
    head_possition = (30, 30)
    segments = [head_possition]
    ate_apple = False
    set_new_apple()
    pass


def check_button():
    global next_move, ate_apple
    if gp.switch_a.is_pressed:
        if (next_move != MOVE_DOWN):
            next_move = MOVE_UP
    elif gp.switch_b.is_pressed:
        if (next_move != MOVE_UP):
            next_move = MOVE_DOWN
    elif gp.switch_d.is_pressed:
        if (next_move != MOVE_RIGHT):
            next_move = MOVE_LEFT
    elif gp.switch_e.is_pressed:
        if (next_move != MOVE_LEFT):
            next_move = MOVE_RIGHT
    elif gp.switch_c.is_pressed:
        game_over()


def check_eaten():
    global ate_apple, head_possition, apple_possition, score
    if (head_possition == apple_possition):
        ate_apple = True
        score += 1
        set_new_apple()


def check_collision():
    for index in range(len(segments) - 1):
        if (head_possition == segments[index]):
            game_over()
            return
    if (head_possition[0] >= WIDTH):
        game_over()
    if (head_possition[0] <= 0):
        game_over()
    if (head_possition[1] >= HEIGHT):
        game_over()
    if (head_possition[1] <= 20):
        game_over()


def move():
    global head_possition, segments, ate_apple

    head_x, head_y = head_possition

    if (next_move == MOVE_UP):
        head_y -= 1
    elif (next_move == MOVE_DOWN):
        head_y += 1
    elif (next_move == MOVE_LEFT):
        head_x -= 1
    elif (next_move == MOVE_RIGHT):
        head_x += 1

    head_possition = (head_x, head_y)
    segments.append(head_possition)

    if (ate_apple):
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
    display.pixel(apple_possition[0], apple_possition[1])

    # Drawing snake
    for segment in segments:
        display.pixel(segment[0], segment[1])

    display.update()


game_over()

while 1:
    check_button()
    check_eaten()
    move()
    check_collision()
    draw()
    time.sleep(0.2)
