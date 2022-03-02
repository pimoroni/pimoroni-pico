#include <stdio.h>
#include "pico_motor_shim.hpp"

#include "common/pimoroni_common.hpp"
#include "motor.hpp"
#include "button.hpp"
#include "breakout_msa301.hpp"
#include <math.h>

/*
A very basic balancing robot implementation, using an MSA301 to give accelerating values that are passed to the motors using proportional control.
Press "A" to start and stop the balancer
*/

using namespace pimoroni;

static constexpr float TOP_SPEED = 1.0f; //A value between 0 and 1
static constexpr float Z_BIAS_CORRECTION = 0.5f; //A magic number that seems to correct the MSA301's Z bias
static constexpr float PROPORTIONAL = 0.03f;



Button button_a(pico_motor_shim::BUTTON_A, Polarity::ACTIVE_LOW, 0);

Motor motor_1(pico_motor_shim::MOTOR_1_POS, pico_motor_shim::MOTOR_1_NEG);//, Motor::DEFAULT_PWM_FREQUENCY, Motor::DEFAULT_DECAY_MODE);
Motor motor_2(pico_motor_shim::MOTOR_2_POS, pico_motor_shim::MOTOR_2_NEG);//, Motor::DEFAULT_PWM_FREQUENCY, Motor::DEFAULT_DECAY_MODE);

I2C i2c(BOARD::BREAKOUT_GARDEN);
BreakoutMSA301 msa301(&i2c);

static bool button_toggle = false;
static float target_angle = 0.0f;

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
 * Takes and angle and wraps it around so that it stays within a -180 to +180 degree range.
 *
 * Note, it will only work for values between -540 and +540 degrees.
 * This can be resolved by changing the 'if's into 'while's, but for most uses it is unnecessary
 */
float wrap_angle(float angle) {
  if(angle <= -180.0f)
    angle += 360.0f;

  if(angle > 180.0f)
    angle -= 360.0f;

  return angle;
}

/**
 * The entry point of the program.
 */
int main() {
  stdio_init_all();

  //Initialise the LED. We use this to indicate that the sequence is running.
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  for(int i = 0; i < 20; i++) {
    gpio_put(PICO_DEFAULT_LED_PIN, true);
    sleep_ms(250);
    gpio_put(PICO_DEFAULT_LED_PIN, false);
    sleep_ms(250);
  }

  //Initialise the two motors
  if(!motor_1.init() || !motor_2.init()) {
    printf("Cannot initialise motors. Check the provided parameters\n");
    return 0;
  }

  if(!msa301.init()) {
    printf("Cannot initialise msa301. Check that it is connected\n");
    return 0;
  }

  printf("Ready\n");

  while(true) {
    //Turn the Pico's LED on to show that the sequence has started
    gpio_put(PICO_DEFAULT_LED_PIN, true);
    sleep_ms(50);

    //Has the user has pressed the button to start the sequence
    while(check_button_toggle()) {
      float y = msa301.get_y_axis();
      float z = msa301.get_z_axis() + Z_BIAS_CORRECTION;

      float current_angle = (atan2(z, -y) * 180.0f) / M_PI;
      float angle_error = wrap_angle(target_angle - current_angle);
      printf("Y: %f, Z: %f, AngErr: %f\n", y, z, angle_error);

      float output = angle_error * PROPORTIONAL; //No need to clamp this value as set_speed does this internally
      motor_1.set_speed(output);
      motor_2.set_speed(-output);

      sleep_ms(1);
    }

    //The sequence loop has ended, so turn off the Pico's LED and disable the motors
    gpio_put(PICO_DEFAULT_LED_PIN, false);
    motor_1.disable();
    motor_2.disable();
    sleep_ms(50);
  }
  return 0;
}
