#include <math.h>
#include <vector>

#include "common/pimoroni_common.hpp"
#include "button.hpp"
#include "rgbled.hpp"
#include "breakout_colourlcd320x240.hpp"

using namespace pimoroni;

static const uint A = 12;
static const uint B = 13;
static const uint X = 14;
static const uint Y = 15;
static const uint LED_R = 6;
static const uint LED_G = 7;
static const uint LED_B = 8;

Button buttonA(A, ACTIVE_LOW);
Button buttonB(B, ACTIVE_LOW);
Button buttonX(X, ACTIVE_LOW);
Button buttonY(Y, ACTIVE_LOW);

RGBLED rgbled(LED_R, LED_G, LED_B, ACTIVE_LOW);

uint16_t buffer[BreakoutColourLCD320x240::WIDTH * BreakoutColourLCD320x240::HEIGHT];
BreakoutColourLCD320x240 lcd(buffer);

void setup_buttons() {
  gpio_set_function(A, GPIO_FUNC_SIO); gpio_set_dir(A, GPIO_IN); gpio_pull_up(A);
  gpio_set_function(B, GPIO_FUNC_SIO); gpio_set_dir(B, GPIO_IN); gpio_pull_up(B);
  gpio_set_function(X, GPIO_FUNC_SIO); gpio_set_dir(X, GPIO_IN); gpio_pull_up(X);
  gpio_set_function(Y, GPIO_FUNC_SIO); gpio_set_dir(Y, GPIO_IN); gpio_pull_up(Y);
}

int main() {
  lcd.init();
  lcd.set_backlight(255);

  struct pt {
    float      x;
    float      y;
    uint8_t    r;
    float     dx;
    float     dy;
    uint16_t pen;
  };

  std::vector<pt> shapes;
  for(int i = 0; i < 100; i++) {
    pt shape;
    shape.x = rand() % lcd.bounds.w;
    shape.y = rand() % lcd.bounds.h;
    shape.r = (rand() % 10) + 3;
    shape.dx = float(rand() % 255) / 64.0f;
    shape.dy = float(rand() % 255) / 64.0f;
    shape.pen = lcd.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  Point text_location(0, 0);

  while(true) {
    if(buttonA.read()) text_location.x -= 1;
    if(buttonB.read()) text_location.x += 1;

    if(buttonX.read()) text_location.y -= 1;
    if(buttonY.read()) text_location.y += 1;
  
    lcd.set_pen(120, 40, 60);
    lcd.clear();

    for(auto &shape : shapes) {
      shape.x += shape.dx;
      shape.y += shape.dy;
      if((shape.x - shape.r) < 0) {
        shape.dx *= -1;
        shape.x = shape.r;
      }
      if((shape.x + shape.r) >= lcd.bounds.w) {
        shape.dx *= -1;
        shape.x = lcd.bounds.w - shape.r;
      }
      if((shape.y - shape.r) < 0) {
        shape.dy *= -1;
        shape.y = shape.r;
      }
      if((shape.y + shape.r) >= lcd.bounds.h) {
        shape.dy *= -1;
        shape.y = lcd.bounds.h - shape.r;
      }

      lcd.set_pen(shape.pen);
      lcd.circle(Point(shape.x, shape.y), shape.r);

    }

    // Since HSV takes a float from 0.0 to 1.0 indicating hue,
    // then we can divide millis by the number of milliseconds
    // we want a full colour cycle to take. 5000 = 5 sec.
    rgbled.set_hsv(float(millis()) / 5000.0f, 1.0f, 1.0f);
    rgbled.set_brightness(127 + sinf(millis() / 100.0f / 3.14159f) * 127);

    lcd.set_pen(255, 255, 255);
    lcd.text("Hello World", text_location, 320);

    // update screen
    lcd.update();
  }

    return 0;
}
