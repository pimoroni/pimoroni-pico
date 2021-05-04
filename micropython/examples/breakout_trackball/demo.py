import time
from breakout_trackball import BreakoutTrackball

sensitivity = 2

trackball = BreakoutTrackball()

trackball.set_rgbw(0, 0, 0, 64)

while True:
    state = trackball.read()
    if state[BreakoutTrackball.SW_PRESSED]:
        trackball.set_rgbw(0, 0, 0, 255)
    elif state[BreakoutTrackball.LEFT] > sensitivity:
        trackball.set_rgbw(0, 0, 255, 0)
    elif state[BreakoutTrackball.RIGHT] > sensitivity:
        trackball.set_rgbw(255, 0, 0, 0)
    elif state[BreakoutTrackball.UP] > sensitivity:
        trackball.set_rgbw(255, 255, 0, 0)
    elif state[BreakoutTrackball.DOWN] > sensitivity:
        trackball.set_rgbw(0, 255, 0, 0)
    elif state[BreakoutTrackball.SW_CHANGED]:
        trackball.set_rgbw(0, 0, 0, 64)

    time.sleep(0.02)
