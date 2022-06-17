import time
import random
from pimoroni import Button
from picographics import PicoGraphics, DISPLAY_TUFTY_2040

display = PicoGraphics(display=DISPLAY_TUFTY_2040)
WIDTH, HEIGHT = display.get_bounds()

# Load the spritsheets so we can flip between them
tilemap = bytearray(128 * 128)
open("s4m_ur4i-pirate-tilemap.rgb332", "rb").readinto(tilemap)

character = bytearray(128 * 128)
open("s4m_ur4i-pirate-characters.rgb332", "rb").readinto(character)

display.set_spritesheet(character)


# Buttons
button_a = Button(7, invert=False)
button_b = Button(8, invert=False)
button_c = Button(9, invert=False)
button_up = Button(22, invert=False)
button_down = Button(6, invert=False)

display.set_backlight(1.0)
display.set_pen(255)
display.clear()


class Player():
    def __init__(self):
        self.reset()

    def reset(self):
        self.x = 150
        self.y = 180
        self.w = 15
        self.h = 30
        self.speed = 10
        self.is_alive = True
        self.lives = 3
        self.score = 0
        self.moving = 0

    def move(self, x, y):
        if self.x + x > 0 - self.w and self.x + x < WIDTH - self.w:
            self.x += x
            self.y += y

    def sprite(self):
        display.set_spritesheet(character)
        display.sprite(1, 1 if self.moving else 0, self.x, self.y, 4, 0)


class Treasure():
    def __init__(self):
        self.w = 16
        self.h = 16
        self.randomize()

    def sprite(self):
        if not self.enabled:
            return
        display.set_spritesheet(tilemap)
        display.sprite(4, 2, self.x, self.y, 3, 0)

    def randomize(self):
        self.enabled = True
        self.x = random.randint(15, WIDTH - 60)
        self.y = HEIGHT - 50


class Block():
    def __init__(self):
        self.w = 16
        self.h = 16
        self.is_alive = True
        self.randomize()

    def move(self):
        self.y += self.speed

    def sprite(self):
        display.set_spritesheet(character)
        display.sprite(10, 8, self.x, self.y, 4, 0)

    def randomize(self):
        self.last_update = time.time()
        self.x = random.randint(10, WIDTH - self.w - 10)
        self.y = -self.h
        self.speed = random.randint(4, 12)


class Game():
    def __init__(self):
        self.player = Player()
        self.block = []
        self.last_new_block = 0

        self.treasure = Treasure()
        self.last_treasure = 0

        self.SKY = display.create_pen(72, 180, 224)

        for i in range(5):
            self.block.append(Block())

    def reset(self):
        for block in self.block:
            block.randomize()

        self.treasure.randomize()
        self.player.reset()

    def get_input(self):
        if button_c.read():
            self.player.move(self.player.speed, 0)
            self.player.moving = 0
        if button_a.read():
            self.player.move(-self.player.speed, 0)
            self.player.moving = 1

    def background(self):
        display.set_spritesheet(tilemap)
        display.set_pen(self.SKY)
        display.clear()

        for i in range(WIDTH / 32):
            display.sprite(1, 2, i * 32, 210, 4, 0)

    def draw(self):
        self.background()
        for block in self.block:
            block.sprite()
        display.set_pen(255)
        display.text("Score: " + str(self.player.score), 10, 10, 320, 2)
        self.treasure.sprite()
        display.set_pen(0)
        self.player.sprite()
        display.update()
        time.sleep(0.01)

    def check_collision(self, a, b):
        return a.x + a.w >= b.x and a.x <= b.x + b.w and a.y + a.h >= b.y and a.y <= b.y + b.h

    def update(self):
        for block in self.block:
            block.move()
            if block.y > HEIGHT:
                block.randomize()

            if block.y + block.h >= self.player.y and self.check_collision(self.player, block):
                block.randomize()
                self.player.is_alive = False

        if self.treasure.enabled:
            if self.check_collision(self.player, self.treasure):
                self.player.score += 1
                self.treasure.enabled = False
                self.last_treasure = time.time()

        if time.time() - self.last_treasure > 2:
            if not self.treasure.enabled:
                self.treasure.randomize()

        if self.player.lives == 0:
            self.player.is_alive = False


game = Game()

while True:
    game.background()
    display.set_pen(255)
    display.text("ARGH!", 40, 35, 200, 10)
    display.text("Press B to Start", 80, 150, 180, 2)
    display.update()

    while not button_b.read():
        pass

    while game.player.is_alive:
        game.get_input()
        game.update()
        game.draw()

    game.background()
    display.set_pen(255)
    display.text("OOPS!", 40, 35, 200, 10)
    display.text("Your score:  " + str(game.player.score), 50, 150, 180, 2)
    display.update()

    while not button_b.read():
        pass

    game.reset()
