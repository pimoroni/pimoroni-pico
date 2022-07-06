#include "enviro_urban.hpp"

using namespace pimoroni;
using namespace enviro;

EnviroUrban urban;

int main() {
  urban.init();
  urban.activity_led(true);

  // init usb serial for debugging and give our terminal monitor a bit of
  // time to connect
  stdio_init_all();
  sleep_ms(500);

  printf("Enviro Urban starting up...\n");
  printf("\n");
  while(true) {
    printf("- getting sensor readings:\n");
    urban.take_reading();

    printf("\n");
    printf("- particulate concentration (ug/m3) standardised:\n");
    printf("  > PM1   = %d\n", urban.particulates(PM1_UGM3));
    printf("  > PM2.5 = %d\n", urban.particulates(PM2_5_UGM3));
    printf("  > PM10  = %d\n", urban.particulates(PM10_UGM3));
    printf("\n");
    printf("- particulate concentration (ug/m3) actual:\n");
    printf("  > PM1   = %d\n", urban.particulates(PM1_UGM3_ATHMOSPHERIC));
    printf("  > PM2.5 = %d\n", urban.particulates(PM2_5_UGM3_ATHMOSPHERIC));
    printf("  > PM10  = %d\n", urban.particulates(PM10_UGM3_ATHMOSPHERIC));
    printf("\n");
    printf("- particulate count per litre of air:\n");
    printf("  > PM0.3  = %d\n", urban.particulates(PM0_3_PER_LITRE) * 10);
    printf("  > PM0.5  = %d\n", urban.particulates(PM0_5_PER_LITRE) * 10);
    printf("  > PM1    = %d\n", urban.particulates(PM1_PER_LITRE  ) * 10);
    printf("  > PM2.5  = %d\n", urban.particulates(PM2_5_PER_LITRE) * 10);
    printf("  > PM5    = %d\n", urban.particulates(PM5_PER_LITRE  ) * 10);
    printf("  > PM10   = %d\n", urban.particulates(PM10_PER_LITRE ) * 10);
    printf("\n");
    printf("- noise level = %d\n", urban.noise_level());
    printf("\n");
    printf("- temperature = %0.2f celcius (%0.2f fahrenheit, %0.2f kelvin)\n", 
      urban.temperature(CELCIUS),
      urban.temperature(FAHRENHEIT),
      urban.temperature(KELVIN));
    printf("> humidity = %0.2f%%\n", urban.humidity());
    printf("> pressure = %0.2f hPa (%0.2f psi, %0.2f inHg, %0.2f bar)\n", 
      urban.pressure(),
      urban.pressure(PSI),
      urban.pressure(INHG),
      urban.pressure(BAR));


    sleep_ms(5000);
  }

  urban.sleep(5);


  return 0;
}
