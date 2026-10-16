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


class __ClusterMotor:
    # One motor of a MotorCluster, offering the methods of a Motor

    def __init__(self, cluster, index):
        self.__cluster = cluster
        self.__index = index

    def pins(self):
        return self.__cluster.pins(self.__index)

    def enable(self):
        self.__cluster.enable(self.__index)

    def disable(self):
        self.__cluster.disable(self.__index)

    def is_enabled(self):
        return self.__cluster.is_enabled(self.__index)

    # These read with no argument and set with one, as a Motor's methods do
    def duty(self, *args):
        return self.__cluster.duty(self.__index, *args)

    def speed(self, *args):
        return self.__cluster.speed(self.__index, *args)

    # The cluster's, so setting it on either motor sets both
    def frequency(self, *args):
        return self.__cluster.frequency(*args)

    def direction(self, *args):
        return self.__cluster.direction(self.__index, *args)

    def speed_scale(self, *args):
        return self.__cluster.speed_scale(self.__index, *args)

    def zeropoint(self, *args):
        return self.__cluster.zeropoint(self.__index, *args)

    def deadzone(self, *args):
        return self.__cluster.deadzone(self.__index, *args)

    def decay_mode(self, *args):
        return self.__cluster.decay_mode(self.__index, *args)

    def stop(self):
        self.__cluster.stop(self.__index)

    def coast(self):
        self.__cluster.coast(self.__index)

    def brake(self):
        self.__cluster.brake(self.__index)

    def full_negative(self):
        self.__cluster.full_negative(self.__index)

    def full_positive(self):
        self.__cluster.full_positive(self.__index)

    def to_percent(self, *args):
        self.__cluster.to_percent(self.__index, *args)


class MotorDriver:
    """Two motors and the chip driving them, on the connector the breakout plugs into.

    The breakout's driver chip runs the motors off four lines, enabled by the fifth.
    It starts disabled, so nothing wired to it moves while a program comes up.

    Given a PIO and state machine, that state machine drives both motors as a MotorCluster
    at one frequency, leaving the hardware PWM channels behind their pins to other pins.
    """

    def __init__(self, pins, pio=None, sm=None):
        a_pos, a_neg, b_pos, b_neg, enable = pins

        # Check if the motors take hardware PWM, or a cluster on the PIO given.
        # Either way they are named as the breakout labels them
        if pio is None:
            from motor import Motor
            self.motor_a = Motor((a_pos, a_neg))
            self.motor_b = Motor((b_pos, b_neg))
            self.__held = (self.motor_a, self.motor_b)
        else:
            from motor import MotorCluster
            cluster = MotorCluster(pio, sm, ((a_pos, a_neg), (b_pos, b_neg)))
            self.motor_a = __ClusterMotor(cluster, 0)
            self.motor_b = __ClusterMotor(cluster, 1)
            self.__held = (cluster,)
        self.motors = (self.motor_a, self.motor_b)  # For a program driving both

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

    def deinit(self):
        """Disable the driver and release the hardware its motors run on, ending their use.

        Collection alone can leave that hardware claimed, as a stale reference may remain.
        """
        self.disable()
        for held in self.__held:
            held.__del__()


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
        # A Pin gives up its number only through its repr, Pin(GPIO34, ...)
        gpio = int(str(tx)[8:].split(",")[0])

        # RP2 UARTs alternate in blocks of eight GPIOs offset by four, which is the test
        # MicroPython itself applies to a requested pin
        return ((gpio + 4) & 8) >> 3
