import time
import random
from cosmic import CosmicUnicorn, Channel
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN, PEN_P8

"""
A gloriously terrible melody maker.

Use Vol + and Vol - to move up/down (note pitch)

Use Lux - and D to move left/right (note position)

Press A to set a note.

Press B to delete a note.

Use Lux + to play/pause.
"""

NOTE_DURATION = 125

cu = CosmicUnicorn()
cu.set_brightness(0.5)
graphics = PicoGraphics(DISPLAY_COSMIC_UNICORN, pen_type=PEN_P8)

boopety_beepety = cu.synth_channel(0)
boopety_beepety.configure(
    waveforms=Channel.SQUARE | Channel.SINE,
    attack=0.1,
    decay=0.5,
    sustain=0.0,
    release=1.0,
    volume=1.0
)

cu.play_synth()

black = graphics.create_pen(0, 0, 0)
note = graphics.create_pen(255, 255, 255)
cursor_bg = graphics.create_pen(64, 0, 0)
cursor = graphics.create_pen(255, 0, 0)
playhead = graphics.create_pen(0, 128, 0)

cursor_position = [0, 0]

playhead_position = 0

width, height = graphics.get_bounds()

notes = [random.randint(0, height) for _ in range(width)]

last_note_advance = time.ticks_ms()

last_action = time.ticks_ms()

playing = True


def debounce(button, duration=100):
    global last_action
    if cu.is_pressed(button) and time.ticks_ms() - last_action > duration:
        last_action = time.ticks_ms()
        return True
    return False


def note_to_frequency(note_number):
    return int((2 ** ((note_number - 69.0) / 12)) * 440)


while True:
    if debounce(CosmicUnicorn.SWITCH_D):
        cursor_position[0] -= 1
        cursor_position[0] %= width

    if debounce(CosmicUnicorn.SWITCH_BRIGHTNESS_DOWN):
        cursor_position[0] += 1
        cursor_position[0] %= width

    if debounce(CosmicUnicorn.SWITCH_VOLUME_DOWN):
        cursor_position[1] += 1
        cursor_position[1] %= height

    if debounce(CosmicUnicorn.SWITCH_VOLUME_UP):
        cursor_position[1] -= 1
        cursor_position[1] %= height

    if debounce(CosmicUnicorn.SWITCH_BRIGHTNESS_UP, 500):
        playing = not playing
        if not playing:
            boopety_beepety.trigger_release()

    if cu.is_pressed(CosmicUnicorn.SWITCH_A):
        notes[cursor_position[0]] = cursor_position[1]

    if cu.is_pressed(CosmicUnicorn.SWITCH_B):
        notes[cursor_position[0]] = None

    if time.ticks_ms() - last_note_advance > NOTE_DURATION:
        current_note = None
        if playing:
            playhead_position += 1
            playhead_position %= width
            current_note = notes[playhead_position]
        if current_note is not None:
            current_note = height - current_note  # Bottom = Low, Top = High
            current_note += 36  # Shift up the scale a couple of octaves
            current_freq = note_to_frequency(current_note)
            boopety_beepety.frequency(current_freq)
            boopety_beepety.trigger_attack()
        last_note_advance = time.ticks_ms()

    graphics.set_pen(black)
    graphics.clear()

    graphics.set_pen(playhead)
    graphics.line(playhead_position, 0, playhead_position, height)

    graphics.set_pen(cursor_bg)
    graphics.line(cursor_position[0], 0, cursor_position[0], height)

    graphics.set_pen(note)
    for x in range(width):
        y = notes[x]
        if y is not None:
            graphics.pixel(x, y)

    graphics.set_pen(cursor)
    graphics.pixel(*cursor_position)

    cu.update(graphics)
