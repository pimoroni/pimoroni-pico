import time
import usb.device
from usb.device.keyboard import KeyboardInterface, KeyCode
from pimoroni import Button

# A very basic HID Keyboard example for Keybow 2040
# Inspired by: https://github.com/micropython/micropython-lib/blob/master/micropython/usb/examples/device/keyboard_example.py

# This example requires a Pico USB compatible firmware, eg: pico_usb-1.23.0-pimoroni-micropython.uf2

# The pin order for Keybow 2040 is weird,
# But the below is laid out to match the pad with USB up
# from top left to bottom right.
KEYS = {
    18: KeyCode.A, 14: KeyCode.B, 10: KeyCode.C, 6: KeyCode.D,
    19: KeyCode.E, 15: KeyCode.F, 11: KeyCode.G, 7: KeyCode.H,
    20: KeyCode.I, 16: KeyCode.J, 12: KeyCode.K, 8: KeyCode.L,
    21: KeyCode.M, 17: KeyCode.N, 13: KeyCode.O, 9: KeyCode.P,
}

# We'll fill this with Button instances
BUTTONS = {}


class Keybow2040(KeyboardInterface):
    def on_led_update(self, led_mask):
        pass


def main():
    for pin, keycode in KEYS.items():
        BUTTONS[pin] = Button(pin)

    k = Keybow2040()
    usb.device.get().init(k, builtin_driver=True)

    keys = [0, 0, 0, 0, 0, 0]

    while True:
        changed = False
        if k.is_open():
            for pin, code in KEYS.items():
                pressed = BUTTONS[pin].read()

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
                time.sleep_ms(1)


if __name__ == "__main__":
    print("Starting Keybow 2040...")
    main()
