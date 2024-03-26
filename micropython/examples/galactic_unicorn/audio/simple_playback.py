from machine import Pin
from audio import WavPlayer
from galactic import GalacticUnicorn

gu = GalacticUnicorn()
amp_enable = Pin(22, Pin.OUT)
amp_enable.on()

sound = WavPlayer(0, 10, 11, 9)

sound.play_wav("beepboop.wav", False)

while sound.is_playing():
    pass
