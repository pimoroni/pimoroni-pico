from audio import WavPlayer

sound = WavPlayer(0, 10, 11, 9, amp_enable=29)

sound.play_wav("pirate-arrrr.wav", False)

while sound.is_playing():
    pass
