import badger2040
from machine import Pin, ADC
import time

# Global Constants
# for e.g. 2xAAA batteries, try max 3.4 min 3.0
MAX_BATTERY_VOLTAGE = 4.0
MIN_BATTERY_VOLTAGE = 3.2

WIDTH = badger2040.WIDTH
HEIGHT = badger2040.HEIGHT

BATT_WIDTH = 200
BATT_HEIGHT = 100
BATT_BORDER = 10
BATT_TERM_WIDTH = 20
BATT_TERM_HEIGHT = 50
BATT_BAR_PADDING = 10
BATT_BAR_HEIGHT = BATT_HEIGHT - (BATT_BORDER * 2) - (BATT_BAR_PADDING * 2)
BATT_BAR_START = ((WIDTH - BATT_WIDTH) // 2) + BATT_BORDER + BATT_BAR_PADDING
BATT_BAR_END = ((WIDTH + BATT_WIDTH) // 2) - BATT_BORDER - BATT_BAR_PADDING

NUM_BATT_BARS = 4


# ------------------------------
#      Utility functions
# ------------------------------


def map_value(input, in_min, in_max, out_min, out_max):
    return (((input - in_min) * (out_max - out_min)) / (in_max - in_min)) + out_min


# ------------------------------
#      Drawing functions
# ------------------------------

# Draw the frame of the reader
def draw_battery(level, resolution):
    display.pen(15)
    display.clear()

    display.thickness(1)

    # Draw the battery outline
    display.pen(0)
    display.rectangle(
        (WIDTH - BATT_WIDTH) // 2, (HEIGHT - BATT_HEIGHT) // 2, BATT_WIDTH, BATT_HEIGHT
    )

    display.rectangle(
        (WIDTH + BATT_WIDTH) // 2,
        (HEIGHT - BATT_TERM_HEIGHT) // 2,
        BATT_TERM_WIDTH,
        BATT_TERM_HEIGHT,
    )

    display.pen(15)
    display.rectangle(
        (WIDTH - BATT_WIDTH) // 2 + BATT_BORDER,
        (HEIGHT - BATT_HEIGHT) // 2 + BATT_BORDER,
        BATT_WIDTH - BATT_BORDER * 2,
        BATT_HEIGHT - BATT_BORDER * 2,
    )

    # Add a special check for no battery
    if level < 1:
        X = WIDTH // 2
        Y = HEIGHT // 2

        display.pen(0)
        display.thickness(1)
        thickness = (BATT_BORDER * 3) // 2
        start_extra = thickness // 3
        end_extra = (thickness * 2) // 3
        for i in range(0, thickness):
            excess = i // 2
            display.line(
                X - (BATT_HEIGHT // 2) + i - excess - start_extra,
                Y - (BATT_HEIGHT // 2) - excess - start_extra,
                X + (BATT_HEIGHT // 2) + i - excess + end_extra,
                Y + (BATT_HEIGHT // 2) - excess + end_extra,
            )
        display.pen(15)
        for i in range(0 - thickness, 0):
            display.line(
                X - (BATT_HEIGHT // 2) + i,
                Y - (BATT_HEIGHT // 2),
                X + (BATT_HEIGHT // 2) + i,
                Y + (BATT_HEIGHT // 2),
            )
    else:
        # Draw the battery bars
        display.pen(0)
        length = (
            BATT_BAR_END - BATT_BAR_START - ((NUM_BATT_BARS - 1) * BATT_BAR_PADDING)
        ) // NUM_BATT_BARS
        current_level = 0.0
        normalised_level = level / resolution
        for i in range(NUM_BATT_BARS):
            current_level = (1.0 * i) / NUM_BATT_BARS
            if normalised_level > current_level:
                pos = i * (length + BATT_BAR_PADDING)
                display.rectangle(
                    BATT_BAR_START + pos,
                    (HEIGHT - BATT_BAR_HEIGHT) // 2,
                    length,
                    BATT_BAR_HEIGHT,
                )

    display.update()


# ------------------------------
#        Program setup
# ------------------------------

# Create a new Badger and set it to update FAST
display = badger2040.Badger2040()
display.update_speed(badger2040.UPDATE_FAST)

# Set up the ADCs for measuring battery voltage
vbat_adc = ADC(badger2040.PIN_BATTERY)
vref_adc = ADC(badger2040.PIN_1V2_REF)
vref_en = Pin(badger2040.PIN_VREF_POWER)
vref_en.init(Pin.OUT)
vref_en.value(0)


# ------------------------------
#       Main program loop
# ------------------------------

last_level = -1

while True:
    # Enable the onboard voltage reference
    vref_en.value(1)

    # Calculate the logic supply voltage, as will be lower that the usual 3.3V when running off low batteries
    vdd = 1.24 * (65535 / vref_adc.read_u16())
    vbat = (
        (vbat_adc.read_u16() / 65535) * 3 * vdd
    )  # 3 in this is a gain, not rounding of 3.3V

    # Disable the onboard voltage reference
    vref_en.value(0)

    # Print out the voltage
    print("Battery Voltage = ", vbat, "V", sep="")

    # Convert the voltage to a level to display onscreen
    level = int(
        map_value(vbat, MIN_BATTERY_VOLTAGE, MAX_BATTERY_VOLTAGE, 0, NUM_BATT_BARS)
    )

    # Only draw if the battery level has changed significantly
    if level != last_level:
        draw_battery(level, NUM_BATT_BARS)
        last_level = level

    time.sleep(1)
