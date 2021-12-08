import hub75
import time

WIDTH, HEIGHT = 64, 64

hub = hub75.Hub75(WIDTH, HEIGHT, panel_type=hub75.PANEL_FM6126A)

hub.start()
hub.clear()
hub.flip()

while True:
    h = time.ticks_ms() / 5000.0
    hub.set_all_hsv(h, 1.0, 1.0)
    for y in range(8):
        for x in range(WIDTH):
            c = int(x * 255 / WIDTH)
            hub.set_rgb(x, y, c, c, c)
    for x in range(WIDTH):
        hub.set_rgb(x, x, 255, 0, 0)
        hub.set_rgb(WIDTH - 1 - x, x, 255, 0, 0)
    hub.flip()
    time.sleep(1.0 / 60)
