#include <stdio.h>
#include "motor.hpp"

using namespace pimoroni;

static const uint MOTOR_LEFT_P = 7;
static const uint MOTOR_LEFT_N = 6;

static const uint MOTOR_RIGHT_P = 1;
static const uint MOTOR_RIGHT_N = 0;

int main() {
  stdio_init_all();

  Motor left(MOTOR_LEFT_P, MOTOR_LEFT_N);
  Motor right(MOTOR_RIGHT_P, MOTOR_RIGHT_N);

  while(true){
    printf("accelerate forward\n");
    for(int i = 0; i < 200; i++) {
      float v = (float)i / 200.0f;

      left.set_speed(v);
      right.set_speed(v);
      sleep_ms(10);
    }

    printf("hold forward\n");
    sleep_ms(1000);

    printf("deccelerate forward\n");
    for(int i = 0; i < 200; i++) {
      float v = 1.0f - ((float)i / 200.0f);

      left.set_speed(v);
      right.set_speed(v);
      sleep_ms(10);
    }

    printf("stop\n");
    left.stop();
    right.stop();
    sleep_ms(1000);

    printf("accelerate turn right\n");
    for(int i = 0; i < 100; i++) {
      float v = ((float)i / 100.0f) * 0.5f;

      left.set_speed(v);
      right.set_speed(-v);
      sleep_ms(10);
    }

    printf("hold turn right\n");
    sleep_ms(1000);

    printf("deccelerate turn right\n");
    for(int i = 0; i < 100; i++) {
      float v = (1.0f - ((float)i / 100.0f)) * 0.5f;

      left.set_speed(v);
      right.set_speed(-v);
      sleep_ms(10);
    }

    printf("stop\n");
    left.stop();
    right.stop();
    sleep_ms(1000);
  };
  return 0;
}
