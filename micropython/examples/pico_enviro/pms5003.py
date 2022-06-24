# MIT License

# Copyright (c) 2018 Pimoroni Ltd.
# Copyright (c) 2020 Kevin J. Walters
# Copyright (c) 2020 Erik Hess

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


import ustruct as struct
import time

import machine


__version__ = '0.0.7'


PMS5003_SOF = bytearray(b'\x42\x4d')
PMS5003_CMD_MODE_PASSIVE = b'\xe1\x00\x00'
PMS5003_CMD_MODE_ACTIVE = b'\xe1\x00\x01'
PMS5003_CMD_READ = b'\xe2\x00\x00'
PMS5003_CMD_SLEEP = b'\xe4\x00\x00'
PMS5003_CMD_WAKEUP = b'\xe4\x00\x01'


class ChecksumMismatchError(RuntimeError):
    pass


class FrameLengthError(RuntimeError):
    pass


class ReadTimeoutError(RuntimeError):
    pass


class SerialTimeoutError(RuntimeError):
    pass


class PMS5003Response:
    FRAME_LEN = None
    DATA_LEN = None
    DATA_FMT = None
    CHECKSUM_IDX = None

    @classmethod
    def check_data_len(cls, raw_data_len, desc="Data"):
        if raw_data_len != cls.DATA_LEN:
            raise FrameLengthError(desc + " too "
                                   + ("short" if raw_data_len < cls.DATA_LEN else "long")
                                   + " {:d} bytes".format(raw_data_len))

    def __init__(self, raw_data, *, frame_length_bytes):
        raw_data_len = len(raw_data)
        self.check_data_len(raw_data_len)
        self.raw_data = raw_data
        self.data = struct.unpack(self.DATA_FMT, raw_data)
        self.checksum = self.data[self.CHECKSUM_IDX]

        # Don't include the checksum bytes in the checksum calculation
        checksum = sum(PMS5003_SOF) + sum(raw_data[:-2])
        if frame_length_bytes is None:
            checksum += (raw_data_len >> 256) + (raw_data_len & 0xff)
        else:
            checksum += sum(frame_length_bytes)
        if checksum != self.checksum:
            raise ChecksumMismatchError("PMS5003 Checksum Mismatch {} != {}".format(checksum,
                                                                                    self.checksum))


class PMS5003CmdResponse(PMS5003Response):
    FRAME_LEN = 8
    DATA_LEN = FRAME_LEN - 4  # includes checksum
    DATA_FMT = ">BBH"
    CHECKSUM_IDX = 2

    def __init__(self, raw_data, *, frame_length_bytes=None):
        super().__init__(raw_data, frame_length_bytes=frame_length_bytes)


class PMS5003Data(PMS5003Response):
    FRAME_LEN = 32
    DATA_LEN = FRAME_LEN - 4  # includes checksum
    DATA_FMT = ">HHHHHHHHHHHHHH"
    CHECKSUM_IDX = 13

    def __init__(self, raw_data, *, frame_length_bytes=None):
        super().__init__(raw_data, frame_length_bytes=frame_length_bytes)

    def pm_ug_per_m3(self, size, atmospheric_environment=False):
        if atmospheric_environment:
            if size == 1.0:
                return self.data[3]
            if size == 2.5:
                return self.data[4]
            if size is None:
                return self.data[5]

        else:
            if size == 1.0:
                return self.data[0]
            if size == 2.5:
                return self.data[1]
            if size == 10:
                return self.data[2]

        raise ValueError("Particle size {} measurement not available.".format(size))

    def pm_per_1l_air(self, size):
        if size == 0.3:
            return self.data[6]
        if size == 0.5:
            return self.data[7]
        if size == 1.0:
            return self.data[8]
        if size == 2.5:
            return self.data[9]
        if size == 5:
            return self.data[10]
        if size == 10:
            return self.data[11]

        raise ValueError("Particle size {} measurement not available.".format(size))

    def __repr__(self):
        return """
PM1.0 ug/m3 (ultrafine particles):                             {}
PM2.5 ug/m3 (combustion particles, organic compounds, metals): {}
PM10 ug/m3  (dust, pollen, mould spores):                      {}
PM1.0 ug/m3 (atmos env):                                       {}
PM2.5 ug/m3 (atmos env):                                       {}
PM10 ug/m3 (atmos env):                                        {}
>0.3um in 0.1L air:                                            {}
>0.5um in 0.1L air:                                            {}
>1.0um in 0.1L air:                                            {}
>2.5um in 0.1L air:                                            {}
>5.0um in 0.1L air:                                            {}
>10um in 0.1L air:                                             {}
""".format(*self.data[:-2], checksum=self.checksum)

    def __str__(self):
        return self.__repr__()


class PMS5003():
    MAX_RESET_TIME = 20_000  # 9.2 seconds seen in testing
    MAX_RESP_TIME = 5_000
    MIN_CMD_INTERVAL = 0.1   # mode changes with interval < 50ms break a PMS5003

    @staticmethod
    def _build_cmd_frame(cmd_bytes):
        """
        Builds a valid command frame byte array with checksum for given command bytes
        """
        if len(cmd_bytes) != 3:
            raise RuntimeError("Malformed command frame")
        cmd_frame = bytearray()
        cmd_frame.extend(PMS5003_SOF)
        cmd_frame.extend(cmd_bytes)
        cmd_frame.extend(sum(cmd_frame).to_bytes(2, "big"))
        return cmd_frame


    def __init__(self,
                 uart,
                 pin_reset,
                 pin_enable,
                 mode='active',
                 retries=5
                 ):
        self._uart = uart
        self._mode = 'active'  # device starts up in active mode

        self._pin_enable = pin_enable
        self._pin_reset = pin_reset
        self._attempts = retries + 1 if retries else 1

        if mode not in ('active', 'passive'):
            raise ValueError("Invalid mode")

        if self._pin_enable:
            self._pin_enable.init(machine.Pin.OPEN_DRAIN)
            self._pin_enable.value(1)

        if self._pin_reset:
            self._pin_reset.init(machine.Pin.OUT)
            self._pin_reset.value(1)

        self.reset()

        if mode == 'passive':
            self.cmd_mode_passive()

    def cmd_mode_passive(self):
        """
        Sends command to device to enable 'passive' mode.
        In passive mode data frames are only sent in response to
        a read command.
        """
        self._mode = 'passive'

        time.sleep(self.MIN_CMD_INTERVAL)
        self._reset_input_buffer()
        self._uart.write(self._build_cmd_frame(PMS5003_CMD_MODE_PASSIVE))
        # In rare cases a single data frame sneaks in giving FrameLengthError
        try:
            resp = self._read_data(PMS5003CmdResponse)
        except FrameLengthError:
            resp = self._read_data(PMS5003CmdResponse)
        time.sleep(self.MIN_CMD_INTERVAL)
        return resp

    def cmd_mode_active(self):
        """
        Sends command to device to enable 'active' mode.
        In active mode data frames are streamed continuously at intervals
        ranging from 200ms to 2.3 seconds.
        """
        self._mode = 'active'
        # mode changes with interval < 50ms break on a PMS5003
        time.sleep(self.MIN_CMD_INTERVAL)
        self._reset_input_buffer()
        self._uart.write(self._build_cmd_frame(PMS5003_CMD_MODE_ACTIVE))
        # In rare cases a single data frame sneaks in giving FrameLengthError
        try:
            resp = self._read_data(PMS5003CmdResponse)
        except FrameLengthError:
            resp = self._read_data(PMS5003CmdResponse)
        time.sleep(self.MIN_CMD_INTERVAL)
        return resp
        
    def _reset_input_buffer(self):
        while self._uart.read() is not None:
            pass

    def reset(self):
        """This resets the device via a pin if one is defined.
           It restores passive mode as necessary."""
        if self._pin_reset is None:
            return False

        time.sleep(0.1)
        self._pin_reset.value(0)
        self._reset_input_buffer()
        time.sleep(0.1)
        self._pin_reset.value(1)

        # Wait for first data frame from the device
        # CircuitPython 6.0.0 on nRF52840 sometimes picks up 2 bogus bytes here
        start = time.ticks_ms()
        while True:
            if self.data_available():
                break
            elapsed = time.ticks_ms() - start
            if elapsed > self.MAX_RESET_TIME:
                raise ReadTimeoutError("PMS5003 Read Timeout: No response after reset")

        # After a reset device will be in active mode, restore passive mode
        if self._mode == "passive":
            self._reset_input_buffer()
            self.cmd_mode_passive()

        return True

    def data_available(self):
        """Returns boolean indicating if one or more data frames are waiting.
           Only for use in active mode."""
        return self._uart.any() >= PMS5003Data.FRAME_LEN

    def read(self):
        """Read a data frame. In passive mode this will transmit a request for one.
           This will make additional attempts based on retries value in constructor
           if there are exceptions and only raise the first exception if all fail."""
        read_ex = None
        for _ in range(self._attempts):
            if self._mode == 'passive':
                self._cmd_passive_read()
            try:
                return self._read_data()
            except RuntimeError as ex:
                if read_ex is None:
                    read_ex = ex
        raise read_ex if read_ex else RuntimeError("read failed - internal error")

    def _wait_for_bytes(self, num_bytes, timeout=MAX_RESP_TIME):
        start = time.ticks_ms()
        while self._uart.any() < num_bytes:
            elapsed = time.ticks_ms() - start
            if elapsed > timeout:
                raise ReadTimeoutError("PMS5003 Read Timeout: Waiting for {} bytes!".format(num_bytes))

    def _read_data(self, response_class=PMS5003Data):
        sof_index = 0

        while True:
            self._wait_for_bytes(1)

            one_byte = self._uart.read(1)
            if one_byte is None or len(one_byte) == 0:
                continue

            if ord(one_byte) == PMS5003_SOF[sof_index]:
                if sof_index == 0:
                    sof_index = 1
                elif sof_index == 1:
                    break
            else:
                sof_index = 0

        self._wait_for_bytes(2)

        len_data = self._uart.read(2)  # Get frame length packet
        frame_length = struct.unpack(">H", len_data)[0]
        response_class.check_data_len(frame_length, desc="Length field")

        self._wait_for_bytes(frame_length)

        raw_data = self._uart.read(frame_length)
        return response_class(raw_data, frame_length_bytes=len_data)

    def _cmd_passive_read(self):
        """
        Sends command to request a data frame while in 'passive'
        mode and immediately reads in frame.
        """
        self._reset_input_buffer()
        self._uart.write(self._build_cmd_frame(PMS5003_CMD_READ))
