import gc
import time
import math
from machine import Timer
from galactic import GalacticUnicorn, Channel
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN as DISPLAY

'''
Displays some text, gradients and colours and demonstrates button use.
Also demonstrates some of the audio / synth features.

- Button A plays a synth tune
- Button B plays a solo channel of the synth tune
- Button C plays a sinewave (it's frequency can be adjusted with VOL + and -)
- Button D plays a second sinewave (it's frequency can be adjusted with LUX + and -)
- Sleep button stops the sounds
'''

gc.collect()

gu = GalacticUnicorn()
graphics = PicoGraphics(DISPLAY)

width = GalacticUnicorn.WIDTH
height = GalacticUnicorn.HEIGHT

SONG_LENGTH = 384
HAT = 20000
BASS = 500
SNARE = 6000
SUB = 50

melody_notes = (
    147, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 247, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 175, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 330, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 349, 0, 0, 0, 0, 0, 0, 0, 349, 0, 330, 0, 294, 0, 220, 0, 262, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 247, 0, 0, 0, 0, 0, 0, 0, 247, 0, 220, 0, 196, 0, 147, 0, 175, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0,
    147, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 247, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 175, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 330, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 349, 0, 0, 0, 0, 0, 0, 0, 349, 0, 330, 0, 294, 0, 220, 0, 262, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 247, 0, 0, 0, 0, 0, 0, 0, 247, 0, 220, 0, 196, 0, 147, 0, 175, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0,
    147, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 247, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 175, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 330, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 349, 0, 0, 0, 0, 0, 0, 0, 349, 0, 330, 0, 294, 0, 220, 0, 262, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 247, 0, 0, 0, 0, 0, 0, 0, 247, 0, 262, 0, 294, 0, 392, 0, 440, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

rhythm_notes = (
    294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 392, 0, 523, 0, 659, 0, 523, 0, 392, 0, 523, 0, 659, 0, 523, 0, 698, 0, 587, 0, 440, 0, 587, 0, 698, 0, 587, 0, 440, 0, 587, 0, 523, 0, 440, 0, 330, 0, 440, 0, 523, 0, 440, 0, 330, 0, 440, 0, 349, 0, 294, 0, 220, 0, 294, 0, 349, 0, 294, 0, 220, 0, 294, 0, 262, 0, 247, 0, 220, 0, 175, 0, 165, 0, 147, 0, 131, 0, 98, 0,
    294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 392, 0, 523, 0, 659, 0, 523, 0, 392, 0, 523, 0, 659, 0, 523, 0, 698, 0, 587, 0, 440, 0, 587, 0, 698, 0, 587, 0, 440, 0, 587, 0, 523, 0, 440, 0, 330, 0, 440, 0, 523, 0, 440, 0, 330, 0, 440, 0, 349, 0, 294, 0, 220, 0, 294, 0, 349, 0, 294, 0, 220, 0, 294, 0, 262, 0, 247, 0, 220, 0, 175, 0, 165, 0, 147, 0, 131, 0, 98, 0,
    294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 392, 0, 523, 0, 659, 0, 523, 0, 392, 0, 523, 0, 659, 0, 523, 0, 698, 0, 587, 0, 440, 0, 587, 0, 698, 0, 587, 0, 440, 0, 587, 0, 523, 0, 440, 0, 330, 0, 440, 0, 523, 0, 440, 0, 330, 0, 440, 0, 349, 0, 294, 0, 220, 0, 294, 0, 349, 0, 294, 0, 220, 0, 294, 0, 262, 0, 247, 0, 220, 0, 175, 0, 165, 0, 147, 0, 131, 0, 98, 0)

drum_beats = (
    BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0,
    BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0,
    BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0)

hi_hat = (
    HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1,
    HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1,
    HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1)

bass_notes = (
    SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0,
    SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0,
    SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0)

notes = [melody_notes, rhythm_notes, drum_beats, hi_hat, bass_notes]
channels = [gu.synth_channel(i) for i in range(len(notes))]


def gradient(r, g, b):
    for y in range(0, height):
        for x in range(0, width):
            graphics.set_pen(graphics.create_pen(int((r * x) / 52), int((g * x) / 52), int((b * x) / 52)))
            graphics.pixel(x, y)


def grid(r, g, b):
    for y in range(0, height):
        for x in range(0, width):
            if (x + y) % 2 == 0:
                graphics.set_pen(graphics.create_pen(r, g, b))
            else:
                graphics.set_pen(0)
            graphics.pixel(x, y)


def outline_text(text):
    ms = time.ticks_ms()

    graphics.set_font("bitmap8")
    v = int((math.sin(ms / 100.0) + 1.0) * 127.0)
    w = graphics.measure_text(text, 1)

    x = int(53 / 2 - w / 2 + 1)
    y = 2

    graphics.set_pen(0)
    graphics.text(text, x - 1, y - 1, -1, 1)
    graphics.text(text, x, y - 1, -1, 1)
    graphics.text(text, x + 1, y - 1, -1, 1)
    graphics.text(text, x - 1, y, -1, 1)
    graphics.text(text, x + 1, y, -1, 1)
    graphics.text(text, x - 1, y + 1, -1, 1)
    graphics.text(text, x, y + 1, -1, 1)
    graphics.text(text, x + 1, y + 1, -1, 1)

    graphics.set_pen(graphics.create_pen(v, v, v))
    graphics.text(text, x, y, -1, 1)


gu.set_brightness(0.5)

# Vars for storing button state
was_a_pressed = False
was_b_pressed = False
was_c_pressed = False
was_d_pressed = False
was_z_pressed = False

# The two frequencies to play
tone_a = 0
tone_b = 0

# The current synth beat
beat = 0


def next_beat():
    global beat
    for i in range(5):
        if notes[i][beat] > 0:
            channels[i].frequency(notes[i][beat])
            channels[i].trigger_attack()
        elif notes[i][beat] == -1:
            channels[i].trigger_release()

    beat = (beat + 1) % SONG_LENGTH


def tick(timer):
    next_beat()


timer = Timer(-1)

synthing = False


while True:

    time_ms = time.ticks_ms()
    test = (time_ms // 1000) % 5

    if gu.is_pressed(GalacticUnicorn.SWITCH_A):
        if not was_a_pressed:
            # Configure the synth to play our notes
            channels[0].configure(waveforms=Channel.TRIANGLE + Channel.SQUARE,
                                  attack=0.016,
                                  decay=0.168,
                                  sustain=0xafff / 65535,
                                  release=0.168,
                                  volume=10000 / 65535)
            channels[1].configure(waveforms=Channel.SINE + Channel.SQUARE,
                                  attack=0.038,
                                  decay=0.300,
                                  sustain=0,
                                  release=0,
                                  volume=12000 / 65535)
            channels[2].configure(waveforms=Channel.NOISE,
                                  attack=0.005,
                                  decay=0.010,
                                  sustain=16000 / 65535,
                                  release=0.100,
                                  volume=18000 / 65535)
            channels[3].configure(waveforms=Channel.NOISE,
                                  attack=0.005,
                                  decay=0.005,
                                  sustain=8000 / 65535,
                                  release=0.040,
                                  volume=8000 / 65535)
            channels[4].configure(waveforms=Channel.SQUARE,
                                  attack=0.010,
                                  decay=0.100,
                                  sustain=0,
                                  release=0.500,
                                  volume=12000 / 65535)

            # If the synth is not already playing, init the first beat
            if not synthing:
                beat = 0
                next_beat()

            gu.play_synth()
            synthing = True
            timer.init(freq=10, mode=Timer.PERIODIC, callback=tick)

        was_a_pressed = True
    else:
        was_a_pressed = False

    if gu.is_pressed(GalacticUnicorn.SWITCH_B):
        if not was_b_pressed:
            # Configure the synth to play our notes, but with only one channel audable
            channels[0].configure(waveforms=Channel.TRIANGLE + Channel.SQUARE,
                                  attack=0.016,
                                  decay=0.168,
                                  sustain=0,
                                  release=0.168,
                                  volume=0)
            channels[1].configure(waveforms=Channel.SINE + Channel.SQUARE,
                                  attack=0.038,
                                  decay=0.300,
                                  sustain=0,
                                  release=0,
                                  volume=12000 / 65535)
            channels[2].configure(waveforms=Channel.NOISE,
                                  attack=0.005,
                                  decay=0.010,
                                  sustain=16000 / 65535,
                                  release=0.100,
                                  volume=0)
            channels[3].configure(waveforms=Channel.NOISE,
                                  attack=0.005,
                                  decay=0.005,
                                  sustain=8000 / 65535,
                                  release=0.040,
                                  volume=0)
            channels[4].configure(waveforms=Channel.SQUARE,
                                  attack=0.010,
                                  decay=0.100,
                                  sustain=0,
                                  release=0.500,
                                  volume=0)

            # If the synth is not already playing, init the first beat
            if not synthing:
                beat = 0
                next_beat()

            gu.play_synth()
            synthing = True
            timer.init(freq=10, mode=Timer.PERIODIC, callback=tick)

        was_b_pressed = True
    else:
        was_b_pressed = False

    if gu.is_pressed(GalacticUnicorn.SWITCH_C):
        if not was_c_pressed:
            # Stop synth (if running) and play Tone A
            timer.deinit()
            tone_a = 400
            channels[0].play_tone(tone_a, 0.06)

            gu.play_synth()
            synthing = False

        was_c_pressed = True
    else:
        was_c_pressed = False

    if gu.is_pressed(GalacticUnicorn.SWITCH_D):
        if not was_d_pressed:
            # Stop synth (if running) and play Tone B
            timer.deinit()
            tone_b = 600

            channels[1].play_tone(tone_b, 0.06, attack=0.5)

            gu.play_synth()
            synthing = False

        was_d_pressed = True
    else:
        was_d_pressed = False

    if gu.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_UP):
        if tone_b > 0:  # Zero means tone not playing
            # Increase Tone B
            tone_b = min(tone_b + 10, 20000)
            channels[1].frequency(tone_b)

    if gu.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_DOWN):
        if tone_b > 0:  # Zero means tone not playing
            # Decrease Tone B
            tone_b = max(tone_b - 10, 10)
            channels[1].frequency(max(tone_b, 10))

    if gu.is_pressed(GalacticUnicorn.SWITCH_VOLUME_UP):
        if tone_a > 0:  # Zero means tone not playing
            # Increase Tone A
            tone_a = min(tone_a + 10, 20000)
            channels[0].frequency(tone_a)

    if gu.is_pressed(GalacticUnicorn.SWITCH_VOLUME_DOWN):
        if tone_a > 0:  # Zero means tone not playing
            # Decrease Tone A
            tone_a = max(tone_a - 10, 10)
            channels[0].frequency(tone_a)

    if gu.is_pressed(GalacticUnicorn.SWITCH_SLEEP):
        if not was_z_pressed:
            # Stop synth and both tones
            tone_a = 0
            tone_b = 0
            gu.stop_playing()
            timer.deinit()
            synthing = False

        was_z_pressed = True
    else:
        was_z_pressed = False

    graphics.set_pen(graphics.create_pen(0, 0, 0))
    graphics.clear()

    if test == 0:
        # print("grid pattern")
        grid(255, 255, 255)
    elif test == 1:
        # print("red gradient")
        gradient(255, 0, 0)
    elif test == 2:
        # print("green gradient")
        gradient(0, 255, 0)
    elif test == 3:
        # print("blue gradient")
        gradient(0, 0, 255)
    elif test == 4:
        # print("white gradient")
        gradient(255, 255, 255)

    text = ""

    if gu.is_pressed(GalacticUnicorn.SWITCH_A):
        text = "Play Synth"

    if gu.is_pressed(GalacticUnicorn.SWITCH_B):
        text = "Solo Synth"

    if gu.is_pressed(GalacticUnicorn.SWITCH_C):
        text = "Tone A"

    if gu.is_pressed(GalacticUnicorn.SWITCH_D):
        text = "Tone B"

    if gu.is_pressed(GalacticUnicorn.SWITCH_VOLUME_UP):
        text = "Raise A"

    if gu.is_pressed(GalacticUnicorn.SWITCH_VOLUME_DOWN):
        text = "Lower A"

    if gu.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_UP):
        text = "Raise B"

    if gu.is_pressed(GalacticUnicorn.SWITCH_BRIGHTNESS_DOWN):
        text = "Lower B"

    if gu.is_pressed(GalacticUnicorn.SWITCH_SLEEP):
        text = "Stop"

    outline_text(text)

    gu.update(graphics)

    # pause for a moment (important or the USB serial device will fail
    time.sleep(0.001)
