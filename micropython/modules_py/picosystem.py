import machine
from micropython import const
from breakout_colourlcd240x240 import BreakoutColourLCD240x240
import pimoroni

# Constants and definitions
PICOSYSTEM_LCD_SPI = const(0)
PICOSYSTEM_LCD_RESET = const(4)
PICOSYSTEM_LCD_CS = const(5)
PICOSYSTEM_LCD_SCK = const(6)
PICOSYSTEM_LCD_MOSI = const(7)
PICOSYSTEM_LCD_VSYNC = const(8)
PICOSYSTEM_LCD_DC = const(9)
PICOSYSTEM_LCD_BL = const(12)

PICOSYSTEM_AUDIO = const(11)

PICOSYSTEM_A = const(18)
PICOSYSTEM_B = const(19)
PICOSYSTEM_X = const(17)
PICOSYSTEM_Y = const(16)

PICOSYSTEM_UP = const(23)
PICOSYSTEM_DOWN = const(20)
PICOSYSTEM_LEFT = const(22)
PICOSYSTEM_RIGHT = const(21)

PICOSYSTEM_LED_R = const(14)
PICOSYSTEM_LED_G = const(13)
PICOSYSTEM_LED_B = const(15)

PICOSYSTEM_BAT_SENSE = const(26)
PICOSYSTEM_CHARGE_STATUS = const(24)
PICOSYSTEM_VBUS_STATUS = const(2)


# Display dimensions
WIDTH = BreakoutColourLCD240x240.WIDTH
HEIGHT = BreakoutColourLCD240x240.HEIGHT

# PicoSystem Buttons
BUTTON_A = const(0)
BUTTON_B = const(1)
BUTTON_X = const(2)
BUTTON_Y = const(3)
DPAD_UP = const(4)
DPAD_DOWN = const(5)
DPAD_LEFT = const(6)
DPAD_RIGHT = const(7)

BUTTON_PINS = (PICOSYSTEM_A, PICOSYSTEM_B, PICOSYSTEM_X, PICOSYSTEM_Y, PICOSYSTEM_UP, PICOSYSTEM_DOWN, PICOSYSTEM_LEFT, PICOSYSTEM_RIGHT)
buttons = [machine.Pin(p, machine.Pin.IN, machine.Pin.PULL_UP) for p in BUTTON_PINS]

# PicoSystem Audio
audio_pin = machine.Pin(PICOSYSTEM_AUDIO)
buzzer = machine.PWM(audio_pin)
buzzer.duty_u16(0)


def buzz(freq, duty=0.5):
    if freq < 50.0:  # uh... https://github.com/micropython/micropython/blob/af64c2ddbd758ab6bac0fcca94c66d89046663be/ports/rp2/machine_pwm.c#L105-L119
        buzzer.duty_u16(0)
    else:
        buzzer.freq(freq)
        buzzer.duty_u16(int(65535 * duty))


# PicoSystem Status
battery_sense = pimoroni.Analog(PICOSYSTEM_BAT_SENSE, amplifier_gain=(1.0 / 3.0))
charge_status = machine.Pin(PICOSYSTEM_CHARGE_STATUS, machine.Pin.IN)
vbus_status = machine.Pin(PICOSYSTEM_VBUS_STATUS, machine.Pin.IN)

# PicoSystem Display
# Bring the LCD out of reset
lcd_reset = machine.Pin(PICOSYSTEM_LCD_RESET, machine.Pin.OUT)
lcd_reset.value(1)

display_buffer = bytearray(WIDTH * HEIGHT * 2)  # 2-bytes per pixel (RGB565)
display = BreakoutColourLCD240x240(
    display_buffer,
    PICOSYSTEM_LCD_SPI,
    PICOSYSTEM_LCD_CS,
    PICOSYSTEM_LCD_DC,
    PICOSYSTEM_LCD_SCK,
    PICOSYSTEM_LCD_MOSI,
    PICOSYSTEM_LCD_BL)

display.update()
display.set_backlight(1.0)

# PicoSystem LED
led = pimoroni.RGBLED(PICOSYSTEM_LED_R, PICOSYSTEM_LED_G, PICOSYSTEM_LED_B, invert=False)
led.set_rgb(0, 0, 0)
