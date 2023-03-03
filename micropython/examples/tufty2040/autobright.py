# Automatic brightness example.

import time
from machine import ADC, Pin
from pimoroni import Button
from picographics import PicoGraphics, DISPLAY_TUFTY_2040
import micropython

# Constants for automatic brightness adjustment.
# Below about 3/8ths, the backlight is entirely off. The top of the range is OK.
BACKLIGHT_LOW = micropython.const(0.375)
BACKLIGHT_HIGH = micropython.const(1.0)

# The luminance sensor seems to cover the whole 16-bit range pretty well from
# buried in shadow to bright phone torch in its face, but setting a lower high
# point will make it generally bias brighter in calm room lighting, and a higher
# low will use the darkest backlight without needing to completely hard-mask the
# sensor in complete darkness.
LUMINANCE_LOW = micropython.const(384)
LUMINANCE_HIGH = micropython.const(2048)  # 65535 to use the full range.

# If on battery, and the supply voltage drops below this, force minimum
# backlight brightness.
# The bottom of the Tufty2040 input range is 3.0v, so values below that likely
# will not trigger before it cuts out.
LOW_BATTERY_VOLTAGE = micropython.const(3.1)

# Hold button A to pretend the LUX sensor is seeing maximally bright light.
# (To test darkness, put your finger over the sensor! :D )
button_a = Button(7, invert=False)
# Hold button B to pretend the system is on battery, and the battery is low.
button_b = Button(8, invert=False)
# Pins and analogue-digital converters we need to set up to measure sensors.
lux_vref_pwr = Pin(27, Pin.OUT)
lux = ADC(26)
vbat_adc = ADC(29)
vref_adc = ADC(28)
usb_power = Pin(24, Pin.IN)

display = PicoGraphics(display=DISPLAY_TUFTY_2040)
display.set_backlight(1.0)
display.set_font("bitmap8")
WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)
WIDTH, HEIGHT = display.get_bounds()


# Returns a tuple of the raw luminance value, and the brightness to now use.
def auto_brightness(previous: float) -> (float, float):
    luminance = lux.read_u16()
    if button_a.is_pressed:  # Debug key.
        luminance = 65535
    luminance_frac = max(0.0, float(luminance - LUMINANCE_LOW))
    luminance_frac = min(1.0, luminance_frac / (LUMINANCE_HIGH - LUMINANCE_LOW))
    backlight = BACKLIGHT_LOW + (luminance_frac * (BACKLIGHT_HIGH - BACKLIGHT_LOW))
    # Use the previous value to smooth out changes to reduce flickering.
    # The "32" value here controls how quickly it reacts (larger = slower).
    # The rate at which the main loop calls us also affects that!
    backlight_diff = backlight - previous
    backlight = previous + (backlight_diff * (1.0 / 32.0))
    return (luminance, backlight)


# Returns a tuple of voltage (fake value if on USB), "is on USB", and "is low".
def measure_battery() -> (float, bool, bool):
    if button_b.is_pressed:  # Debug key.
        return (3.0, False, True)

    if usb_power.value():
        return (5.0, True, False)

    # See the battery.py example for how this works.
    vdd = 1.24 * (65535 / vref_adc.read_u16())
    vbat = ((vbat_adc.read_u16() / 65535) * 3 * vdd)

    low_battery = False
    if vbat < LOW_BATTERY_VOLTAGE:
        low_battery = True
    return (vbat, False, low_battery)


backlight = BACKLIGHT_LOW
while True:
    # Turn on VREF and LUX only while we measure things.
    lux_vref_pwr.value(1)
    (vbat, on_usb, low_battery) = measure_battery()
    if low_battery:
        backlight = BACKLIGHT_LOW
    else:
        (luminance, backlight) = auto_brightness(backlight)
    lux_vref_pwr.value(0)

    # Set the new backlight value.
    display.set_backlight(backlight)

    # Show our measurements.
    display.set_pen(WHITE)
    display.clear()
    display.set_pen(BLACK)
    display.text(f"Backlight: {backlight * 100:03.0f}%", 8, 8, WIDTH - 8, 4)

    if low_battery:
        display.text("Luminance: Ignored, low battery!", 8, 48 + (20 * 1), WIDTH - 8, 2)
    else:
        display.text(f"Luminance: {luminance:05.0f} (of 65535)", 8, 48 + (20 * 1), WIDTH - 8, 2)
    if on_usb:
        display.text("Battery: Ignored, on USB.", 8, 48 + (20 * 2), WIDTH - 8, 2)
    else:
        display.text(f"Battery: {vbat:.2f}v", 8, 48 + (20 * 2), WIDTH - 8, 2)

    display.text("Hold A to pretend the room is bright.", 8, HEIGHT - (20 * 4), WIDTH - 8, 2)
    display.text("Hold B to pretend the battery is low.", 8, HEIGHT - (20 * 2), WIDTH - 8, 2)
    display.update()

    time.sleep(0.1)
