#include "pico/stdlib.h"

#include "breakout_rtc.hpp"
#include "drivers/button/button.hpp"
#include "libraries/gfx_pack/gfx_pack.hpp"

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);

BreakoutRTC3032 rtc(&i2c);

ST7567 st7567(128, 64, GfxPack::gfx_pack_pins);
PicoGraphics_Pen1Bit graphics(st7567.width, st7567.height, nullptr);
RGBLED backlight_rgb(GfxPack::BL_R, GfxPack::BL_G, GfxPack::BL_B, Polarity::ACTIVE_HIGH);
Button button_a(GfxPack::A);
Button button_b(GfxPack::B);
Button button_c(GfxPack::C);
Button button_d(GfxPack::D);
Button button_e(GfxPack::E);

int main() {
  stdio_init_all();
  sleep_ms(5000);
#ifdef PICO_DEFALUT_LED_PIN
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif



  if(!rtc.init()) {
    printf("Init failed! Check your connections and i2c pin choices.\n");
  }
  if(rtc.setup()){
    printf("RTC setup correctly\n");
  }
  else{
    while(1){
    printf("RTC setup error !\n");
    }
  }

  // Uncomment the below line on first run to set the RTC date and time to the program's compiler time
  //rtc.set_to_compiler_time();

  // Make sure we have 24-hour time
  if(rtc.is_12_hour())
    rtc.set_24_hour();
  graphics.set_pen(0);
  graphics.clear();
  graphics.set_pen(15);

  graphics.text("Hello World", Point(0,0), 320);
  st7567.update(&graphics);
  // Set to update once per second
  rtc.enable_periodic_update_interrupt(true);
  rtc.set_backup_switchover_mode(1);

  while(true) {
    // Has a second passed?
    
    if(rtc.read_periodic_update_interrupt_flag()) {
      rtc.clear_periodic_update_interrupt_flag();
    
      // Update the locally stored time from the RTC
      if(rtc.update_time()) {

        printf("temperature is %.4f C\n", rtc.get_temperature(true));
         printf("temperature is %.4f F\n", rtc.get_temperature(false));
        printf("Date: %s, Time: %s\n", rtc.string_date(), rtc.string_time());
        graphics.set_pen(0);
        graphics.clear();
        graphics.set_pen(15);
        graphics.text(rtc.string_time(), Point(0,0), 320);
        graphics.text(rtc.string_date(), Point(0,16), 320);
        graphics.text(rtc.string_temperature(true), Point(0,32), 320);
        graphics.text(rtc.string_temperature(false), Point(0,48), 320);
        st7567.update(&graphics);
#ifdef PICO_DEFAULT_LED_PIN
        gpio_put(PICO_DEFAULT_LED_PIN, true);
#endif
        sleep_ms(100);
#ifdef PICO_DEFAULT_LED_PIN
        gpio_put(PICO_DEFAULT_LED_PIN, false);
#endif
      }
    }
    sleep_ms(100);
  }

  return 0;
}
