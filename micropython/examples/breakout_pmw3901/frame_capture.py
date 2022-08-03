import time

# Pick *one* sensor type by uncommenting the relevant line below:

# PMW3901
from breakout_pmw3901 import BreakoutPMW3901 as FlowSensor

# PAA5100
# from breakout_paa5100 import BreakoutPAA5100 as FlowSensor

flo = FlowSensor()

rotation = FlowSensor.DEGREES_0
SIZE = FlowSensor.FRAME_SIZE
BYTES = FlowSensor.FRAME_BYTES
data = bytearray(BYTES)

flo.set_rotation(rotation)

offset = 0
value = 0


def value_to_char(value):
    charmap = " .:-=+*#%@"
    val = float(value) / 255.0
    val *= len(charmap) - 1
    chosen_char = charmap[int(val)]
    return chosen_char * 2  # Double chars to - sort of - correct aspect ratio


while True:
    print("Capturing...")
    time.sleep(0.1)

    # Warning! The frame capture function below can take up to 10 seconds to run! Also, it often fails to capture all bytes.
    # A shorter timeout (in seconds) can be set with the 'timeout' keyword e.g. frame_capture(data, timeout=6.0)
    data_size = flo.frame_capture(data)
    if data_size == BYTES:
        for y in range(0, SIZE):
            if rotation == FlowSensor.DEGREES_180 or rotation == FlowSensor.DEGREES_270:
                y = SIZE - y - 1

            for x in range(0, SIZE):
                if rotation == FlowSensor.DEGREES_180 or rotation == FlowSensor.DEGREES_90:
                    x = SIZE - x - 1

                if rotation == FlowSensor.DEGREES_90 or rotation == FlowSensor.DEGREES_270:
                    offset = (x * 35) + y
                else:
                    offset = (y * 35) + x

                value = data[offset]
                print(value_to_char(value), end="")
            print()
    else:
        print("Capture failed. {} bytes received, of {}. Recapturing in ".format(data_size, BYTES))

    print("5...")
    time.sleep(1.0)
    print("4...")
    time.sleep(1.0)
    print("3...")
    time.sleep(1.0)
    print("2...")
    time.sleep(1.0)
    print("Get Ready!")
    time.sleep(1.0)
