from machine import Pin, PWM
from pimoroni_i2c import PimoroniI2C
from pcf85063a import PCF85063A


##################################################
# Enviro Base
##################################################

CELCIUS = 0
FAHRENHEIT = 1
KELVIN = 2

HECTOPASCAL = 0
INHG = 1
PSI = 2
BAR = 3


class EnviroBase():
    HOLD_VSYS_EN_PIN = 2
    I2C_INTERRUPT_PIN = 3
    I2C_SDA_PIN = 4
    I2C_SCL_PIN = 5
    ACTIVITY_LED_PIN = 6
    SWITCH_PIN = 7
    RTC_ALARM_PIN = 8

    # Warning LED Constants
    WARN_LED_OFF = 0
    WARN_LED_ON = 1
    WARN_LED_BLINK = 2

    # Wake Reason Constants
    REASON_SWITCH_PRESSED = 0
    REASON_RTC_ALARM = 1
    REASON_CAMERA_TRIGGER = 2
    REASON_RAIN_TRIGGER = 3
    REASON_NONE = -1

    def __init__(self):
        self.i2c = PimoroniI2C(self.I2C_SDA_PIN, self.I2C_SCL_PIN, 100000)

        # Set up and enable vsys hold so we don't go to sleep
        self.__hold_vsys_en = Pin(self.HOLD_VSYS_EN_PIN, Pin.OUT)
        self.__hold_vsys_en.value(True)

        # Set up the activity led
        self.__activity_led_pwm = PWM(Pin(self.ACTIVITY_LED_PIN))
        self.__activity_led_pwm.freq(1000)
        self.__activity_led_pwm.duty_u16(0)

        # Set up the switch pin
        self.__switch = Pin(self.SWITCH_PIN, Pin.IN, Pin.PULL_DOWN)

        # Set up the rtc alarm pin
        self.__rtc_alarm = Pin(self.RTC_ALARM_PIN, Pin.IN, Pin.PULL_DOWN)

        # Find out what we were woken by (if anything)
        self.__woken_up_by = self.REASON_NONE
        if self.__switch.value():
            self.__woken_up_by = self.REASON_SWITCH_PRESSED
        if self.__rtc_alarm.value():
            self.__woken_up_by = self.REASON_RTC_ALARM

        # Init the RTC
        self.rtc = PCF85063A(self.i2c)

        # The pcf85063a defaults to 32KHz clock output so
        # we need to explicitly turn that off by default
        self.warn_led(EnviroBase.WARN_LED_OFF)

    def sleep(self, seconds=-1):
        if seconds != -1:
            # Tell the RTC to wake us up once the number of seconds has passed
            self.rtc.enable_timer_interrupt(True)
            self.rtc.set_timer(seconds)

        # Disable the vsys hold, causing us to turn off
        self.__hold_vsys_en.init(Pin.IN)

    def activity_led(self, brightness):
        if brightness is True:
            self.__activity_led_pwm.duty_u16(65535)
        elif brightness is False:
            self.__activity_led_pwm.duty_u16(0)
        elif brightness < 0.0 or brightness > 100.0:
            raise ValueError("brightness out of range. Expected 0 to 100, or True or False")
        else:
            gamma = 2.8
            value = int(pow(brightness / 100.0, gamma) * 65535 + 0.5)
            self.__activity_led_pwm.duty_u16(value)

    def warn_led(self, wls):
        if wls == EnviroBase.WARN_LED_OFF:
            self.rtc.set_clock_output(PCF85063A.CLOCK_OUT_OFF)
            return

        if wls == EnviroBase.WARN_LED_ON:
            self.rtc.set_clock_output(PCF85063A.CLOCK_OUT_1024HZ)
            return

        if wls == EnviroBase.WARN_LED_BLINK:
            self.rtc.set_clock_output(PCF85063A.CLOCK_OUT_1HZ)
            return

    def switch_pressed(self):
        return self.__switch.value()

    def wake_reason(self, clear=False):
        result = self.__woken_up_by
        if clear:
            self.__woken_up_by = self.REASON_NONE
        return result
