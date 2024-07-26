# SPDX-FileCopyrightText: 2024 Christopher Parrott for Pimoroni Ltd
#
# SPDX-License-Identifier: MIT

def rgb_from_hsv(h, s, v):
    if s == 0.0:
        return v, v, v
    else:
        i = int(h * 6.0)
        f = (h * 6.0) - i
        p, q, t = v * (1.0 - s), v * (1.0 - s * f), v * (1.0 - s * (1.0 - f))

        i = i % 6
        if i == 0:
            return v, t, p
        elif i == 1:
            return q, v, p
        elif i == 2:
            return p, v, t
        elif i == 3:
            return p, q, v
        elif i == 4:
            return t, p, v
        elif i == 5:
            return v, p, q
            
            
class HSVFX:
    def __init__(self, hue=0.0, sat=1.0, val=1.0):
        self.hue = hue
        self.sat = sat
        self.val = val

    def __call__(self):
        r, g, b = rgb_from_hsv(self.hue, self.sat, self.val)
        return int(r * 255), int(g * 255), int(b * 255)
