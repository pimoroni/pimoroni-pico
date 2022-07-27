import time
from galactic import GalacticUnicorn

gu = GalacticUnicorn()

def gradient(r, g, b):
    for y in range(0, 11):
        for x in range(0, 53):
            # graphics.set_pen((r * x) / 52, (g * x) / 52, (b * x) / 52);
            gu.set_pixel(x, y, x)


def grid(ms, x, y) {
    v = (x + y + (ms / 1000)) % 2
    if v == 0:
        gu.set_pixel(x, y, 255)
    else:
        gu.set_pixel(x, y, 0)
}

void shader_fill(int ms, shader_func_t f) {
  for(int y = 0; y < 12; y++) {
    for(int x = 0; x < 53; x++) {
      f(ms, x, y);
    }
  } 
}

void outline_text(std::string text) {
  uint ms = to_ms_since_boot(get_absolute_time());

  graphics.set_font("bitmap8");
  uint8_t v = (sin(ms / 100.0f) + 1.0f) * 127.0f;
  uint w = graphics.measure_text(text, 1);

  int x = 53 / 2 - w / 2 + 1, y = 2;

  graphics.set_pen(0, 0, 0);
  graphics.text(text, Point(x - 1, y - 1), -1, 1);
  graphics.text(text, Point(x    , y - 1), -1, 1);
  graphics.text(text, Point(x + 1, y - 1), -1, 1);
  graphics.text(text, Point(x - 1, y    ), -1, 1);
  graphics.text(text, Point(x + 1, y    ), -1, 1);
  graphics.text(text, Point(x - 1, y + 1), -1, 1);
  graphics.text(text, Point(x    , y + 1), -1, 1);
  graphics.text(text, Point(x + 1, y + 1), -1, 1);

  graphics.set_pen(v, v, v);
  graphics.text(text, Point(x, y), -1, 1);
}