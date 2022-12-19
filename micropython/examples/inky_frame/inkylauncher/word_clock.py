import ntp
import machine

# Length of time between updates in minutes.
UPDATE_INTERVAL = 15
graphics = None

rtc = machine.RTC()
time_string = None
words = ["it", "dx", "is", "mn", "about", "lve", "quarter", "c", "half", "to", "ou", "past", "n", "one",
         "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven", "twelve", "r", "O'Clock"]


def approx_time(hours, minutes):
    nums = {0: "twelve", 1: "one", 2: "two",
            3: "three", 4: "four", 5: "five", 6: "six",
            7: "seven", 8: "eight", 9: "nine", 10: "ten",
            11: "eleven", 12: "twelve"}

    if hours == 12:
        hours = 0
    if minutes > 0 and minutes < 8:
        return "it is about " + nums[hours] + " O'Clock"
    elif minutes >= 8 and minutes < 23:
        return "it is about quarter past " + nums[hours]
    elif minutes >= 23 and minutes < 38:
        return "Tit is about half past " + nums[hours]
    elif minutes >= 38 and minutes < 53:
        return "it is about quarter to " + nums[hours + 1]
    else:
        return "it is about " + nums[hours + 1] + " O'Clock"


def update():
    global time_string
    # grab the current time from the ntp server and update the Pico RTC
    ntp.fetch()

    current_t = rtc.datetime()
    time_string = approx_time(current_t[4] - 12 if current_t[4] > 12 else current_t[4], current_t[5])

    # Splits the string into an array of words for displaying later
    time_string = time_string.split()

    print(time_string)


def draw():
    global time_string
    graphics.set_font("bitmap8")

    WIDTH, HEIGHT = graphics.get_bounds()

    # Clear the screen
    graphics.set_pen(1)
    graphics.clear()
    graphics.set_pen(6)

    x = 10
    y = 10
    scale = 5
    spacing = 2

    for word in words:

        if word in time_string:
            graphics.set_pen(0)
        else:
            graphics.set_pen(7)

        for letter in word:
            text_length = graphics.measure_text(letter, scale, spacing)
            if not x + text_length <= WIDTH:
                y += 70
                x = 5

            graphics.text(letter.upper(), x, y, 640, scale, spacing)
            x += 40

    graphics.update()
