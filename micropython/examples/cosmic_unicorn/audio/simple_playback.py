from audio import WavPlayer

sound = WavPlayer(0, 10, 11, 9, amp_enable=22)

sound.play_wav("beepboop.wav", False)

while sound.is_playing():
    pass
