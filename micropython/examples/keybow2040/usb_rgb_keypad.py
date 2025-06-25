import machine
import usb.device
from usb.device.keyboard import KeyboardInterface, KeyCode
from pimoroni import Button
from is31fl3731 import Matrix_Keybow2040

# A very basic HID Keyboard example for Keybow 2040
# Inspired by: https://github.com/micropython/micropython-lib/blob/master/micropython/usb/examples/device/keyboard_example.py

# This example requires a Pico USB compatible firmware, eg: pico_usb-1.23.0-pimoroni-micropython.uf2

# Don't forget to copy over is31fl3731.py too!

# The pin order for Keybow 2040 is weird,
# But the below is laid out to match the pad with USB up
# from top left to bottom right.
KEYS = {
    18: KeyCode.A, 14: KeyCode.B, 10: KeyCode.C, 6: KeyCode.D,
    19: KeyCode.E, 15: KeyCode.F, 11: KeyCode.G, 7: KeyCode.H,
    20: KeyCode.I, 16: KeyCode.J, 12: KeyCode.K, 8: KeyCode.L,
    21: KeyCode.M, 17: KeyCode.N, 13: KeyCode.O, 9: KeyCode.P,
}

# Another look up table for the balmy LED positioning
PIXELS = {
    18: (3, 0), 14: (3, 1), 10: (3, 2), 6: (3, 3),
    19: (2, 0), 15: (2, 1), 11: (2, 2), 7: (2, 3),
    20: (1, 0), 16: (1, 1), 12: (1, 2), 8: (1, 3),
    21: (0, 0), 17: (0, 1), 13: (0, 2), 9: (0, 3),
}

# We'll fill this with Button instances
BUTTONS = {}
VALUES = {}
MAX_ON_TIME = 20.0


# From CPython Lib/colorsys.py
def hsv_to_rgb(h, s, v):  # noqa: RET503
    if s == 0.0:
        return v, v, v
    i = int(h * 6.0)
    f = (h * 6.0) - i
    p = v * (1.0 - s)
    q = v * (1.0 - s * f)
    t = v * (1.0 - s * (1.0 - f))
    i = i % 6
    if i == 0:
        return v, t, p
    if i == 1:
        return q, v, p
    if i == 2:
        return p, v, t
    if i == 3:
        return p, q, v
    if i == 4:
        return t, p, v
    if i == 5:
        return v, p, q


class Keybow2040(KeyboardInterface):
    def on_led_update(self, led_mask):
        pass


def main():
    for pin, _keycode in KEYS.items():
        BUTTONS[pin] = Button(pin, repeat_time=0)
        VALUES[pin] = 0

    m = Matrix_Keybow2040(machine.I2C(0, 400000))
    k = Keybow2040()
    usb.device.get().init(k, builtin_driver=True)

    keys = [0, 0, 0, 0, 0, 0]

    while True:
        changed = False
        if k.is_open():
            for pin, code in KEYS.items():
                pressed = BUTTONS[pin].read()

                if pressed:
                    VALUES[pin] = MAX_ON_TIME

                # If the key is pressed then try to insert
                # it at the first zero. Otherwise (try to) replace
                # its keycode with 0 to clear that press.
                c = code if pressed else 0
                i = 0 if pressed else code

                try:
                    keys[keys.index(i)] = c
                    changed = True
                except ValueError:
                    # Either our 6-key list is full
                    # or we're releasing a key that's not in it!
                    # Or handling a key that isn't pressed.
                    pass

            if changed:
                k.send_keys(keys)

        for pin, _code in KEYS.items():
            x, y = PIXELS[pin]
            v = VALUES[pin] / MAX_ON_TIME
            r, g, b = [int(c * 255) for c in hsv_to_rgb(x / 4.0, 0.7, 1.0 - v)]
            m.pixelrgb(x, y, r, g, b)
            if VALUES[pin] > 0:
                VALUES[pin] -= 1


if __name__ == "__main__":
    print("Starting Keybow 2040...")
    main()
