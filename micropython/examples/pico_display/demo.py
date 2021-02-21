import time, random
import picodisplay as display 

width = display.get_width()
height = display.get_height()

display_buffer = bytearray(width * height * 2)  # 2-bytes per pixel (RGB565)
display.init(display_buffer)

display.set_backlight(1.0)


class Ball:
    def __init__(self, x, y, r, dx, dy, pen):
        self.x = x
        self.y = y
        self.r = r
        self.dx = dx
        self.dy = dy
        self.pen = pen

# initialise shapes
balls = []
for i in range(0, 100):
    balls.append(
        Ball(
            random.randint(0, width),
            random.randint(0, height),
            random.randint(0, 10) + 3,
            random.randint(0, 255) / 128,
            random.randint(0, 255) / 128,
            display.create_pen(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255)),            
        )
    )
    
while True:
    display.set_pen(120, 40, 60)    
    display.clear()
    
    for ball in balls:        
        ball.x += ball.dx
        ball.y += ball.dy
        
        if ball.x < 0 or ball.x > width:
            ball.dx *= -1
        if ball.y < 0 or ball.y > height:
            ball.dy *= -1
            
        display.set_pen(ball.pen)
        display.circle(int(ball.x), int(ball.y), int(ball.r))
        
    display.update()
    time.sleep(0.01)