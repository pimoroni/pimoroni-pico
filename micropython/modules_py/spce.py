# SPDX-FileCopyrightText: 2026 Christopher Parrott for Pimoroni Ltd
#
# SPDX-License-Identifier: MIT
#
# The SP/CE connectors a host firmware names, each as a tuple of its five pins, and
# classes for the breakouts that plug into one. A board names a connector's lines
# SPCE_DC, SPCE_CS, SPCE_SCK, SPCE_MOSI and SPCE_BL in its pins.csv, lettered as
# SPCE_A_DC where it has more than one, so supporting a host takes only those names.
# Nothing here is board specific, and a connector left unnamed is None.

from machine import UART, Pin

__LINE_ROLES = ("DC", "CS", "SCK", "MOSI", "BL")


def connector(name=None):
    """The five pins the firmware names for the connector lettered `name`, or None."""
    prefix = "SPCE_" if name is None else f"SPCE_{name}_"
    try:
        return tuple(getattr(Pin.board, prefix + role) for role in __LINE_ROLES)
    except AttributeError:
        return None


SPCE_PINS = connector()       # A host's only connector, its lines named without a letter
SPCE_A_PINS = connector("A")
SPCE_B_PINS = connector("B")


def gpio_number(pin):
    """The GPIO number of a Pin."""
    # A Pin gives up its number only through its repr, Pin(GPIO34, ...)
    return int(str(pin)[8:].split(",")[0])


class MotorDriver:
    """Two motors and the chip driving them, on the connector the breakout plugs into.

    The breakout's driver chip runs the motors off four lines, enabled by the fifth.
    It starts disabled, so nothing wired to it moves while a program comes up.
    """

    def __init__(self, pins):
        from motor import Motor

        # Pin() returns a Pin it is given unchanged, so pins may be Pins or GPIO numbers
        a_pos, a_neg, b_pos, b_neg, enable = (gpio_number(Pin(pin)) for pin in pins)

        # Named as the breakout labels them, and as a sequence for a program driving both
        self.motor_a = Motor((a_pos, a_neg))
        self.motor_b = Motor((b_pos, b_neg))
        self.motors = (self.motor_a, self.motor_b)

        self.__enable = Pin(enable, Pin.OUT, value=False)

    def enable(self):
        """Enable the driver. A motor then moves on its next speed or enable()."""
        self.__enable.on()

    def disable(self):
        """Stop both motors and disable the driver.

        Disabling the chip alone would leave the direction LEDs lit, since the motors'
        pins also drive them.
        """
        for motor in self.motors:
            motor.disable()

        self.__enable.off()

    def is_enabled(self):
        """Whether the driver is enabled."""
        return self.__enable.value() == 1


class Clipper:
    """The Pimoroni Clipper, an LTE modem on a connector, reached as `modem`.

    The connector's lines are the modem's UART TX and RX, its network status output, its
    reset and its power key, so which UART it uses follows from the connector's pins.
    """

    def __init__(self, pins, apn, netlight_led=None, skip_reset=False):
        from lte import LTE

        tx, rx, netlight, reset, power = (Pin(pin) for pin in pins)

        # Left as the firmware set it. The reset line alone brings the modem up, and the
        # datasheet's power key has never been needed, so driving it would be a guess
        self.power_key = power

        self.modem = LTE(apn,
                         uart=UART(self.__uart_instance(tx), tx=tx, rx=rx),
                         reset_pin=Pin(reset, Pin.OUT),
                         netlight_pin=Pin(netlight, Pin.IN),
                         netlight_led=netlight_led,
                         skip_reset=skip_reset)

    @staticmethod
    def __uart_instance(tx):
        # RP2 UARTs alternate in blocks of eight GPIOs offset by four, which is the test
        # MicroPython itself applies to a requested pin
        return ((gpio_number(tx) + 4) & 8) >> 3
