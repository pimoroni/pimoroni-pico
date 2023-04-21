# This example shows you how you can use Pico Explorer's onboard buzzer as a speaker to play different notes and string them together into a bleepy tune.
# It uses code written by Avram Piltch - check out his Tom's Hardware article! https://www.tomshardware.com/uk/how-to/buzzer-music-raspberry-pi-pico
# You'll need to connect a jumper wire between GPO and AUDIO on the Explorer Base to hear noise.

import time
from picographics import PicoGraphics, DISPLAY_PICO_EXPLORER
from pimoroni import Buzzer

display = PicoGraphics(display=DISPLAY_PICO_EXPLORER)

# Create a buzzer on pin 0
# Don't forget to wire GP0 to AUDIO!
BUZZER = Buzzer(0)

BLACK = display.create_pen(0, 0, 0)
GREEN = display.create_pen(0, 255, 0)

# this handy list converts notes into frequencies, which you can use with the explorer.set_tone function
tones = {
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

# put the notes for your song in here!
song = ["AS6", "A6", "AS6", "P", "AS5", "P", "AS5", "P", "F6", "DS6", "D6", "F6", "AS6", "A6", "AS6", "D7", "C7", "AS6", "C7", "P", "C6", "P", "C6", "P", "C6", "AS5", "A5", "C6", "F6", "P", "F6", "P", "G6", "A6", "AS6", "A6", "G6", "F6", "G6", "F6", "DS6", "D6", "DS6", "D6", "C6", "AS5", "AS5", "A5", "G5", "F5", "G5", "AS5", "A5", "C6", "AS5", "D6", "C6", "DS6", "D6", "P", "AS5", "P", "AS5"]


def clear():                        # this function clears Pico Explorer's screen to black
    display.set_pen(BLACK)
    display.clear()
    display.update()


def playtone(frequency):            # this function tells your program how to make noise
    BUZZER.set_tone(frequency)


def bequiet():                      # this function tells your program how not to make noise
    BUZZER.set_tone(-1)


def playsong(song):                 # this function plays your song
    a = 0                           # this variable keeps track of the visualiser bars
    for i in range(len(song)):
        if (song[i] == "P"):
            bequiet()
        else:
            playtone(tones[song[i]])
            display.set_pen(GREEN)  # switch to green pen
            display.rectangle(a, 240 - (int((tones[song[i]]) / 21)), 5, 240)  # draw a green bar corresponding to the frequency of the note
            a += 7
        if a >= 240:  # clears the screen if the green bars reach the right hand edge
            clear()
            a = 0
        display.update()
        time.sleep(0.15)  # change this number if you want to alter how long the notes play for
    bequiet()


clear()
playsong(song)
clear()
