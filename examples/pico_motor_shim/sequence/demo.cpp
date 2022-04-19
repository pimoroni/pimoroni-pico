#include <stdio.h>
#include "pico_motor_shim.hpp"

#include "common/pimoroni_common.hpp"
#include "motor.hpp"
#include "button.hpp"

/*
Program showing how the two motors of the Pico Motor Shim can be perform a sequence of movements.
Press "A" to start and stop the movement sequence
*/

using namespace pimoroni;
using namespace motor;

static constexpr float TOP_SPEED = 1.0f; //A value between 0 and 1
static const uint32_t ACCELERATE_TIME_MS = 2000;
static const uint32_t WAIT_TIME_MS = 1000;
static const uint32_t STOP_TIME_MS = 1000;


Button button_a(pico_motor_shim::BUTTON_A, Polarity::ACTIVE_LOW, 0);

Motor motor_1(pico_motor_shim::MOTOR_1);//, Motor::DEFAULT_PWM_FREQUENCY, Motor::DEFAULT_DECAY_MODE);
Motor motor_2(pico_motor_shim::MOTOR_2);//, Motor::DEFAULT_PWM_FREQUENCY, Motor::DEFAULT_DECAY_MODE);

static bool button_toggle = false;

/**
 * Checks if the button has been pressed, toggling a value that is also returned.
 */
bool check_button_toggle() {
  bool button_pressed = button_a.read();
  if(button_pressed) {
    button_toggle = !button_toggle;
  }
  return button_toggle;
}

/**
 * Waits for a given amount of time (in milliseconds).
 * Exits early if the user presses the button to stop the sequence, returning false.
 */
bool wait_for(uint32_t duration_ms) {
  uint32_t start_time = millis();
  uint32_t ellapsed = 0;

  //Loops until the duration has elapsed, checking the button state every millisecond
  while(ellapsed < duration_ms) {
    if(!check_button_toggle())
      return false;

    sleep_ms(1);
    ellapsed = millis() - start_time;
  }
  return true;
}

/**
 * Accelerate/Decelerate the motors from their current speed to the target speed over the given amount of time (in milliseconds).
 * Exits early if the user presses the button to stop the sequence, returning false.
 */
bool accelerate_over(float left_speed, float right_speed, uint32_t duration_ms) {
  uint32_t start_time = millis();
  uint32_t ellapsed = 0;

  //Get the current motor speeds
  float last_left = motor_1.get_speed();
  float last_right = motor_2.get_speed();

  //Loops until the duration has elapsed, checking the button state every millisecond, and updating motor speeds
  while(ellapsed <= duration_ms) {
    if(!check_button_toggle())
      return false;

    //Calculate and set the new motor speeds
    float percentage = (float)ellapsed / (float)duration_ms;
    motor_1.set_speed(((left_speed - last_left) * percentage) + last_left);
    motor_2.set_speed(((right_speed - last_right) * percentage) + last_right);

    sleep_ms(1);
    ellapsed = millis() - start_time;
  }

  //Set the final motor speeds as loop may not reach 100%
  motor_1.set_speed(left_speed);
  motor_2.set_speed(right_speed);

  return true;
}

/**
 * The function that performs the driving sequence.
 * Exits early if the user presses the button to stop the sequence, returning false.
 */
bool sequence() {
  printf("accelerate forward\n");
  if(!accelerate_over(-TOP_SPEED, TOP_SPEED, ACCELERATE_TIME_MS))
    return false; //Early exit if the button was toggled

  printf("driving forward\n");
  if(!wait_for(WAIT_TIME_MS))
    return false; //Early exit if the button was toggled

  printf("deccelerate forward\n");
  if(!accelerate_over(0.0f, 0.0f, ACCELERATE_TIME_MS))
    return false; //Early exit if the button was toggled

  printf("stop\n");
  motor_1.stop();
  motor_2.stop();
  if(!wait_for(STOP_TIME_MS))
    return false; //Early exit if the button was toggled

  printf("accelerate turn left\n");
  if(!accelerate_over(TOP_SPEED * 0.5f, TOP_SPEED * 0.5f, ACCELERATE_TIME_MS * 0.5f))
    return false; //Early exit if the button was toggled

  printf("turning left\n");
  if(!wait_for(WAIT_TIME_MS))
    return false; //Early exit if the button was toggled

  printf("deccelerate turn left\n");
  if(!accelerate_over(0.0f, 0.0f, ACCELERATE_TIME_MS * 0.5f))
    return false; //Early exit if the button was toggled

  printf("stop\n");
  motor_1.stop();
  motor_2.stop();
  if(!wait_for(STOP_TIME_MS))
    return false; //Early exit if the button was toggled

  //Signal that the sequence completed successfully
  return true;
}

/**
 * The entry point of the program.
 */
int main() {
  stdio_init_all();

  //Initialise the LED. We use this to indicate that the sequence is running.
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  //Initialise the two motors
  if(!motor_1.init() || !motor_2.init()) {
    printf("Cannot initialise motors. Check the provided parameters\n");
    return 0;
  }

  while(true) {
    //Has the user has pressed the button to start the sequence
    if(check_button_toggle()) {

      //Turn the Pico's LED on to show that the sequence has started
      gpio_put(PICO_DEFAULT_LED_PIN, true);

      //Run the sequence in a perpetual loop, exiting early if the button is pressed again
      while(sequence());
    }

    //The sequence loop has ended, so turn off the Pico's LED and disable the motors
    gpio_put(PICO_DEFAULT_LED_PIN, false);
    motor_1.disable();
    motor_2.disable();
  }
  return 0;
}
