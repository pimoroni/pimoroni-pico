import time
from machine import Pin, PWM, ADC


BREAKOUT_GARDEN_I2C_PINS = {"sda": 4, "scl": 5}
PICO_EXPLORER_I2C_PINS = {"sda": 20, "scl": 21}


class Analog:
    def __init__(self, pin, amplifier_gain=1, resistor=0):
        self.gain = amplifier_gain
        self.resistor = resistor
        self.pin = ADC(pin)

    def read_voltage(self):
        return self.pin.read_u16() * 3.3 / 65535 / self.gain

    def read_current(self):
        if self.resistor > 0:
            return self.read_voltage() / self.resistor
        else:
            return self.read_voltage()


class Button:
    def __init__(self, button, invert=True, repeat_time=200, hold_time=1000):
        self.invert = invert
        self.repeat_time = repeat_time
        self.hold_time = hold_time
        self.pin = Pin(button, pull=Pin.PULL_UP if invert else Pin.PULL_DOWN)
        self.last_state = False
        self.pressed = False
        self.pressed_time = 0

    def read(self):
        current_time = time.ticks_ms()
        state = self.raw()
        changed = state != self.last_state
        self.last_state = state

        if changed:
            if state:
                self.pressed_time = current_time
                self.pressed = True
                self.last_time = current_time
                return True
            else:
                self.pressed_time = 0
                self.pressed = False
                self.last_time = 0

        if self.repeat_time == 0:
            return False

        if self.pressed:
            repeat_rate = self.repeat_time
            if self.hold_time > 0 and current_time - self.pressed_time > self.hold_time:
                repeat_rate /= 3
            if current_time - self.last_time > repeat_rate:
                self.last_time = current_time
                return True

        return False

    def raw(self):
        if self.invert:
            return not self.pin.value()
        else:
            return self.pin.value()


class RGBLED:
    def __init__(self, r, g, b, invert=True):
        self.invert = invert
        self.led_r = PWM(Pin(r))
        self.led_r.freq(1000)
        self.led_g = PWM(Pin(g))
        self.led_g.freq(1000)
        self.led_b = PWM(Pin(b))
        self.led_b.freq(1000)

    def set_rgb(self, r, g, b):
        if self.invert:
            r = 255 - r
            g = 255 - g
            b = 255 - b
        self.led_r.duty_u16(int((r * 65535) / 255))
        self.led_g.duty_u16(int((g * 65535) / 255))
        self.led_b.duty_u16(int((b * 65535) / 255))


class Motor:
    FAST_DECAY = 0  # Recirculation current fast decay mode (coasting)
    SLOW_DECAY = 1  # Recirculation current slow decay mode (braking)

    def __init__(self, pos, neg, freq=25000, decay_mode=Motor.SLOW_DECAY):
        self.speed = 0.0
        self.freq = freq
        if decay_mode in (Motor.FAST_DECAY, Motor.SLOW_DECAY):
            self.decay_mode = decay_mode
        else:
            raise ValueError("Decay mode value must be either Motor.FAST_DECAY or Motor.SLOW_DECAY")

        self.pos_pwm = PWM(Pin(pos))
        self.pos_pwm.freq(freq)
        self.neg_pwm = PWM(Pin(neg))
        self.neg_pwm.freq(freq)

    def get_speed(self):
        return self.speed

    def set_speed(self, speed):
        if speed > 1.0 or speed < -1.0:
            raise ValueError("Speed must be between -1.0 and +1.0")
        self.speed = speed
        self._update_pwm()

    def get_frequency(self):
        return self.freq

    def set_frequency(self, freq):
        self.pos_pwm.freq(freq)
        self.neg_pwm.freq(freq)
        self._update_pwm()

    def get_decay_mode(self):
        return self.decay_mode

    def set_decay_mode(self, mode):
        if mode in (Motor.FAST_DECAY, Motor.SLOW_DECAY):
            self.decay_mode = mode
            self._update_pwm()
        else:
            raise ValueError("Decay mode value must be either Motor.FAST_DECAY or Motor.SLOW_DECAY")

    def stop(self):
        self.speed = 0.0
        self._update_pwm()

    def disable(self):
        self.speed = 0.0
        self.pos_pwm.duty_u16(0)
        self.neg_pwm.duty_u16(0)

    def _update_pwm(self):
        signed_duty_cycle = int(self.speed * 0xFFFF)

        if self.decay_mode is Motor.SLOW_DECAY:  # aka 'Braking'
            if signed_duty_cycle >= 0:
                self.pos_pwm.duty_u16(0xFFFF)
                self.neg_pwm.duty_u16(0xFFFF - signed_duty_cycle)
            else:
                self.pos_pwm.duty_u16(0xFFFF + signed_duty_cycle)
                self.neg_pwm.duty_u16(0xFFFF)

        else:  # aka 'Coasting'
            if signed_duty_cycle >= 0:
                self.pos_pwm.duty_u16(signed_duty_cycle)
                self.neg_pwm.duty_u16(0)
            else:
                self.pos_pwm.duty_u16(0)
                self.neg_pwm.duty_u16(0 - signed_duty_cycle)
