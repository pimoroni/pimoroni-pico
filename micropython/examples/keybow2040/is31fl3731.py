# Borrows heavily from : https://github.com/adafruit/Adafruit_CircuitPython_IS31FL3731/blob/main/adafruit_is31fl3731/keybow2040.py
# and : https://github.com/adafruit/micropython-adafruit-is31fl3731/blob/master/is31fl3731.py
import math
import utime


_MODE_REGISTER = const(0x00)  # noqa: F821
_FRAME_REGISTER = const(0x01)  # noqa: F821
_AUTOPLAY1_REGISTER = const(0x02)  # noqa: F821
_AUTOPLAY2_REGISTER = const(0x03)  # noqa: F821
_BLINK_REGISTER = const(0x05)  # noqa: F821
_AUDIOSYNC_REGISTER = const(0x06)  # noqa: F821
_BREATH1_REGISTER = const(0x08)  # noqa: F821
_BREATH2_REGISTER = const(0x09)  # noqa: F821
_SHUTDOWN_REGISTER = const(0x0a)  # noqa: F821
_GAIN_REGISTER = const(0x0b)  # noqa: F821
_ADC_REGISTER = const(0x0c)  # noqa: F821

_CONFIG_BANK = const(0x0b)  # noqa: F821
_BANK_ADDRESS = const(0xfd)  # noqa: F821

_PICTURE_MODE = const(0x00)  # noqa: F821
_AUTOPLAY_MODE = const(0x08)  # noqa: F821
_AUDIOPLAY_MODE = const(0x18)  # noqa: F821

_ENABLE_OFFSET = const(0x00)  # noqa: F821
_BLINK_OFFSET = const(0x12)  # noqa: F821
_COLOR_OFFSET = const(0x24)  # noqa: F821


class Matrix:
    width = 16
    height = 9

    def __init__(self, i2c, address=0x74):
        self.i2c = i2c
        self.address = address
        self.reset()
        self.init()

    def _bank(self, bank=None):
        if bank is None:
            return self.i2c.readfrom_mem(self.address, _BANK_ADDRESS, 1)[0]
        self.i2c.writeto_mem(self.address, _BANK_ADDRESS, bytearray([bank]))

    def _register(self, bank, register, value=None):
        self._bank(bank)
        if value is None:
            return self.i2c.readfrom_mem(self.address, register, 1)[0]
        self.i2c.writeto_mem(self.address, register, bytearray([value]))

    def _mode(self, mode=None):
        return self._register(_CONFIG_BANK, _MODE_REGISTER, mode)

    def init(self):
        self._mode(_PICTURE_MODE)
        self.frame(0)
        for frame in range(8):
            self.fill(0, False, frame=frame)
            for col in range(18):
                self._register(frame, _ENABLE_OFFSET + col, 0xff)
        self.audio_sync(False)

    def reset(self):
        self.sleep(True)
        utime.sleep_us(10)
        self.sleep(False)

    def sleep(self, value):
        return self._register(_CONFIG_BANK, _SHUTDOWN_REGISTER, not value)

    def autoplay(self, delay=0, loops=0, frames=0):
        if delay == 0:
            self._mode(_PICTURE_MODE)
            return
        delay //= 11
        if not 0 <= loops <= 7:
            raise ValueError("Loops out of range")
        if not 0 <= frames <= 7:
            raise ValueError("Frames out of range")
        if not 1 <= delay <= 64:
            raise ValueError("Delay out of range")
        self._register(_CONFIG_BANK, _AUTOPLAY1_REGISTER, loops << 4 | frames)
        self._register(_CONFIG_BANK, _AUTOPLAY2_REGISTER, delay % 64)
        self._mode(_AUTOPLAY_MODE | self._frame)

    def fade(self, fade_in=None, fade_out=None, pause=0):
        if fade_in is None and fade_out is None:
            self._register(_CONFIG_BANK, _BREATH2_REGISTER, 0)
        elif fade_in is None:
            fade_in = fade_out
        elif fade_out is None:
            fade_out = fade_in
        fade_in = int(math.log(fade_in / 26, 2))
        fade_out = int(math.log(fade_out / 26, 2))
        pause = int(math.log(pause / 26, 2))
        if not 0 <= fade_in <= 7:
            raise ValueError("Fade in out of range")
        if not 0 <= fade_out <= 7:
            raise ValueError("Fade out out of range")
        if not 0 <= pause <= 7:
            raise ValueError("Pause out of range")
        self._register(_CONFIG_BANK, _BREATH1_REGISTER, fade_out << 4 | fade_in)
        self._register(_CONFIG_BANK, _BREATH2_REGISTER, 1 << 4 | pause)

    def frame(self, frame=None, show=True):
        if frame is None:
            return self._frame
        if not 0 <= frame <= 8:
            raise ValueError("Frame out of range")
        self._frame = frame
        if show:
            self._register(_CONFIG_BANK, _FRAME_REGISTER, frame)

    def audio_sync(self, value=None):
        return self._register(_CONFIG_BANK, _AUDIOSYNC_REGISTER, value)

    def audio_play(self, sample_rate, audio_gain=0,
                   agc_enable=False, agc_fast=False):
        if sample_rate == 0:
            self._mode(_PICTURE_MODE)
            return
        sample_rate //= 46
        if not 1 <= sample_rate <= 256:
            raise ValueError("Sample rate out of range")
        self._register(_CONFIG_BANK, _ADC_REGISTER, sample_rate % 256)
        audio_gain //= 3
        if not 0 <= audio_gain <= 7:
            raise ValueError("Audio gain out of range")
        self._register(_CONFIG_BANK, _GAIN_REGISTER,
                       bool(agc_enable) << 3 | bool(agc_fast) << 4 | audio_gain)
        self._mode(_AUDIOPLAY_MODE)

    def blink(self, rate=None):
        if rate is None:
            return (self._register(_CONFIG_BANK, _BLINK_REGISTER) & 0x07) * 270
        elif rate == 0:
            self._register(_CONFIG_BANK, _BLINK_REGISTER, 0x00)
            return
        rate //= 270
        self._register(_CONFIG_BANK, _BLINK_REGISTER, rate & 0x07 | 0x08)

    def fill(self, color=None, blink=None, frame=None):
        if frame is None:
            frame = self._frame
        self._bank(frame)
        if color is not None:
            if not 0 <= color <= 255:
                raise ValueError("Color out of range")
            data = bytearray([color] * 24)
            for row in range(6):
                self.i2c.writeto_mem(self.address,
                                     _COLOR_OFFSET + row * 24, data)
        if blink is not None:
            data = bool(blink) * 0xff
            for col in range(18):
                self._register(frame, _BLINK_OFFSET + col, data)

    def write_frame(self, data, frame=None):
        if len(data) > 144:
            raise ValueError("Bytearray too large for frame")
        if frame is None:
            frame = self._frame
        self._bank(frame)
        self.i2c.writeto_mem(self.address, _COLOR_OFFSET, data)

    def _pixel_addr(self, x, y):
        return x + y * 16

    def pixel(self, x, y, color=None, blink=None, frame=None):
        if not 0 <= x <= self.width:
            return
        if not 0 <= y <= self.height:
            return
        pixel = self._pixel_addr(x, y)
        if color is None and blink is None:
            return self._register(self._frame, pixel)
        if frame is None:
            frame = self._frame
        if color is not None:
            if not 0 <= color <= 255:
                raise ValueError("Color out of range")
            self._register(frame, _COLOR_OFFSET + pixel, color)
        if blink is not None:
            addr, bit = divmod(pixel, 8)
            bits = self._register(frame, _BLINK_OFFSET + addr)
            if blink:
                bits |= 1 << bit
            else:
                bits &= ~(1 << bit)
            self._register(frame, _BLINK_OFFSET + addr, bits)


class Matrix_Keybow2040(Matrix):
    width = 16
    height = 3

    def pixelrgb(self, x, y, r, g, b, blink=None, frame=None):
        """
        Blink or brightness for x, y-pixel

        :param x: horizontal pixel position
        :param y: vertical pixel position
        :param r: red brightness value 0->255
        :param g: green brightness value 0->255
        :param b: blue brightness value 0->255
        :param blink: True to blink
        :param frame: the frame to set the pixel
        """
        x = (4 * (3 - x)) + y

        super().pixel(x, 0, r, blink, frame)
        super().pixel(x, 1, g, blink, frame)
        super().pixel(x, 2, b, blink, frame)

    def _pixel_addr(self, x, y):
        lookup = [
            (120, 88, 104),  # 0, 0
            (136, 40, 72),  # 1, 0
            (112, 80, 96),  # 2, 0
            (128, 32, 64),  # 3, 0
            (121, 89, 105),  # 0, 1
            (137, 41, 73),  # 1, 1
            (113, 81, 97),  # 2, 1
            (129, 33, 65),  # 3, 1
            (122, 90, 106),  # 0, 2
            (138, 25, 74),  # 1, 2
            (114, 82, 98),  # 2, 2
            (130, 17, 66),  # 3, 2
            (123, 91, 107),  # 0, 3
            (139, 26, 75),  # 1, 3
            (115, 83, 99),  # 2, 3
            (131, 18, 67),  # 3, 3
        ]

        return lookup[x][y]
