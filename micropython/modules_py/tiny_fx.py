from machine import ADC, Pin, PWM
#from pimoroni import PWMLED
from pimoroni_i2c import PimoroniI2C

# TODO merge this in with Pimoroni Pico.
class PWMLED:
    def __init__(self, pin, invert=False, gamma=1):
        self._gamma = gamma
        self._led = PWM(Pin(pin), freq=1000, duty_u16=0, invert=invert)

    def brightness(self, brightness):
        brightness = min(1.0, max(0.0, brightness))
        self._brightness = brightness
        self._led.duty_u16(int(pow(brightness, self._gamma) * 65535 + 0.5))

    def on(self):
        self.brightness(1)

    def off(self):
        self.brightness(0)

    def toggle(self):
        self.brightness(1 - self._brightness)


# TODO merge this in with Pimoroni Pico. Could have it use PWMLED clean up the code?
class RGBLED:
    def __init__(self, r, g, b, invert=True, gamma=1):
        self._gamma = gamma
        self.led_r = PWM(Pin(r), freq=1000, duty_u16=0, invert=invert)
        self.led_g = PWM(Pin(g), freq=1000, duty_u16=0, invert=invert)
        self.led_b = PWM(Pin(b), freq=1000, duty_u16=0, invert=invert)

    def _rgb(self, r, g, b):
        self.led_r.duty_u16(int(pow(r, self._gamma) * 65535 + 0.5))
        self.led_g.duty_u16(int(pow(g, self._gamma) * 65535 + 0.5))
        self.led_b.duty_u16(int(pow(b, self._gamma) * 65535 + 0.5))

    def set_rgb(self, r, g, b):
        r = min(255, max(0, r))
        g = min(255, max(0, g))
        b = min(255, max(0, b))
        self._rgb(r / 255, g / 255, b / 255)

    def set_hsv(self, h, s, v):
        if s == 0.0:
            self._rgb(v, v, v)
        else:
            i = int(h * 6.0)
            f = (h * 6.0) - i
            p, q, t = v * (1.0 - s), v * (1.0 - s * f), v * (1.0 - s * (1.0 - f))

            i = i % 6
            if i == 0:
                self._rgb(v, t, p)
            elif i == 1:
                self._rgb(q, v, p)
            elif i == 2:
                self._rgb(p, v, t)
            elif i == 3:
                self._rgb(p, q, v)
            elif i == 4:
                self._rgb(t, p, v)
            elif i == 5:
                self._rgb(v, p, q)


class TinyFX:
    OUT_PINS = (3, 2, 4, 5, 8, 9)
    RGB_PINS = (13, 14, 15)

    I2C_SDA_PIN = 16
    I2C_SCL_PIN = 17

    I2S_DATA = 18
    I2S_BCLK = 19
    I2S_LRCLK = 20
    AMP_EN_PIN = 21

    USER_SW_PIN = 22
    SENSOR_PIN = 26
    V_SENSE_PIN = 28

    V_SENSE_GAIN = 2
    V_SENSE_DIODE_CORRECTION = 0.3

    OUTPUT_GAMMA = 2.8
    RGB_GAMMA = 2.2

    def __init__(self, init_i2c=True):
        # Set up the mono and RGB LED outputs
        self.outputs = [PWMLED(out, gamma=self.OUTPUT_GAMMA) for out in self.OUT_PINS]
        self.rgb = RGBLED(*self.RGB_PINS, invert=False, gamma=self.RGB_GAMMA)

        # Set up the i2c for Qw/st, if the user wants
        if init_i2c:
            self.i2c = PimoroniI2C(self.I2C_SDA_PIN, self.I2C_SCL_PIN, 100000)

        # Set up the user switch
        self.__switch = Pin(self.USER_SW_PIN, Pin.IN, Pin.PULL_UP)

        # Set up the internal voltage sensor
        self.__v_sense = ADC(Pin(self.V_SENSE_PIN))

    def boot_pressed(self):
        return self.__switch.value() == 0

    def voltage(self, samples=1):
        val = 0
        for _ in range(samples):
            val += self.__v_sense.read_u16()
        val /= samples

        return ((val * 3.3 * self.V_SENSE_GAIN) / 65535) + self.V_SENSE_DIODE_CORRECTION

    @property
    def one(self):
        return self.outputs[0]

    @property
    def two(self):
        return self.outputs[1]

    @property
    def three(self):
        return self.outputs[2]

    @property
    def four(self):
        return self.outputs[3]

    @property
    def five(self):
        return self.outputs[4]

    @property
    def six(self):
        return self.outputs[5]

    def clear(self):
        for out in self.outputs:
            out.off()

        self.rgb.set_rgb(0, 0, 0)
