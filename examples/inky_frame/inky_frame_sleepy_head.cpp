#include <cstdio>
#include <math.h>

#include <stdio.h>
#include "pico/stdlib.h"

#include "libraries/inky_frame/inky_frame.hpp"

using namespace pimoroni;

InkyFrame inky;

uint32_t time() {
  absolute_time_t t = get_absolute_time();
  return to_ms_since_boot(t);
}

void center_text(std::string message, int y, float scale = 1.0f) {
  int32_t width = inky.measure_text(message, scale);
  inky.text(message, {(600 / 2) - (width / 2), y}, 600, scale);
}

int main() {
  inky.init();
  inky.set_pen(InkyFrame::Pen::WHITE);
  inky.clear();
  inky.set_pen(InkyFrame::Pen::BLUE);

  //inky.set_font("sans");

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

  printf("writing to screen.. ");
  center_text("i woke up because...", 100, 0.5f);

  center_text("i'm going back to sleep now, leave me alone...", 350, 0.75f);

  std::map<uint8_t, std::string> insults = {
    {InkyFrame::BUTTON_A_EVENT, "...you pressed A. You monster."},
    {InkyFrame::BUTTON_B_EVENT, "...some clown fiddled with B."},
    {InkyFrame::BUTTON_C_EVENT, "...this bozo thought C looked fun."},
    {InkyFrame::BUTTON_D_EVENT, "...a wise guy couldn't help pushing D."},
    {InkyFrame::BUTTON_E_EVENT, "...i was poked right in the E(ye)."},
    {InkyFrame::RTC_ALARM_EVENT, "...my alarm went off. zzz..."},
    {InkyFrame::EXTERNAL_TRIGGER_EVENT, "...i was externally triggered."}
  };

  std::string insult = "...of an unexplained event.";

  if(insults.count(event)) {
    insult = insults.at(event);
  }


  center_text(insult, 180, 1.0f);

  inky.update();
  printf("done!\n");

  // go to sleep and wake up in five minutes - sweet dreams little inky x
  inky.sleep(5);


  return 0;
}
