import gc
from machine import Pin, PWM
from pimoroni_i2c import PimoroniI2C
from plasma import WS2812
from motor import Motor
from servo import Servo
from encoder import Encoder, MMME_CPR

# IO Pin Constants
GP0 = 0
GP1 = 1
GP2 = 2

A0 = 26
A1 = 27
A2 = 28

GPIOS = (GP0, GP1, GP2, A0, A1, A2)
ADCS = (A0, A1, A2)


# Index Constants
MOTOR_A = 0
MOTOR_B = 1

SERVO_1 = 0
SERVO_2 = 1
SERVO_3 = 2
SERVO_4 = 3
SERVO_5 = 4
SERVO_6 = 5

LED_GP0 = 0
LED_GP1 = 1
LED_GP2 = 2
LED_A0 = 3
LED_A1 = 4
LED_A2 = 5
LED_SERVO_1 = 6
LED_SERVO_2 = 7
LED_SERVO_3 = 8
LED_SERVO_4 = 9
LED_SERVO_5 = 10
LED_SERVO_6 = 11


# Count Constants
NUM_GPIOS = 6
NUM_ADCS = 3
NUM_MOTORS = 2
NUM_SERVOS = 6
NUM_LEDS = 12


class Inventor2040W():
    AMP_EN_PIN = 3
    I2C_SDA_PIN = 4
    I2C_SCL_PIN = 5
    MOTOR_A_PINS = (6, 7)
    MOTOR_B_PINS = (8, 9)
    ENCODER_A_PINS = (19, 18)
    ENCODER_B_PINS = (17, 16)

    SERVO_1_PIN = 10
    SERVO_2_PIN = 11
    SERVO_3_PIN = 12
    SERVO_4_PIN = 13
    SERVO_5_PIN = 14
    SERVO_6_PIN = 15

    LED_DATA_PIN = 20
    PWM_AUDIO_PIN = 21
    USER_SW_PIN = 22

    AMP_CORRECTION = 4
    DEFAULT_VOLUME = 0.2

    def __init__(self, motor_gear_ratio=50, init_motors=True, init_servos=True):
        # Free up hardware resources
        gc.collect()

        # Set up the motors and encoders, if the user wants them
        self.motors = None
        if init_motors:
            cpr = MMME_CPR * motor_gear_ratio
            self.motors = [Motor(self.MOTOR_A_PINS), Motor(self.MOTOR_B_PINS)]
            # Set the encoders to use PIO 0 and State Machines 0 and 1
            self.encoders = [Encoder(0, 0, self.ENCODER_A_PINS, counts_per_rev=cpr, count_microsteps=True),
                             Encoder(0, 1, self.ENCODER_B_PINS, counts_per_rev=cpr, count_microsteps=True)]

        # Set up the servos, if the user wants them
        self.servos = None
        if init_servos:
            self.servos = [Servo(i) for i in range(self.SERVO_1_PIN, self.SERVO_6_PIN + 1)]

        # Set up the i2c for Qw/st and Breakout Garden
        self.i2c = PimoroniI2C(self.I2C_SDA_PIN, self.I2C_SCL_PIN, 100000)

        # Set up the amp enable
        self.__amp_en = Pin(self.AMP_EN_PIN, Pin.OUT)
        self.__amp_en.off()

        self.audio_pwm = PWM(Pin(self.PWM_AUDIO_PIN))
        self.__volume = self.DEFAULT_VOLUME

        # Set up the user switch
        self.__switch = Pin(self.USER_SW_PIN, Pin.IN, Pin.PULL_DOWN)

        # Set up the WS2812 LEDs, using PIO 0 and State Machine 2
        self.leds = WS2812(NUM_LEDS, 0, 2, self.LED_DATA_PIN)
        self.leds.start()

    def switch_pressed(self):
        return self.__switch.value()

    def play_tone(self, frequency):
        try:
            self.audio_pwm.freq(frequency)
        except ValueError:
            self.play_silence()
            raise ValueError("frequency of range. Expected greater than 0")

        corrected_volume = (self.__volume ** 4)  # Correct for RC Filter curve
        self.audio_pwm.duty_u16(int(32768 * corrected_volume))
        self.unmute_audio()

    def play_silence(self):
        self.audio_pwm.freq(44100)

        corrected_volume = (self.__volume ** 4)  # Correct for RC Filter curve
        self.audio_pwm.duty_u16(int(32768 * corrected_volume))
        self.unmute_audio()

    def stop_playing(self):
        self.audio_pwm.duty_u16(0)
        self.mute_audio()

    def volume(self, volume=None):
        if volume is None:
            return self.__volume

        if volume < 0.01 or volume > 1.0:
            raise ValueError("volume out of range. Expected 0.0 to 1.0")

        self.__volume = volume

    def mute_audio(self):
        self.__amp_en.off()

    def unmute_audio(self):
        self.__amp_en.on()
