#include <stdio.h>
#include "motor.hpp"
#include "button.hpp"

/*
Press "A" to start/stop the motors
*/

using namespace pimoroni;

static const uint MOTOR_1P = 6;
static const uint MOTOR_1N = 7;

static const uint MOTOR_2P = 27;
static const uint MOTOR_2N = 26;

static const uint BUTTON_A = 2;

Button button_a(BUTTON_A, Polarity::ACTIVE_LOW, 0);

bool button_last_pressed = false;
bool drive_motors = false;

bool check_driving_state() {
  bool button_pressed = button_a.read();
  if(button_pressed) {
    drive_motors = !drive_motors;
  }
  return drive_motors;
}

int main() {
  stdio_init_all();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  Motor left(MOTOR_1P, MOTOR_1N);
  Motor right(MOTOR_2P, MOTOR_2N);

  int j = 0;
  bool state;
  while(true){
    gpio_put(PICO_DEFAULT_LED_PIN, true);
    left.stop();
    right.stop();
    sleep_ms(10);

    while(true){
      if(check_driving_state()) {
        gpio_put(PICO_DEFAULT_LED_PIN, false);

        printf("accelerate forward\n");
        state = check_driving_state();
        for(int i = 0; state && i < 200; i++) {
          float v = (float)i / 200.0f;

          left.set_speed(-v);
          right.set_speed(v);
          sleep_ms(10);
          state = check_driving_state();
        }
        if(!state)
            break;

        printf("hold forward\n");
        //sleep_ms(1000);
        state = check_driving_state();
        j = 0;
        while(state && j < 100){
          sleep_ms(10);
          state = check_driving_state();
          j++;
        }
        if(!state)
          break;

        printf("deccelerate forward\n");
        state = check_driving_state();
        for(int i = 0; state && i < 200; i++) {
          float v = 1.0f - ((float)i / 200.0f);

          left.set_speed(-v);
          right.set_speed(v);
          sleep_ms(10);
          state = check_driving_state();
        }
        if(!state)
          break;

        printf("stop\n");
        left.stop();
        right.stop();
        //sleep_ms(1000);
        state = check_driving_state();
        j = 0;
        while(state && j < 100){
          sleep_ms(10);
          state = check_driving_state();
          j++;
        }
        if(!state)
          break;

        printf("accelerate turn left\n");
        state = check_driving_state();
        for(int i = 0; state && i < 100; i++) {
          float v = ((float)i / 100.0f) * 0.5f;

          left.set_speed(v);
          right.set_speed(v);
          sleep_ms(10);
          state = check_driving_state();
        }
        if(!state)
          break;

        printf("hold turn left\n");
        //sleep_ms(1000);
        state = check_driving_state();
        j = 0;
        while(state && j < 100){
          sleep_ms(10);
          state = check_driving_state();
          j++;
        }
        if(!state)
          break;

        printf("deccelerate turn left\n");
        state = check_driving_state();
        for(int i = 0; state && i < 100; i++) {
          float v = (1.0f - ((float)i / 100.0f)) * 0.5f;

          left.set_speed(v);
          right.set_speed(v);
          sleep_ms(10);
          state = check_driving_state();
        }
        if(!state)
          break;

        printf("stop\n");
        left.stop();
        right.stop();
        //sleep_ms(1000);
        state = check_driving_state();
        j = 0;
        while(state && j < 100){
          sleep_ms(10);
          state = check_driving_state();
          j++;
        }
        if(!state)
          break;
      }
      else {
        sleep_ms(10);
      }
    }
  }
  return 0;
}
