from pimoroni import RGBLED, Button


class GfxPack:

    SW_A = 12
    SW_B = 13
    SW_C = 14
    SW_D = 15
    SW_E = 22
    LED_R = 6
    LED_G = 7
    LED_B = 8

    def __init__(self):
        self.rgb = RGBLED(GfxPack.LED_R, GfxPack.LED_G, GfxPack.LED_B, invert=False)
        self.switch_a = Button(GfxPack.SW_A)
        self.switch_b = Button(GfxPack.SW_B)
        self.switch_c = Button(GfxPack.SW_C)
        self.switch_d = Button(GfxPack.SW_D)
        self.switch_e = Button(GfxPack.SW_E)
