import time
from inventor import Inventor2040W, MOTOR_A, MOTOR_B
from motor import SLOW_DECAY  # , FAST_DECAY

"""
A fun example of how to change a motor's frequency to have it play a song.

It uses code written by Avram Piltch - check out his Tom's Hardware article!
https://www.tomshardware.com/uk/how-to/buzzer-music-raspberry-pi-pico

Press "User" to start or stop the song.
"""

# This handy dictionary converts notes into frequencies
TONES = {
    "B0": 31,
    "C1": 33,
    "CS1": 35,
    "D1": 37,
    "DS1": 39,
    "E1": 41,
    "F1": 44,
    "FS1": 46,
    "G1": 49,
    "GS1": 52,
    "A1": 55,
    "AS1": 58,
    "B1": 62,
    "C2": 65,
    "CS2": 69,
    "D2": 73,
    "DS2": 78,
    "E2": 82,
    "F2": 87,
    "FS2": 93,
    "G2": 98,
    "GS2": 104,
    "A2": 110,
    "AS2": 117,
    "B2": 123,
    "C3": 131,
    "CS3": 139,
    "D3": 147,
    "DS3": 156,
    "E3": 165,
    "F3": 175,
    "FS3": 185,
    "G3": 196,
    "GS3": 208,
    "A3": 220,
    "AS3": 233,
    "B3": 247,
    "C4": 262,
    "CS4": 277,
    "D4": 294,
    "DS4": 311,
    "E4": 330,
    "F4": 349,
    "FS4": 370,
    "G4": 392,
    "GS4": 415,
    "A4": 440,
    "AS4": 466,
    "B4": 494,
    "C5": 523,
    "CS5": 554,
    "D5": 587,
    "DS5": 622,
    "E5": 659,
    "F5": 698,
    "FS5": 740,
    "G5": 784,
    "GS5": 831,
    "A5": 880,
    "AS5": 932,
    "B5": 988,
    "C6": 1047,
    "CS6": 1109,
    "D6": 1175,
    "DS6": 1245,
    "E6": 1319,
    "F6": 1397,
    "FS6": 1480,
    "G6": 1568,
    "GS6": 1661,
    "A6": 1760,
    "AS6": 1865,
    "B6": 1976,
    "C7": 2093,
    "CS7": 2217,
    "D7": 2349,
    "DS7": 2489,
    "E7": 2637,
    "F7": 2794,
    "FS7": 2960,
    "G7": 3136,
    "GS7": 3322,
    "A7": 3520,
    "AS7": 3729,
    "B7": 3951,
    "C8": 4186,
    "CS8": 4435,
    "D8": 4699,
    "DS8": 4978
}

# Put the notes for your song in here!
SONG = ("F6", "F6", "E6", "F6", "F5", "P", "F5", "P", "C6", "AS5", "A5", "C6", "F6", "P", "F6", "P", "G6", "FS6", "G6", "G5", "P", "G5", "P", "G6", "F6", "E6", "D6", "C6", "P", "C6", "P", "D6", "E6", "F6", "E6", "D6", "C6", "D6", "C6", "AS5", "A5", "AS5", "A5", "G5", "F5", "G5", "F5", "E5", "D5", "C5", "D5", "E5", "F5", "G5", "AS5", "A5", "G5", "A5", "F5", "P", "F5")

NOTE_DURATION = 0.150           # The time (in seconds) to play each note for. Change this to make the song play faster or slower
STATIONARY_TOGGLE_US = 2000     # The time (in microseconds) between each direction switch of the motor when using STATIONARY_PLAYBACK
STATIONARY_PLAYBACK = False     # Whether to play the song with or without the motors spinning
DECAY_MODE = SLOW_DECAY         # The motor decay mode to use, either FAST_DECAY (0) or SLOW_DECAY (1). Affects the song's quality


# Create a new Inventor2040W
board = Inventor2040W()
motor_a = board.motors[MOTOR_A]
motor_b = board.motors[MOTOR_B]


# Variables for recording the button state and if it has been toggled
# Starting as True makes the song play automatically
button_toggle = True
last_button_state = False


# Function to check if the button has been toggled
def check_button_toggle():
    global button_toggle
    global last_button_state
    button_state = board.switch_pressed()
    if button_state and not last_button_state:
        button_toggle = not button_toggle
    last_button_state = button_state

    return button_toggle


while True:
    # Has the button been toggled?
    if check_button_toggle():

        # Play the song
        for i in range(len(SONG)):
            if check_button_toggle():
                if SONG[i] == "P":
                    # This is a "pause" note, so stop the motors
                    motor_a.stop()
                    motor_b.stop()
                    time.sleep(NOTE_DURATION)
                else:
                    # Get the frequency of the note and set the motors to it
                    freq = TONES[SONG[i]]
                    motor_a.frequency(freq)
                    motor_b.frequency(freq)

                    if STATIONARY_PLAYBACK:
                        # Set the motors to 50% duty cycle to play the note, but alternate
                        # the direction so that the motor does not actually spin
                        t = 0
                        while t < NOTE_DURATION * 1000000:
                            motor_a.duty(0.5)
                            motor_b.duty(0.5)
                            time.sleep_us(STATIONARY_TOGGLE_US)
                            t += STATIONARY_TOGGLE_US

                            motor_a.duty(-0.5)
                            motor_b.duty(-0.5)
                            time.sleep_us(STATIONARY_TOGGLE_US)
                            t += STATIONARY_TOGGLE_US
                    else:
                        # Set the motors to 50% duty cycle to play the note whilst spinning
                        motor_a.duty(0.5)
                        motor_b.duty(0.5)
                        time.sleep(NOTE_DURATION)
            else:
                # The button was toggled again, so stop playing the song
                break

        button_toggle = False

        # The song has finished, so disable the motors
        motor_a.disable()
        motor_b.disable()

    time.sleep(0.01)
