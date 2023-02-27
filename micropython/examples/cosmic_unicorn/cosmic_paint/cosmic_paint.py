import os
from microdot_asyncio import Microdot, send_file
from microdot_asyncio_websocket import with_websocket
from phew import connect_to_wifi
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY
from WIFI_CONFIG import SSID, PSK


cu = CosmicUnicorn()
graphics = PicoGraphics(DISPLAY)
mv_graphics = memoryview(graphics)
cu.set_brightness(0.5)

WIDTH, HEIGHT = graphics.get_bounds()

ip = connect_to_wifi(SSID, PSK)

print(f"Start painting at: http://{ip}")


server = Microdot()


@server.route("/", methods=["GET"])
def route_index(request):
    return send_file("cosmic_paint/index.html")


@server.route("/static/<path:path>", methods=["GET"])
def route_static(request, path):
    return send_file(f"cosmic_paint/static/{path}")


def get_pixel(x, y):
    if x < WIDTH and y < HEIGHT and x >= 0 and y >= 0:
        o = (y * WIDTH + x) * 4
        return tuple(mv_graphics[o:o + 3])
    return None


def flood_fill(x, y, r, g, b):
    todo = []

    def fill(x, y, c):
        if get_pixel(x, y) != c:
            return

        graphics.pixel(x, y)

        up = get_pixel(x, y - 1)
        dn = get_pixel(x, y + 1)
        lf = get_pixel(x - 1, y)
        ri = get_pixel(x + 1, y)

        if up == c:
            todo.append((x, y - 1))
        if dn == c:
            todo.append((x, y + 1))
        if lf == c:
            todo.append((x - 1, y))
        if ri == c:
            todo.append((x + 1, y))

    c = get_pixel(x, y)

    if c is None:
        return

    fill(x, y, c)

    while len(todo):
        x, y = todo.pop(0)
        fill(x, y, c)


@server.route('/paint')
@with_websocket
async def echo(request, ws):
    while True:
        data = await ws.receive()
        try:
            x, y, r, g, b = [int(n) for n in data[0:5]]
            graphics.set_pen(graphics.create_pen(r, g, b))
            graphics.pixel(x, y)

        except ValueError:
            if data == "show":
                cu.update(graphics)

            if data == "fill":
                data = await ws.receive()
                x, y, r, g, b = [int(n) for n in data[0:5]]
                graphics.set_pen(graphics.create_pen(r, g, b))
                flood_fill(x, y, r, g, b)

            if data == "clear":
                graphics.set_pen(graphics.create_pen(0, 0, 0))
                graphics.clear()

            if data == "save":
                filename = await ws.receive()
                print(f"Saving to {filename}.bin")
                try:
                    os.mkdir("saves")
                except OSError:
                    pass
                with open(f"saves/{filename}.bin", "wb") as f:
                    f.write(graphics)
                await ws.send(f"alert: Saved to saves/{filename}.bin")


server.run(host="0.0.0.0", port=80)
