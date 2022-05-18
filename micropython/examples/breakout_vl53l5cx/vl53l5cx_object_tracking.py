import pimoroni_i2c
import breakout_vl53l5cx
import time
from ulab import numpy

# The VL53L5CX requires a firmware blob to start up.
# Make sure you upload "vl53l5cx_firmware.bin" via Thonny to the root of your filesystem
# You can find it here: https://github.com/ST-mirror/VL53L5CX_ULD_driver/blob/no-fw/lite/en/vl53l5cx_firmware.bin

# This example attempts to track a "bright" object (such as a white business card)
# It uses reflectance to identify the target and compute the X/Y coordinates
# of its "center of mass" in the sensors view.

# Motion indication only works at distances > 400mm so it's not
# really useful as a method to reject data.

# Configure your distance and brightness thresholds to suit your object
DISTANCE_THRESHOLD = 400  # Distance in mm
REFLECTANCE_THRESHOLD = 60  # Estimated reflectance in %


PINS_BREAKOUT_GARDEN = {"sda": 4, "scl": 5}
PINS_PICO_EXPLORER = {"sda": 20, "scl": 21}

# Sensor startup time is proportional to i2c baudrate
# HOWEVER many sensors may not run at > 400KHz (400000)
i2c = pimoroni_i2c.PimoroniI2C(**PINS_BREAKOUT_GARDEN, baudrate=2_000_000)

print("Starting up sensor...")
t_sta = time.ticks_ms()
sensor = breakout_vl53l5cx.VL53L5CX(i2c)
t_end = time.ticks_ms()
print("Done in {}ms...".format(t_end - t_sta))

# Make sure to set resolution and other settings *before* you start ranging
sensor.set_resolution(breakout_vl53l5cx.RESOLUTION_8X8)
sensor.set_ranging_frequency_hz(15)
sensor.start_ranging()


while True:
    time.sleep(1.0 / 60)
    if sensor.data_ready():
        # "data" is a namedtuple (attrtuple technically)
        # it includes average readings as "distance_avg" and "reflectance_avg"
        # plus a full 4x4 or 8x8 set of readings (as a 1d tuple) for both values.
        data = sensor.get_data()

        reflectance = numpy.array(data.reflectance).reshape((8, 8))
        distance = numpy.array(data.distance).reshape((8, 8))

        scalar = 0
        target_distance = 0
        n_distances = 0
        # Filter out unwanted reflectance values
        for ox in range(8):
            for oy in range(8):
                d = distance[ox][oy]
                r = reflectance[ox][oy]
                if d > DISTANCE_THRESHOLD or r < REFLECTANCE_THRESHOLD:
                    reflectance[ox][oy] = 0
                else:
                    scalar += r

        # Get a total from all the distances within our accepted target
        for ox in range(8):
            for oy in range(8):
                d = distance[ox][oy]
                r = reflectance[ox][oy]
                if r > 0:
                    target_distance += d
                    n_distances += 1

        # Average the target distance
        if n_distances > 0:
            target_distance /= n_distances
        else:
            target_distance = 0

        # Flip reflectance now we've applied distance
        # both fields are upside-down!
        reflectance = numpy.flip(reflectance, axis=0)

        # Calculate the center of mass along X and Y
        x = 0
        y = 0
        if scalar > 0:
            for ox in range(8):
                for oy in range(8):
                    y += reflectance[ox][oy] * ox
            y /= scalar
            y /= 3.5
            y -= 1.0

            for oy in range(8):
                for ox in range(8):
                    x += reflectance[ox][oy] * oy
            x /= scalar
            x /= 3.5
            x -= 1.0

            print(round(x, 2), round(y, 2), round(target_distance, 2))
