#include <cstdio>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "pico/stdlib.h"

#include "inky_frame.hpp"

using namespace pimoroni;

InkyFrame inky;
datetime_t now;
datetime_t today;


uint32_t time() {
  absolute_time_t t = get_absolute_time();
  return to_ms_since_boot(t);
}

void center_text(std::string message, int y, float scale = 1.0f) {
  int32_t tw = inky.measure_text(message, scale);
  inky.text(message, (600 / 2) - (tw / 2), y, scale);
}

void center_text(std::string message, int x, int y, int w, float scale = 1.0f) {
  int32_t tw = inky.measure_text(message, scale);
  inky.text(message, (w / 2) - (tw / 2) + x, y, scale);
}

std::string day_name(datetime_t &dt) {
  std::string result = std::to_string(dt.day);
       if(result.back() == '1') {result += "st"; }
  else if(result.back() == '2') {result += "nd";}
  else if(result.back() == '3') {result += "rd";}
  else                          {result += "th";}
  return result;
}

std::string month_name(datetime_t &dt) {
  static std::array<std::string, 12> month_names = {
    "January", "February", "March", "April", "May", "June",
    "July", "August","September", "October", "November", "December"
  };
  return month_names[dt.month - 1];
}

std::string short_day_name(int dotw) {
  static std::array<std::string, 7> day_names = {
    "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"
  };

  return day_names[dotw];
}

std::string year_name(datetime_t &dt) {
  return std::to_string(dt.year);
}

int days_in_month(datetime_t &dt)
{
	if(dt.month == 2)	{ // leap year handling
    return (dt.year % 400 == 0) || (dt.year % 4 == 0 && dt.year % 100 != 0) ? 29 : 28;
	}

  return (dt.month == 1 || dt.month == 3 || dt.month ==  5 ||
          dt.month == 7 || dt.month == 8 || dt.month == 10 ||
          dt.month == 12) ? 31 : 30;
}

int day_of_week(datetime_t &dt) {
  // based on implementation found at
  // https://www.tondering.dk/claus/cal/chrweek.php#calcdow
  uint a = (14 - dt.month) / 12;
  uint y = dt.year - a;
  uint m = dt.month + (12 * a) - 2;
  return (dt.day + y + (y / 4) - (y / 100) + (y / 400) + ((31 * m) / 12)) % 7;
}

void draw_flipper(int x, int y, int w, int h, std::string t) {
  inky.text_tracking(0.95f);
  inky.text_aspect(1.1f);

  inky.pen(InkyFrame::BLACK);
  inky.thickness(6);
  inky.rectangle(x + 3 + 5, y + 3 + 5, w - 6, h - 6);

  inky.pen(InkyFrame::WHITE);
  inky.thickness(6);
  inky.rectangle(x + 3, y + 3, w - 6, h - 6);

  inky.pen(InkyFrame::TAUPE);
  inky.rectangle(x + 3, y + (h / 2) + 3, w - 6, (h / 2) - 6);

  inky.pen(InkyFrame::BLACK);
  inky.thickness(h / 8);
  center_text(t, x, y + (h / 2), w, h / 38.0f);
}

void render_calendar_view() {
  int width = 240;

  inky.text_tracking(0.95f);
  inky.text_aspect(1.1f);

  inky.pen(InkyFrame::RED);
  inky.rectangle(0, 0, width, 448);

  draw_flipper(20, 20, 90, 120, "2");
  draw_flipper(130, 20, 90, 120, "1");


  draw_flipper(20, 160, 200, 36, "FEBRUARY");
/*



  inky.pen(InkyFrame::WHITE);
  center_text(day_name(now), 0, 60, width, 2.6f);

  inky.thickness(1);
  center_text(month_name(now) + " " + year_name(now), 0, 110, width, 0.75f);
*/
/*
  inky.pen(InkyFrame::WHITE);
  inky.thickness(1);
  inky.line(10, 140, 300, 140);
  int baseline = get_baseline(inky._font);
  printf("baseline is %d\n", baseline);
  inky.text("This is a jolly test for great descenders!", 10, 140 + baseline);
*/

  datetime_t first_day = {.year = now.year, .month = now.month, .day = 1};
  int first_dotw = day_of_week(first_day);

  // draw a calendar
  int xoff = 20;
  int yoff = 240;
  int day_width = (width - xoff - xoff) / 7;
  int row_height = day_width;
  int dotw = first_dotw;

  inky.thickness(6);
  inky.pen(InkyFrame::BLACK);
  inky.rectangle(xoff + 5, yoff + 3 - 16 + 5, width - xoff - xoff - 3, (row_height + 5) * 6 - 6);

  inky.pen(InkyFrame::WHITE);
  inky.rectangle(xoff, yoff + 3 - 16, width - xoff - xoff - 6, (row_height + 5) * 6 - 6);

  inky.thickness(2);
  inky.pen(InkyFrame::BLUE);
  for(int dotw_header = 0; dotw_header <= 6; dotw_header++) {
    center_text(short_day_name(dotw_header), day_width * dotw_header + xoff, yoff, day_width, 0.6f);
  }
  yoff += row_height;

  inky.thickness(1);

  for(int day = 1; day <= days_in_month(first_day); day++) {
    if(day == now.day) {
      inky.pen(InkyFrame::RED);
      inky.rectangle(day_width * dotw + xoff, yoff - (row_height / 2), day_width, row_height);
      inky.pen(InkyFrame::WHITE);
      center_text(std::to_string(day), day_width * dotw + xoff, yoff, day_width, 0.6f);
    } else if(dotw == 0 || dotw == 6) {
      inky.pen(InkyFrame::YELLOW);
      inky.rectangle(day_width * dotw + xoff, yoff - (row_height / 2), day_width, row_height);
      inky.pen(InkyFrame::BLACK);
      center_text(std::to_string(day), day_width * dotw + xoff, yoff, day_width, 0.6f);
    } else {
      inky.pen(InkyFrame::BLACK);
      center_text(std::to_string(day), day_width * dotw + xoff, yoff, day_width, 0.6f);
    }

    dotw++;
    if(dotw >= 7) {dotw = 0; yoff += row_height + 5;}
  }
}

// horrible hacky way to convert strings into numbers because std::string_view
// isn't supported by the various std::itoa methods.
int parse_number(const char *p, int len) {
  p += len - 1; // start at end of string
  int m = 1, r = 0;
  while(len--) {r += ((*p--) - 48) * m; m *= 10;}
  return r;
}

bool today_or_after(datetime_t dt1, datetime_t dt2) {
  return (dt1.year < dt2.year) ? true :
         (dt1.year == dt2.year && dt1.month < dt2.month) ? true :
         (dt1.year == dt2.year && dt1.month == dt2.month && dt1.day < dt2.day) ? true : false;
}

uint datetime_to_timestamp(const datetime_t &dt) {
  struct tm tm = {
    .tm_sec = dt.sec, .tm_min = dt.min, .tm_hour = dt.hour,
    .tm_mday = dt.day, .tm_mon = dt.month, .tm_year = dt.year
  };
  return mktime(&tm);
}

bool operator==(const datetime_t &dt1, const datetime_t &dt2) {
  uint ts1 = datetime_to_timestamp(dt1);
  uint ts2 = datetime_to_timestamp(dt2);
  return ts1 == ts2;
}

bool operator>(const datetime_t &dt1, const datetime_t &dt2) {
  uint ts1 = datetime_to_timestamp(dt1);
  uint ts2 = datetime_to_timestamp(dt2);
  return ts1 > ts2;
}

datetime_t datetime_from_iso8601(std::string_view s) {
  datetime_t dt;

  if(s.back() == 'Z') {
    // has time element, parse that first
    dt.sec = parse_number(&s[s.length() - 3], 2);
    dt.min = parse_number(&s[s.length() - 5], 2);
    dt.hour = parse_number(&s[s.length() - 7], 2);
  } else {
    dt.sec = 0;
    dt.min = 0;
    dt.hour = 0;

  }

  dt.year = parse_number(&s[0], 4);
  dt.month = parse_number(&s[4], 2);
  dt.day = parse_number(&s[6], 2);

  return dt;
}


// horrible hacky way to trim whitespace from a std::string_view. why is
// dealing with text always so horrible in c++...? :-)
void trim(std::string_view &s) {
  while(std::isspace(s.front()) && s.length() > 0) {s.remove_prefix(1);}
  while(std::isspace( s.back()) && s.length() > 0) {s.remove_suffix(1);}
}


void render_calendar_entries() {

  FATFS fs;
  FRESULT fr;

  fr = f_mount(&fs, "", 1);
  if (fr != FR_OK) {
    // TODO: show error message on screen - missing or bad SD card
  }

  // TODO: find all (first 6?) calendar files (.ics) on SD card and assign
  // each one a colour for entries
  /*FILINFO fileinfo;
  auto dir = new DIR();
  f_opendir(dir, "/");
  while(f_readdir(dir, &fileinfo) == FR_OK && fileinfo.fname[0]) {
      printf("- %s %lld\n", fileinfo.fname, fileinfo.fsize);
  }
  f_closedir(dir);
  printf("done!\n");*/

  FIL file;
  f_open(&file, "jon.ics", FA_READ);


  struct event_t {
    std::string location;
    std::string summary;
    datetime_t when = {.hour = 0, .min = 0, .sec = 0};
  };

  std::vector<event_t> events;
  event_t event;

  char buffer[1024];
  while(!f_eof(&file)) {
    f_gets(buffer, 1024, &file);

    std::string_view line(buffer);

    if(line.substr(0, 9) == "LOCATION:") {
      line = line.substr(9);
      trim(line);
      event.location = line;
    }

    if(line.substr(0, 8) == "SUMMARY:") {
      line = line.substr(8);
      trim(line);
      event.summary = line;
    }

    if(line.substr(0, 7) == "DTSTART") {
      line = line.substr(line.find(":") + 1);
      trim(line);
      event.when = datetime_from_iso8601(line);
    }

    if(line.substr(0, 10) == "END:VEVENT") {
      if(event.when > today) {
        events.push_back(event);
      }
    }
  }

  sort(events.begin(), events.end(), [](const event_t &e1, const event_t &e2) -> bool {
    return e2.when > e1.when;
  });

  printf("%d events\n", events.size());

  /*

  // draw a calendar
  int xoff = 20;
  int yoff = 220;
  int day_width = (width - xoff - xoff) / 7;
  int row_height = day_width;
  int dotw = first_dotw;

  inky.thickness(2);

  for(int dotw_header = 0; dotw_header <= 6; dotw_header++) {
    center_text(short_day_name(dotw_header), day_width * dotw_header + xoff, yoff, day_width, 0.7f);
  }
  yoff += row_height;

  inky.thickness(1);

  for(int day = 1; day <= days_in_month(first_day); day++) {
    if(day == now.day) {
      inky.pen(InkyFrame::WHITE);
      inky.rectangle(day_width * dotw + xoff, yoff - (row_height / 2), day_width, row_height);
      inky.pen(InkyFrame::RED);
      center_text(std::to_string(day), day_width * dotw + xoff, yoff, day_width, 0.7f);
      inky.pen(InkyFrame::WHITE);
    }else{
      center_text(std::to_string(day), day_width * dotw + xoff, yoff, day_width, 0.7f);
    }

    dotw++;
    if(dotw >= 7) {dotw = 0; yoff += row_height;}
  }
  */

  int spacing = 5;
  int xoff = 240 + spacing;
  int yoff = spacing;
  int width = 600 - xoff - spacing;
  int row_height = 50;


  for(auto event : events) {
    // end of event marker, now we can render this event to the screen
    inky.pen(InkyFrame::BLACK);
    inky.thickness(2);
    inky.text(event.summary, xoff + spacing, yoff + spacing + 10, 0.65f);

    inky.pen(InkyFrame::GREEN);
    inky.thickness(1);
    inky.text(event.location, xoff + spacing, yoff + spacing + 10 + 20, 0.6f);

    inky.pen(InkyFrame::TAUPE);
    inky.line(xoff, yoff + row_height, xoff + width, yoff + row_height);
    inky.line(xoff, yoff + row_height + 1, xoff + width, yoff + row_height + 1);

    yoff += row_height + spacing;

  }


  f_close(&file);
}

int main() {
  inky.init();

  // init usb serial for debugging and give our terminal monitor a bit of
  // time to connect
  stdio_init_all();
  sleep_ms(500);

  printf("\n");
  printf("\n");
  printf("\n");

  printf("initialising inky frame.. ");
  printf("done!\n");


  InkyFrame::WakeUpEvent event = inky.get_wake_up_event();
  printf("%d\n", event);

  // set date to display
  now = {.year = 2021, .month = 3, .day = 25, .hour = 8, .min = 7, .sec = 12};
  today = {.year = now.year, .month = now.month, .day = now.day};

  printf("writing to screen.. ");
  render_calendar_view();


  render_calendar_entries();

  inky.update();
  printf("done!\n");

  // go to sleep and wake up in five minutes - sweet dreams little inky x
  inky.sleep(5);


  return 0;
}
